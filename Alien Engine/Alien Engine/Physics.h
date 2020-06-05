#pragma once
#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "UtilitiesPhysX.h"

using namespace std;
typedef unsigned int uint;
typedef vector<RaycastHit> RaycastHits;
typedef vector<ComponentCollider*> Colliders;

// Helper ---------------------------------------

class __declspec(dllexport) Physics {
public:
	// General ----------------------------------------

	static float3			GetGravity();
	static void				SetGravity(const float3 gravity);
	static void             SetQueryInitialOverlaping(bool value);
	static void				SetQueryHitTriggers(bool value);
	static void             SetQueryHitBackfaces(bool value);

	// Layers -----------------------------------------

	static void				SetLayer(int layer_0, int layer_1, bool value);
	static int				GetLayerMask(const char* layer);
	static int				GetLayerMask(vector<const char*>& layers);

	// Queries -----------------------------------------

	static bool				Raycast(float3 origin, float3 unit_dir, float max_distance, int layer_mask = -1);
	static bool				Raycast(float3 origin, float3 unit_dir, float max_distance, RaycastHit& hit, int layer_mask = -1);
	static RaycastHits		RaycastAll(float3 origin, float3 unitDir, float maxDistance, int layer_mask = -1);

	static bool				CheckBox(float4x4& trans, float half_size, int layer_mask = -1);
	static bool				BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, int layer_mask = -1);
	static bool				BoxCast(float4x4& trans, float half_size, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask = -1);
	static RaycastHits		BoxCastAll(float4x4 trans, float half_size, float3 unit_dir, float max_dist, int layer_mask = -1);
	static Colliders		OverlapBox(float4x4& trans, float half_size, int layer_mask = -1);

	static bool				CheckSphere(float3 center, float radius, int layer_mask = -1);
	static bool				SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);
	static bool				SphereCast(float3 center, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask = -1);
	static RaycastHits		SphereCastAll(float3 center, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);
	static Colliders		OverlapSphere(float3 center, float radius, int layer_mask = -1);

	static bool				CheckCapsule(float4x4& trans, float height, float radius, int layer_mask = -1);
	static bool				CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);
	static bool				CapsuleCast(float4x4& trans, float height, float radius, float3 unit_dir, float max_dist, RaycastHit& hit, int layer_mask = -1);
	static RaycastHits		CapsuleCastAll(float4x4 trans, float height, float radius, float3 unit_dir, float max_dist, int layer_mask = -1);
	static Colliders		OverlapCapsule(float4x4& trans, float height, float radius, int layer_mask = -1);
};
