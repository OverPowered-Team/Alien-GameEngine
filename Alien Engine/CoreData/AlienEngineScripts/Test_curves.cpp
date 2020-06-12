#include "Test_curves.h"

Test_curves::Test_curves() : Alien()
{
}

Test_curves::~Test_curves()
{
}

void Test_curves::Start()
{
	first_frame = true;
	curve = gocurve->GetComponent<ComponentCurve>();
}

void Test_curves::Update()
{
	if (!first_frame) {
		transform->SetGlobalPosition(curve->curve.ValueAtDistance(actual_pos));

		actual_pos += Time::GetDT() / math::Abs(transform->GetGlobalPosition().Distance(curve->curve.ValueAtDistance(actual_pos + speed * Time::GetDT())));
	}
	else
		first_frame = false;

	if (reset) {
		actual_pos = 0.f;
		transform->SetGlobalPosition(curve->curve.ValueAtDistance(0.f));
		reset = false;
	}
}