//#include "C_SphereCollider.h"
//#include "GameObject.h"
//#include "C_Mesh.h"
//#include "Bullet/include/BulletDynamics/Character/btKinematicCharacterController.h"
//#include "Bullet/include/BulletDynamics/Character/btCharacterControllerInterface.h"
//
//ComponentSphereCollider::ComponentSphereCollider(GameObject* go) :ComponentCollider(go)
//{
//	name.assign("Sphere Collider");
//	type = ComponentType::SPHERE_COLLIDER;
//	radius = 0.5f;
//}
//
//void ComponentSphereCollider::CreateShape(C_Mesh* mesh)
//{
//	if (is_loaded == false)
//	{
//		
//		radius = (mesh != nullptr) ? mesh->mesh_aabb.Size().MaxElement() * 0.5f : 0.5f;
//		center = (mesh != nullptr) ? mesh->mesh_aabb.CenterPoint() : float3::zero;
//	}
//
//	shape = new btSphereShape(1.f);
//}
//
//void ComponentSphereCollider::AdjustShapeToAABB()
//{
//	scaled_center = center;
//	float scaled_size = radius *  linked_go->transform->scale.Abs().MaxElement();
//	shape->setLocalScaling(btVector3(scaled_size, scaled_size  , scaled_size));
//}
//
//void ComponentSphereCollider::SaveCollider(Config& config)
//{
//	config.AddFloat("radius", radius);
//}
//
//void ComponentSphereCollider::LoadCollider(Config& config)
//{
//	radius = config.GetFloat("radius", radius);
//}
//
//void ComponentSphereCollider::DrawPanelColliderInfo()
//{
//	ImGui::Title("Radius", 1);	ImGui::DragFloat("##size", &radius, 0.1f, 0.01f, FLT_MAX);
//}
