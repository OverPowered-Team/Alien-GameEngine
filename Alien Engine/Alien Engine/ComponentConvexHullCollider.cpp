#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentConvexHullCollider.h"
#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"
#include "ComponentRigidBody.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "mmgr/mmgr.h"

ComponentConvexHullCollider::ComponentConvexHullCollider(GameObject* go) : ComponentCollider(go)
{
	name.assign("Convex Hull Collider");
	type = ComponentType::CONVEX_HULL_COLLIDER;
	size = float3::one();

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
