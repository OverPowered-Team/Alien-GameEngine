/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Version: v2019.2.0  Build: 7216
  Copyright (c) 2006-2020 Audiokinetic Inc.
*******************************************************************************/

//////////////////////////////////////////////////////////////////////
//
// AkTransferDeferred.cpp
//
// Transfer object used by the deferred lined-up device.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "AkTransferDeferred.h"
#include "AkDeviceDeferredLinedUp.h"

using namespace AK::StreamMgr;

// CAkStmMemViewDeferred
// -----------------------------------------------

// Update a memory view when transfer is complete (updates its owner task).
void CAkStmMemViewDeferred::Update( AKRESULT in_eResult, bool in_bRequiredLowLevelXfer ) 
{ 
	m_pOwner->Update( this, in_eResult, in_bRequiredLowLevelXfer ); 
}

// Cancel a pending memory view. Handshaking is performed with low-level transfer
// in order to determine whether IAkIOHookDeferredBatch::Cancel() will be called.
// If the decision is taken that Cancel() should be called, the view's memory block is untagged 
// atomically to avoid having other tasks trying to attach an observer to it.
// Sync: Owner task must be locked prior to callign this function, device is locked inside.
void CAkStmMemViewDeferred::Cancel(
	IAkIOHookDeferredBatch * in_pLowLevelHook, 
	bool in_bCallLowLevelIO, 
	bool & io_bAllCancelled 
	)
{
	CAkLowLevelTransferDeferred * pLowLevelTransfer = NULL;
	CAkDeviceDeferredLinedUp * pDevice = (CAkDeviceDeferredLinedUp*)m_pOwner->GetDevice();

	// Lock device in order to safely access low-level transfer, ask it if it is possible to cancel it,
	// and perform clean up if it is.
	{
		AkAutoLock<CAkIOThread> deviceLock( *pDevice );

		// Get low-level transfer.
		// Note: pLowLevelTransfer may be NULL if this is a transfer of a cached block that is still in the pending
		// list. Cached transfers are in the pending list between calls to PrepareTransfer() and Update() by the 
		// IO thread. This is protected by the task's status lock.
		pLowLevelTransfer = (CAkLowLevelTransferDeferred*)GetLowLevelTransfer();
		if ( pLowLevelTransfer )
		{
			if ( pLowLevelTransfer->CanCancel() )
			{
				// Need to proceed with cancellation. Untag mem block while still locked.
				pDevice->OnLowLevelTransferCancelled( m_pBlock );
				bool bAllCancelled = io_bAllCancelled;
				pLowLevelTransfer->Cancel(in_pLowLevelHook, in_bCallLowLevelIO, io_bAllCancelled);
				if (io_bAllCancelled && !bAllCancelled)
				{
					AKASSERT(!"Illegal for low-level IO to change io_bAllCancelled from false to true");
					io_bAllCancelled = false;
				}
			}
		}
	}
}



// CAkLowLevelTransferDeferred
// -----------------------------------------------

// Callback from Low-Level I/O.
void CAkLowLevelTransferDeferred::LLIOCallback( 
	AkAsyncIOTransferInfo * in_pTransferInfo,	// Pointer to the AkAsyncIOTransferInfo structure that was passed to corresponding Read() or Write() call.
	AKRESULT				in_eResult			// Result of transfer: AK_Success or AK_Fail (stream becomes invalid).
	)
{
	LLBatchIOCallback(1, &in_pTransferInfo, &in_eResult);
}

// Callback from Low-Level I/O.
void CAkLowLevelTransferDeferred::LLBatchIOCallback(
	AkUInt32				in_uNumTransfers,	///< Number of transfers to process
	AkAsyncIOTransferInfo** in_ppTransferInfo,	///< List of pointers to AkAsyncIOTransferInfo structures that were previously passed in to BatchRead() or BatchWrite()
	AKRESULT*				in_peResult			///< Array of results of each transfer: AK_Success or AK_Fail (streams waiting for this transfer become invalid).
)
{
	AKASSERT(in_ppTransferInfo || !"Invalid transfer info list");
	for (AkUInt32 i = 0; i < in_uNumTransfers; ++i)
	{
		AkAsyncIOTransferInfo* pTransferInfo = in_ppTransferInfo[i];
		AKRESULT eResult = in_peResult[i];

		AKASSERT(pTransferInfo || !"Invalid transfer info");

		// Clean error code from Low-Level IO.
		if (eResult != AK_Success)
		{
			eResult = AK_Fail;
			AK_MONITOR_ERROR(AK::Monitor::ErrorCode_IODevice);
		}

		// Low-level transfer object was stored in pCookie.
		CAkLowLevelTransferDeferred * pLowLevelTransfer = (CAkLowLevelTransferDeferred*)pTransferInfo->pCookie;
		AKASSERT(pLowLevelTransfer || !"Invalid transfer object: corrupted cookie");
		AKASSERT(pLowLevelTransfer->IsValid() || !"Calling the callback on an idle transfer!");

		pLowLevelTransfer->Update(eResult);
	}
}

// Called from LL IO completion static callback.
// Pops all observers, destroys itself, then notifies all cached observers that transfer is complete.
// Sync: None. Locks device to safely pop observers, then
void CAkLowLevelTransferDeferred::Update(
	AKRESULT		in_eResult			// Result of transfer: AK_Success or AK_Fail (stream becomes invalid).
	)
{
	// Lock device, remove all observers and delete this transfer. After this, new data requests trying to 
	// attach to this memory block will be treated as if it was idle.
	CAkDeviceDeferredLinedUp * pDevice = (CAkDeviceDeferredLinedUp*)m_pOwner->GetDevice();
	AkStmMemObserversList observers;
	{
		AkAutoLock<CAkIOThread> deviceLock( *pDevice );

		// Cache observers list and clear it.
		observers = m_observers;
		AKASSERT( observers.First() );	// Length() >= 1.
		m_observers.RemoveAll();

		// OnTransferComplete clears block's low-level transfer, and notifies the memory manager if there was 
		// an error (the block would contain invalid data).
		pDevice->OnLowLevelTransferComplete( observers.First(), in_eResult, !m_pOwner->IsWriteOp() );
	}

	// At this point, "this" has become invalid. Notify all observers cached on stack.
	// Note: in order to report correct profiling info, pass bRequiredLowLevelXfer = true only 
	// to the first observer.

	// Remove and notify all observers of this transfer.
	bool bRequiredLowLevelXfer = true;
	AkStmMemObserversList::IteratorEx it = observers.BeginEx();
	while ( it != observers.End() )
	{
		CAkStmMemViewDeferred * pMemView = (*it);
		it = observers.Erase( it );
		pMemView->Update( in_eResult, bRequiredLowLevelXfer );
		bRequiredLowLevelXfer = false;
	}
	observers.Term();
}

// Cancel a transfer: calls Cancel() on the Low-Level IO if required to do so.
// Observers who call this function must set their status to cancelled first.
// Sync: The owner task must be locked.
void CAkLowLevelTransferDeferred::Cancel( 
	IAkIOHookDeferredBatch * in_pLowLevelHook, 
	bool in_bCallLowLevelIO, 
	bool & io_bAllCancelled 
	)
{
	if ( in_bCallLowLevelIO )
	{
		if ( !m_bWasLLIOCancelCalled )
		{
			// Call Cancel() on Low-Level IO. Pass in file descriptor of the task that initiated the transfer.
			IAkIOHookDeferredBatch::BatchIoTransferItem pIoTransferItem[1];
			pIoTransferItem[0].pFileDesc = m_pOwner->GetFileDesc();
			pIoTransferItem[0].pTransferInfo = &info;
			bool* ppbAllCancelled[] = { &io_bAllCancelled };
			in_pLowLevelHook->BatchCancel( 1, pIoTransferItem, ppbAllCancelled);
		}
		else
		{
			// Cancel() was already called. Clear io_bAllCancelled, as we still need to 
			// ask the Low-Level IO to cancel next transfers, if applicable.
			io_bAllCancelled = false;
		}
	}
	m_bWasLLIOCancelCalled = true;
}

// Ask the low-level transfer if it can be cancelled. It can if and only if 
// there is only one observer to this transfer, and it is the caller.
// Sync: Owner task must be locked, device must be locked.
bool CAkLowLevelTransferDeferred::CanCancel()
{
	AKASSERT( m_observers.First() || !"No observers for this low-level transfer" );

	// Can cancel a low-level transfer only if there is just one observer, and this observer's owner
	// has to be the same as the low-level transfer's owner, otherwise there would be a locking issue:
	// low-level cancellation is synchronized with task locks.
	AkStmMemObserversList::Iterator it = m_observers.Begin();
	++it;
	if ( it != m_observers.End() )
	{
		// There is more than one observer. Cannot cancel.
		return false;
	}
	else
	{
		// There is just one observer. Check its owner.
		return ( m_observers.First()->GetOwner() == m_pOwner );
	}
}
