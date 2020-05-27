#include "TestFollowCurve.h"

TestFollowCurve::TestFollowCurve() : Alien()
{
}

TestFollowCurve::~TestFollowCurve()
{
}

void TestFollowCurve::Start()
{
	at = 0.f;
}

void TestFollowCurve::Update()
{
	if (reset) {
		at = 0.f;
		reset = false;
	}
	
	transform->SetGlobalPosition(curve->GetComponent<ComponentCurve>()->curve.ValueAtDistance(at));
	transform->SetLocalRotation(Quat::RotateFromTo(transform->up.Normalized(), curve->GetComponent<ComponentCurve>()->curve.NormalAtDistance(at).Normalized()) * transform->GetLocalRotation());
	at += speed * Time::GetDT();
}