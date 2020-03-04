#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ComponentCollider.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"
#include "ComponentScript.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ReturnZ.h"
#include "Alien.h"

ComponentCollider::ComponentCollider(GameObject* go) : Component(go)
{
	// GameObject Components 
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;

	// Default values 
	SetCenter(float3::zero());
}

ComponentCollider::~ComponentCollider()
{
	if (rigid_body)
	{
		rigid_body->RemoveCollider();
	}

	App->physics->RemoveBody(aux_body);
	App->physics->RemoveDetector(detector); // TestCallback

	delete detector;
	delete aux_body;
	delete shape;
}

void ComponentCollider::Init()
{
	// Create aux body 
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.F, nullptr, nullptr);
	aux_body = new btRigidBody(rbInfo);
	aux_body->setUserPointer(game_object_attached);

	// Create shape 

	CreateShape();
	aux_body->setCollisionShape(shape);
	App->physics->AddBody(aux_body);

	// Create detector  // TestCallback
	detector = new btPairCachingGhostObject();
	detector->setCollisionShape(shape);
	detector->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	App->physics->AddDetector(detector);

	// Search Rigid Body 

	ComponentRigidBody* new_rb = game_object_attached->GetComponent<ComponentRigidBody>();

	if (new_rb != nullptr)
	{
		new_rb->AddCollider(this);
	}

	// Search Valid Script // TestCallback

	ComponentScript* new_script = game_object_attached->GetComponent<ComponentScript>();

	if (new_script != nullptr && new_script->need_alien == true)
	{
		alien_script = (Alien*)new_script->data_ptr;
	}

	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}


// Colliders Functions --------------------------------

void ComponentCollider::SetCenter(float3 value)
{
	center = value;
	float3 current_scale = transform->GetGlobalScale();
	final_center = center.Mul(current_scale);
}

void ComponentCollider::SetIsTrigger(bool value)
{
	is_trigger = value;
	(is_trigger)
		? aux_body->setCollisionFlags(aux_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE)
		: aux_body->setCollisionFlags(aux_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void ComponentCollider::SetBouncing(const float value)
{
	bouncing = math::Clamp(value, 0.f, 1.f);
	(rigid_body) ? rigid_body->body->setRestitution(bouncing)
		: aux_body->setRestitution(bouncing);
}

void ComponentCollider::SetFriction(const float value)
{
	friction = value;
	(rigid_body) ? rigid_body->body->setFriction(friction)
		: aux_body->setFriction(friction);
}

void ComponentCollider::SetAngularFriction(const float value)
{
	angular_friction = value;
	(rigid_body) ? rigid_body->body->setRollingFriction(angular_friction)
		: aux_body->setRollingFriction(angular_friction);
}

void ComponentCollider::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetBoolean("IsTrigger", is_trigger);
	to_save->SetFloat3("Center", center);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);
}

void ComponentCollider::LoadComponent(JSONArraypack* to_load)
{
	SetCenter(to_load->GetFloat3("Center"));
	SetIsTrigger(is_trigger = to_load->GetBoolean("IsTrigger"));
	SetBouncing(bouncing = to_load->GetNumber("Bouncing"));
	SetFriction(friction = to_load->GetNumber("Friction"));
	SetAngularFriction(angular_friction = to_load->GetNumber("AngularFriction"));

	UpdateShape();
}

void ComponentCollider::CreateShape()
{
	if (!WrapMesh()) UpdateShape();
}

void ComponentCollider::Update()
{
	static float3 last_scale = transform->GetGlobalScale();
	float3 current_scale = transform->GetGlobalScale();

	if (!last_scale.Equals(current_scale))
	{
		last_scale = current_scale;
		UpdateShape();
	}

	if (rigid_body == nullptr)
	{
		aux_body->setWorldTransform(ToBtTransform(transform->GetGlobalPosition() + GetWorldCenter(), transform->GetGlobalRotation()));
	}

	detector->setWorldTransform(aux_body->getCenterOfMassTransform());
	detector->setActivationState(true);

	//if (test_callbacks == true)
	if (alien_script != nullptr)
	{
		int numObjectsInGhost = 0;
		int test = 0;
		numObjectsInGhost = detector->getNumOverlappingObjects(); //numObjectsInGhost is set to 0xcdcdcdcd

		for (auto& x : collisions)
		{
			x.second = false;
		}

		for (int i = 0; i < numObjectsInGhost; ++i)
		{
			btCollisionObject* obj = detector->getOverlappingObject(i);
			btGhostObject* ghost = dynamic_cast<btGhostObject*>(obj);
			if (ghost)
			{
				ComponentCollider* coll = (ComponentCollider*)ghost->getUserPointer();
				std::map<ComponentCollider*, bool>::iterator search = collisions.find(coll);
				if (search != collisions.end())
				{
					collisions[coll] = true;
					alien_script->OnTrigger(coll);
				}
				else
				{
					collisions[coll] = true;
					alien_script->OnTriggerEnter(coll);
				}

				++test;
			}
		}

		std::map<ComponentCollider*, bool>::iterator itr = collisions.begin();

		while (itr != collisions.end()) 
		{
			if (itr->second == false) {
				alien_script->OnTriggerExit(itr->first);
				itr = collisions.erase(itr);
			}
			else {
				++itr;
			}
		}


		LOG_ENGINE("%i", test);
		//detector.overl
		//btManifoldArray manifoldArray; 
		//btBroadphasePairArray& pairArray = detector->getover(); 
		//int numPairs = pairArray.size();

		//for (int i = 0; i < numPairs; ++i) {
		//	manifoldArray.clear();

		//	const btBroadphasePair& pair = pairArray[i];

		//	btBroadphasePair* collisionPair = App->physics->world->getPairCache()->findPair(
		//		pair.m_pProxy0, pair.m_pProxy1);

		//	if (!collisionPair) continue;

		//	if (collisionPair->m_algorithm)
		//		collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

		//	for (int j = 0; j < manifoldArray.size(); j++) {
		//		btPersistentManifold* manifold = manifoldArray[j];

		//		bool isFirstBody = manifold->getBody0() == ghostObject;

		//		btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

		//		for (int p = 0; p < manifold->getNumContacts(); ++p) {
		//			const btManifoldPoint& pt = manifold->getContactPoint(p);

		//			if (pt.getDistance() < 0.f) {
		//				const btVector3& ptA = pt.getPositionWorldOnA(); const
		//					btVector3& ptB = pt.getPositionWorldOnB(); const btVector3& normalOnB =
		//					pt.m_normalWorldOnB;

		//				// handle collisions here
		//			}
		//		}
		//	}
		//}



	}

}

void ComponentCollider::DrawScene()
{
	if (game_object_attached->IsSelected()/* && App->scene->editor_mode*/)
	{
		App->physics->RenderCollider(this);
	}
}

void ComponentCollider::Reset()
{
	SetCenter(float3::zero());
	SetIsTrigger(false);
	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);
}

bool ComponentCollider::DrawInspector()
{
	static bool check;

	check = enabled;

	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}

	float3 current_center = center;
	bool current_is_trigger = is_trigger;
	float current_bouncing = bouncing;
	float current_friction = friction;
	float current_angular_friction = angular_friction;

	ImGui::SameLine();

	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		ImGui::Title("Callbacks", 1);		ImGui::Checkbox("##callback", &test_callbacks);
		ImGui::Title("Center", 1);			if (ImGui::DragFloat3("##center", current_center.ptr(), 0.1f)) { SetCenter(current_center); }

		DrawSpecificInspector();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Is Trigger", 1);		if (ImGui::Checkbox("##is_trigger", &current_is_trigger)) { SetIsTrigger(current_is_trigger); }
		ImGui::Spacing();
		ImGui::Title("Physic Material", 1); ImGui::Text("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Bouncing", 2);	    if (ImGui::DragFloat("##bouncing", &current_bouncing, 0.01f, 0.00f, 1.f)) { SetBouncing(current_bouncing); }
		ImGui::Title("Linear Fric.", 2);	if (ImGui::DragFloat("##friction", &current_friction, 0.01f, 0.00f, FLT_MAX)) { SetFriction(current_friction); }
		ImGui::Title("Angular Fric.", 2);	if (ImGui::DragFloat("##angular_friction", &current_angular_friction, 0.01f, 0.00f, FLT_MAX)) { SetAngularFriction(current_angular_friction); }
		ImGui::Spacing();
	}

	return true;
}

float3 ComponentCollider::GetWorldCenter()
{
	return transform->GetGlobalRotation().Mul(final_center);
}

