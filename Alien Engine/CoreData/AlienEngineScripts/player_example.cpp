#include "player_example.h"
#include "..\..\ComponentBar.h"

player_example::player_example() : Alien()
{
}

player_example::~player_example()
{
}

void player_example::OnDrawGizmos()
{
	RaycastHit hit;
	float4x4 trans = transform->GetGlobalMatrix();
	float3 origin = trans.TranslatePart();
	float3 dir = -trans.WorldY();
	float dist = 10.f;
	int player_mask = 0;
	vector<const char*> layers;
	layers.push_back("Player");
	layers.push_back("Ground");
	int layer_mask = Physics::GetLayerMask(layers);

	if (Physics::Raycast(origin, dir, 10.f, hit, layer_mask))
	{
		Gizmos::DrawLine(origin, hit.point, Color(1.f, 0.f, 0.f));
		float rest_dist = dist - origin.Distance(hit.point);
		Gizmos::DrawLine(hit.point, hit.point + hit.normal * rest_dist, Color(1.f, 0.f, 1.f));
	}
	else
	{
		Gizmos::DrawLine(origin, origin + dir * dist, Color(1.f, 0.f, 0.f));
	}

	//Physics::OverlapSphere(float3(0.f, 0.f, 0.f), 2.f);
	Physics::RaycastAll(origin, dir, 10.f, layer_mask);
}

void player_example::Update()
{

}