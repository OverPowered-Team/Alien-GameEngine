#include "Application.h"
#include "ComponentPhysics.h"
#include "ComponentSphereCollider.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "imgui/imgui.h"
#include "mmgr/mmgr.h"

ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;
	
	InitializeRadius();
	shape = App->physx->CreateShape(PxSphereGeometry(CalculateRadius()));
	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED);
	InitCollider();
}
void ComponentSphereCollider::InitializeRadius()
{
	radius = GetLocalMeshAabbSize().MaxElement() * 0.5f;
}

const float ComponentSphereCollider::CalculateRadius()
{
	return transform->GetGlobalScale().MaxElement() * radius;
}

PxShape* ComponentSphereCollider::RecreateSphereShape()
{
	if (!shape)
		return nullptr;

	BeginUpdateShape();
	PxTransform trans = shape->getLocalPose();
	float _radius = CalculateRadius();
	trans.p = F3_TO_PXVEC3(center.Mul(transform->GetGlobalScale()));
	shape->setLocalPose(trans);
	shape->setGeometry(PxSphereGeometry(_radius));
	EndUpdateShape();

	return shape;
}

void ComponentSphereCollider::ScaleChanged()
{
	RecreateSphereShape();
}


void ComponentSphereCollider::DrawSpecificInspector()
{
	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &radius, 0.01f, 0.01f, FLT_MAX)) { RecreateSphereShape(); }

}

void ComponentSphereCollider::Reset()
{
	ComponentCollider::Reset();
	InitializeRadius();
	RecreateSphereShape();
}

void ComponentSphereCollider::SetRadius(float radius)
{
	this->radius = radius;
	RecreateSphereShape();
}

void ComponentSphereCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
}

void ComponentSphereCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	radius = (to_load->GetNumber("Radius"));
	RecreateSphereShape();
}

//void ComponentSphereCollider::QueryMesh(ComponentMesh* mesh)
//{
//	if (mesh == nullptr) return;
//
//	ResourceMesh* data =  mesh->mesh;
//	float* vertices = data->vertex;
//	
//	float3* points = new float3[data->num_vertex];
//	for (int i = 0; i < data->num_vertex; ++i)
//	{
//		int pos = i * 3;
//		points[i] = float3(vertices[pos], vertices[pos + 1], vertices[pos + 2]);
//	}
//
//	Sphere sphere = Sphere::FastEnclosingSphere(points, data->num_vertex);
//	delete[]points;
//
//	SetRadius(sphere.Diameter() * 0.5f );
//	SetCenter(sphere.Centroid());
//}
