
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
	shape = App->physx->CreateShape(PxSphereGeometry(.5f));
	radius = 0.5F;
	InitCollider();
}

void ComponentSphereCollider::QueryMesh(ComponentMesh* mesh)
{
	if (mesh == nullptr) return;

	//ResourceMesh* data = mesh->mesh;
	//float* vertices = data->vertex;

	//float3* points = new float3[data->num_vertex];
	//for (int i = 0; i < data->num_vertex; ++i)
	//{
	//	int pos = i * 3;
	//	points[i] = float3(vertices[pos], vertices[pos + 1], vertices[pos + 2]);
	//}

	//Sphere sphere = Sphere::FastEnclosingSphere(points, data->num_vertex);
	//
	//delete[]points;

	//SetRadius(sphere.Diameter() * 0.5f);
	//SetCenter(sphere.Centroid());

	SetRadius(mesh->local_aabb.Size().MaxElement() * 0.5f);
	SetCenter(mesh->local_aabb.CenterPoint());
	
}

void ComponentSphereCollider::SetRadius(float value)
{
	radius = value;
	PxSphereGeometry geo(radius * physics->scale.MaxElement());
	BeginUpdateShape();
	shape->setGeometry(geo);
	EndUpdateShape();
}

void ComponentSphereCollider::ScaleChanged()
{
	BeginUpdateShape(true);
	SetRadius(radius);
	SetCenter(center);
	EndUpdateShape(true);
}

void ComponentSphereCollider::DrawSpecificInspector()
{
	float current_radius = radius;
	ImGui::Title("Radius", 1);	if (ImGui::DragFloat("##radius", &current_radius, 0.01f, 0.01f, FLT_MAX)) { SetRadius(current_radius); }
}

void ComponentSphereCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetNumber("Radius", radius);
}

void ComponentSphereCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetRadius(to_load->GetNumber("Radius"));
}

void ComponentSphereCollider::Reset()
{
	BeginUpdateShape(true);
	SetRadius(0.5f);
	SetCenter(float3::zero());
	EndUpdateShape(true);
}
