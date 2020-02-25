#include "Application.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentCollider.h"
#include "GameObject.h"
#include "ModuleInput.h"

ComponentRigidBody::ComponentRigidBody(GameObject* go) : Component(go)
{
	type = ComponentType::RIGID_BODY;
	transform = (transform == nullptr) ? game_object_attached->GetComponent<ComponentTransform>() : transform;
	mass = 1.f;
	// Create compund shape --------------------------

	compound_shape = new btCompoundShape(true, 4);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, compound_shape);

	body = new btRigidBody(rbInfo);
	body->setUserPointer(go);

	SetBodyTranform(transform->GetGlobalPosition(), transform->GetGlobalRotation());
	App->physics->AddBody(body);
}

ComponentRigidBody::~ComponentRigidBody()
{
	for (int i = 0; i < body->getNumConstraintRefs(); ++i)
	{
		//C_JointP2P * joint =(C_JointP2P *) body->getConstraintRef(i)->getUserConstraintPtr();
		//joint->BodyDeleted(linked_go);
		body->removeConstraintRef(body->getConstraintRef(i));
	}

	App->physics->RemoveBody(body);

	delete body;
	delete compound_shape;
}

void ComponentRigidBody::Update()
{
	// Update Local Vars  ------------------------

	float3 go_offset =/* (compound_shape != nullptr) ? compound_shape.tra() :*/ float3::zero();
	//float current_mass = (game_object_attached->is_static) ? 0.f : mass;						// Update Mass

	 // Update Inertia ---------------------------


	body->activate(true);
	// Update Rigid Body Vars ---------------------

	// Set Go Transform ---------------------------

	//SetBodyTranform(transform->GetGlobalPosition() + go_offset, transform->GetGlobalRotation());

	//if (Time::GetGameState() == Time::GameState::PAUSE)
	//{
	//	SetBodyTranform(transform->GetGlobalPosition() + go_offset, transform->GetGlobalRotation());
	//}
	//else
	//{
		//btTransform bt_transform = body->getCenterOfMassTransform();
		//btQuaternion rotation = bt_transform.getRotation();
		//btVector3 position = bt_transform.getOrigin() - ToBtVector3(go_offset);

		//body->activate(true);
		//transform->SetGlobalPosition(float3(position));
		//transform->SetGlobalRotation(math::Quat((btScalar*)rotation));

	//	// Apply Forces ----------------------

	//	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	//	{
	//		if (!force_to_apply[i].Equals(float3::zero()))
	//		{
	//			switch ((ForceMode)i)
	//			{
	//			case ForceMode::FORCE:
	//				body->applyCentralForce(ToBtVector3(force_to_apply[i]));
	//				break;
	//			case ForceMode::IMPULSE:
	//				body->applyCentralImpulse(ToBtVector3(force_to_apply[i]));
	//				break;
	//			}
	//			force_to_apply[i] = float3::zero();
	//		}
	//	}

	//	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	//	{
	//		if (!torque_to_apply[i].Equals(float3::zero()))
	//		{
	//			switch ((ForceMode)i)
	//			{
	//			case ForceMode::FORCE:
	//				body->applyTorque(ToBtVector3(torque_to_apply[i]));
	//				break;
	//			case ForceMode::IMPULSE:
	//				body->applyTorqueImpulse(ToBtVector3(torque_to_apply[i]));
	//				break;
	//			}

	//			torque_to_apply[i] = float3::zero();
	//		}
	//	}
	//}
}

void ComponentRigidBody::Draw()
{

}

bool ComponentRigidBody::DrawInspector()
{

	// RigidBody Config --------------------------------------

	if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();

		ImGui::Title("Mass", 1);			ImGui::DragFloat("##mass", &mass, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Drag", 1);			ImGui::DragFloat("##drag", &drag, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Angular Drag", 1);	ImGui::DragFloat("##angular_drag", &angular_drag, 0.01f, 0.00f, FLT_MAX);

		ImGui::Title("Freeze", 1);	ImGui::Text("");
		ImGui::Spacing();
		ImGui::PushID(freeze_position);
		ImGui::Title("Position", 2);
		ImGui::Checkbox("X", &freeze_position[0]); ImGui::SameLine();
		ImGui::Checkbox("Y", &freeze_position[1]); ImGui::SameLine();
		ImGui::Checkbox("Z", &freeze_position[2]);
		ImGui::PopID();
		ImGui::PushID(freeze_rotation);
		ImGui::Title("Rotation", 2);
		ImGui::Checkbox("X", &freeze_rotation[0]); ImGui::SameLine();
		ImGui::Checkbox("Y", &freeze_rotation[1]); ImGui::SameLine();
		ImGui::Checkbox("Z", &freeze_rotation[2]);
		ImGui::PopID();
		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Title("Is Trigger", 1);		ImGui::Checkbox("##is_trigger", &is_trigger);
		ImGui::Spacing();
		ImGui::Title("Physic Material", 1); ImGui::Text("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Title("Bouncing", 2);	    ImGui::DragFloat("##bouncing", &bouncing, 0.01f, 0.00f, 1.f);
		ImGui::Title("Linear Fric.", 2);	ImGui::DragFloat("##friction", &friction, 0.01f, 0.00f, FLT_MAX);
		ImGui::Title("Angular Fric.", 2);	ImGui::DragFloat("##angular_friction", &angular_friction, 0.01f, 0.00f, FLT_MAX);
		ImGui::Spacing();
	}

	// Colliders Config -------------------------------------

	SetIsTrigger(is_trigger);
	SetMass(mass);
	SetDrag(drag);
	SetAngularDrag(angular_drag);
	SetBouncing(bouncing);
	SetFriction(friction);
	SetAngularFriction(angular_friction);

	btVector3 freeze_p((float)!freeze_position[0], (float)!freeze_position[1], (float)!freeze_position[2]);
	btVector3 freeze_r((float)!freeze_rotation[0], (float)!freeze_rotation[1], (float)!freeze_rotation[2]);

	if (body->getLinearFactor() != freeze_p)
	{
		body->activate(true);
		body->setLinearFactor(freeze_p);
	}
	if (body->getAngularFactor() != freeze_r)
	{
		body->activate(true);
		body->setAngularFactor(freeze_r);
	}
	return true;
}


void ComponentRigidBody::Reset()
{
	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		force_to_apply[i].zero();
		torque_to_apply[i].zero();
	}

	SetMass(1.0f);
	SetDrag(0.f);
	SetAngularDrag(0.f);

	SetBouncing(0.1f);
	SetFriction(0.5f);
	SetAngularFriction(0.1f);

}

void ComponentRigidBody::Clone(Component* clone)
{

}

void ComponentRigidBody::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);

	to_save->SetBoolean("IsTrigger", is_trigger);
	to_save->SetNumber("Bouncing", bouncing);
	to_save->SetNumber("Friction", friction);
	to_save->SetNumber("AngularFriction", angular_friction);

	to_save->SetNumber("Mass", mass);
	to_save->SetNumber("Drag", drag);
	to_save->SetNumber("AngularDrag", angular_drag);

	to_save->SetBoolean("FreezePosX", freeze_position[0]);
	to_save->SetBoolean("FreezePosY", freeze_position[1]);
	to_save->SetBoolean("FreezePosZ", freeze_position[2]);

	to_save->SetBoolean("FreezeRotX", freeze_rotation[0]);
	to_save->SetBoolean("FreezeRotY", freeze_rotation[1]);
	to_save->SetBoolean("FreezeRotZ", freeze_rotation[2]);
}

void ComponentRigidBody::LoadComponent(JSONArraypack* to_load)
{

	bouncing = to_load->GetNumber("Bouncing");
	friction = to_load->GetNumber("Friction");
	angular_friction = to_load->GetNumber("AngularFriction");
	is_trigger = to_load->GetBoolean("IsTrigger");

	mass = to_load->GetNumber("Mass");
	drag = to_load->GetNumber("Drag");
	angular_drag = to_load->GetNumber("AngularDrag");

	freeze_position[0] = to_load->GetBoolean("FreezePosX");
	freeze_position[1] = to_load->GetBoolean("FreezePosY");
	freeze_position[2] = to_load->GetBoolean("FreezePosZ");

	freeze_rotation[0] = to_load->GetBoolean("FreezeRotX");
	freeze_rotation[1] = to_load->GetBoolean("FreezeRotY");
	freeze_rotation[2] = to_load->GetBoolean("FreezeRotZ");

	SetIsTrigger(is_trigger);
}

// Forces Functions ------------------------------

void ComponentRigidBody::AddForce(const float3 force, ForceMode mode, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = transform->global_transformation.RotatePart().Mul(final_force);
	}

	force_to_apply[(uint)mode] += final_force;
}

void ComponentRigidBody::AddTorque(const float3 force, ForceMode mode, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = transform->global_transformation.RotatePart().Mul(final_force);
	}

	torque_to_apply[(uint)mode] += final_force;
}

// Rigid Body Values ----------------------------

void ComponentRigidBody::SetMass(const float value)
{
	if (value != body->getMass())
	{
		mass = value;
		compound_shape->calculateLocalInertia(mass, inertia);
		body->setMassProps(mass, inertia);
	}
}

void ComponentRigidBody::SetDrag(const float value)
{
	if (value != body->getLinearDamping())
	{
		drag = value;
		body->setDamping(drag, angular_drag);
	}
}

void ComponentRigidBody::SetAngularDrag(const float value)
{
	if (value != body->getAngularDamping())
	{
		angular_drag = value;
		body->setDamping(drag, angular_drag);
	}
}

void ComponentRigidBody::SetVelocity(float3 velocity)
{
	body->setLinearVelocity(ToBtVector3(velocity));
}

float3 ComponentRigidBody::GetVelocity()
{
	return float3(&body->getLinearVelocity()[0]);
}

void ComponentRigidBody::SetAngularVelocity(float3 velocity)
{
	body->setAngularVelocity(ToBtVector3(velocity));
}
float3 ComponentRigidBody::GetAngularVelocity()
{
	return float3(&body->getAngularVelocity()[0]);
}

// Colliders values --------------------------------

void ComponentRigidBody::SetIsTrigger(bool value)
{
	if (value == true)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	is_trigger = value;
}

void ComponentRigidBody::SetBouncing(const float value)
{
	if (value != body->getRestitution())
	{
		bouncing = math::Clamp(value, 0.f, 1.f);
		body->setRestitution(bouncing);
	}
}

void ComponentRigidBody::SetFriction(const float value)
{
	if (value != body->getFriction())
	{
		friction = value;
		body->setFriction(friction);
	}
}

void ComponentRigidBody::SetAngularFriction(const float value)
{
	if (value != body->getRollingFriction())
	{
		angular_drag = value;
		body->setRollingFriction(angular_drag);
	}
}

void ComponentRigidBody::CreateBody()
{
	float3 position;
	Quat rotation;

	ComponentMesh* mesh = game_object_attached->GetComponent<ComponentMesh>();

	if (mesh != nullptr)
	{
		position = game_object_attached->GetGlobalOBB().CenterPoint();
	}
	else
	{
		position = transform->GetGlobalPosition();
	}

	rotation = transform->GetGlobalRotation();
}

void ComponentRigidBody::SetBodyTranform(const float3& pos, const Quat& rot)
{
	body->setWorldTransform(ToBtTransform(pos, rot));
}

void ComponentRigidBody::AddCollider(ComponentCollider* collider)
{
	if (collider->is_added == false)
	{
		collider->shape->setUserIndex(colliders_index++); 
		compound_shape->addChildShape(ToBtTransform(collider->scaled_center, collider->rotation), collider->shape);
		compound_shape->calculateLocalInertia(mass, inertia);
		body->setMassProps(mass, inertia); // Update Innertia
		collider->is_added = true;
	}
}

void ComponentRigidBody::RemoveCollider(ComponentCollider* collider)
{
	if (collider->is_added == true)
	{
		compound_shape->removeChildShapeByIndex(collider->child_collider_index);
		compound_shape->calculateLocalInertia(mass, inertia);
		body->setMassProps(mass, inertia); // Update Innertia
		collider->is_added = false;
	}
}
