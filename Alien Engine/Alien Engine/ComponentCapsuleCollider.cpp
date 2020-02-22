//#include "C_CapsuleCollider.h"
//#include "C_Mesh.h"
//#include "GameObject.h"
//
//ComponentCapsuleCollider::ComponentCapsuleCollider(GameObject* go) : ComponentCollider(go)
//{
//	name.assign("Capsule Collider");
//	type = ComponentType::CAPSULE_COLLIDER;
//	capsule_type = CapsuleType::Y;
//}
//
//void ComponentCapsuleCollider::CreateShape(C_Mesh* mesh)
//{
//	if (is_loaded == false)
//	{
//		capsule_type = CapsuleType::Y;
//		float3 size = (mesh != nullptr) ? mesh->mesh_aabb.Size() : size = float3::one;
//		center = (mesh != nullptr) ? mesh->mesh_aabb.CenterPoint() : float3::zero;
//
//		if (mesh != nullptr)
//		{
//			switch (capsule_type)
//			{
//			case ComponentCapsuleCollider::CapsuleType::X:
//				height = size.x;
//				radius = math::Max(size.z, size.y) * 0.5f;
//				break;
//			case ComponentCapsuleCollider::CapsuleType::Y:
//				height = size.y;
//				radius = math::Max(size.z, size.x) * 0.5f;
//				break;
//			case ComponentCapsuleCollider::CapsuleType::Z:
//				height = size.z;
//				radius = math::Max(size.y, size.x) * 0.5f;
//				break;
//			}
//		}
//
//	}
//
//	switch (capsule_type)
//	{
//	case ComponentCapsuleCollider::CapsuleType::X:
//		shape = new btCapsuleShapeX(1.f, 1.f);
//		break;
//	case ComponentCapsuleCollider::CapsuleType::Y:
//		shape = new btCapsuleShape(1.f, 1.f);
//		break;
//	case ComponentCapsuleCollider::CapsuleType::Z:
//		shape = new btCapsuleShapeZ(1.f, 1.f);
//		break;
//	}
//}
//
//void ComponentCapsuleCollider::AdjustShape()
//{
//	static CapsuleType last_type = capsule_type;
//
//	if (last_type != capsule_type)
//	{
//		C_RigidBody* rb = linked_go->GetComponent<C_RigidBody>();
//		bool update_rb = false;
//
//		if (rb != nullptr && rb->collider == this)
//			update_rb = true;
//
//		delete shape;
//
//		switch (capsule_type)
//		{
//		case ComponentCapsuleCollider::CapsuleType::X:
//			shape = new btCapsuleShapeX(1.f, 1.f);
//			break;
//		case ComponentCapsuleCollider::CapsuleType::Y:
//			shape = new btCapsuleShape(1.f, 1.f);
//			break;
//		case ComponentCapsuleCollider::CapsuleType::Z:
//			shape = new btCapsuleShapeZ(1.f, 1.f);
//			break;
//		}
//
//		last_type = capsule_type;
//
//		aux_body->setCollisionShape(shape);
//
//		if (update_rb)
//		{
//			rb->body->setCollisionShape(shape);
//		}
//	}
//
//	float3 scale = linked_go->transform->scale.Abs();
//	float height_scaled = height;
//	float radius_scaled = radius;
//
//	scaled_center = center;
//
//	switch (capsule_type)
//	{
//	case ComponentCapsuleCollider::CapsuleType::X:
//		height_scaled *= scale.x;
//		radius_scaled *= math::Max(scale.z, scale.y);
//		shape->setLocalScaling(btVector3(height_scaled, radius_scaled, radius_scaled));
//		break;
//	case ComponentCapsuleCollider::CapsuleType::Y:
//		height_scaled *= scale.y;
//		radius_scaled *= math::Max(scale.z, scale.x);
//		shape->setLocalScaling(btVector3(radius_scaled, height_scaled, radius_scaled));
//		break;
//	case ComponentCapsuleCollider::CapsuleType::Z:
//		height_scaled *= scale.z;
//		radius_scaled *= math::Max(scale.x, scale.y);
//		shape->setLocalScaling(btVector3(radius_scaled, radius_scaled, height_scaled));
//
//		break;
//	}
//	
//}
//
//void ComponentCapsuleCollider::SaveCollider(Config& config)
//{
//	config.AddFloat("radius", radius);
//	config.AddFloat("height", height);
//	config.AddInt("capsule_type", (int)capsule_type);
//}
//
//void ComponentCapsuleCollider::LoadCollider(Config& config)
//{
//	radius = config.GetFloat("radius", radius);
//	height = config.GetFloat("height", height);
//	capsule_type =(CapsuleType) config.GetInt("capsule_type", (int)capsule_type);
//}
//
//void ComponentCapsuleCollider::DrawPanelColliderInfo()
//{
//	ImGui::Title("Radius", 1);	ImGui::DragFloat("##radius", &radius, 0.1f, 0.01f, FLT_MAX);
//	ImGui::Title("Height", 1);	ImGui::DragFloat("##height", &height, 0.1f, 0.01f, FLT_MAX);
//	ImGui::Title("Direction");
//
//	static const char* items[] = { "X", "Y", "Z"};
//	static const char* current_item = items[1];
//
//	if (ImGui::BeginComboEx(std::string("##viewport_modes").c_str(), std::string(" " + std::string(current_item)).c_str(), 200, ImGuiComboFlags_NoArrowButton))
//	{
//		for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
//		{
//			bool is_selected = (current_item == items[n]);
//
//			if (ImGui::Selectable(std::string("   " + std::string(items[n])).c_str(), is_selected))
//			{
//				current_item = items[n];
//
//				if (current_item == "X")				capsule_type = CapsuleType::X;
//				else if (current_item == "Y")			capsule_type = CapsuleType::Y;
//				else if (current_item == "Z")			capsule_type = CapsuleType::Z;
//			}
//
//			if (is_selected)
//			{
//				ImGui::SetItemDefaultFocus();
//			}
//		}
//
//		ImGui::EndCombo();
//	}
//
//}
