#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentConvexHullCollider.h"
#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "mmgr/mmgr.h"

#include "ComponentPhysics.h"

ComponentConvexHullCollider::ComponentConvexHullCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Convex Hull Collider");
	type = ComponentType::CONVEX_HULL_COLLIDER;
	size = float3::one();

	App->SendAlienEvent(this, AlienEventType::COLLIDER_ADDED); // we need the actor before creating the shape

	PxShape* convexShape = CreateConvexMesh(go);
	if (convexShape)
		shape = convexShape;
	else
		shape = App->physx->CreateShape(PxBoxGeometry(.5f, .5f, .5f)); // if convex mesh cook fail, create default cube
	
	InitCollider();
}

PxShape* ComponentConvexHullCollider::CreateConvexMesh(const GameObject* go)
{
	// try to get mesh from gameobject
	const ComponentMesh* mesh = go->GetComponent<ComponentMesh>();
	if (!mesh)
		return nullptr;

	uint nverts = mesh->mesh->num_vertex;
	uint ntris = mesh->mesh->num_faces;
	uint nindex = mesh->mesh->num_index;

	PxConvexMeshDesc convexDesc;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.points.count = nverts;
	convexDesc.points.stride = sizeof(float) * 3;
	convexDesc.points.data = mesh->mesh->vertex;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;

	if (!App->physx->px_cooking->cookConvexMesh(convexDesc, buf, &result))
		return nullptr;

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = App->physx->px_physics->createConvexMesh(input);
	PxMaterial* mat = App->physx->CreateMaterial(0.5f, 0.5f, 0.5f);
	
	PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*physics->actor, PxConvexMeshGeometry(convexMesh), *mat);
	if (!aConvexShape)
		return nullptr;

	return aConvexShape;
}

void ComponentConvexHullCollider::SetSize(float3 size)
{
}

void ComponentConvexHullCollider::SaveComponent(JSONArraypack* to_save)
{
	ComponentCollider::SaveComponent(to_save);
	to_save->SetFloat3("Size", size);

}

void ComponentConvexHullCollider::LoadComponent(JSONArraypack* to_load)
{
	ComponentCollider::LoadComponent(to_load);
	SetSize(to_load->GetFloat3("Size"));
}

void ComponentConvexHullCollider::CreateDefaultShape()
{

}

void ComponentConvexHullCollider::UpdateShape()
{
	
}


void ComponentConvexHullCollider::DrawScene()
{
	if (game_object_attached->IsSelected() && App->physics->debug_physics == false)
	{
		App->physics->DrawConvexCollider(this);
	}
}

void ComponentConvexHullCollider::DrawSpecificInspector()
{
	float3 current_size = size;

	ImGui::Title("Size", 1);	if (ImGui::DragFloat3("##size", current_size.ptr(), 0.1f, 0.01f, FLT_MAX)) { SetSize(current_size); }
}
