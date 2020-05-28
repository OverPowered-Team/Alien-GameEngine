#include "GameManager.h"
#include "RelicBehaviour.h"
#include "PlayerController.h"
#include "PlayerAttacks.h"
#include "Effect.h"
#include "EffectsFunctions.h"
#include "EventManager.h"
#include "Relic_Notification.h"

// Relic
Relic::Relic()
{
}

Relic::~Relic()
{
}

void Relic::OnPickUp(PlayerController* player, std::string attack)
{
	player->PickUpRelic(this);
	if(GameObject::FindWithName("InGame") && GameObject::FindWithName("InGame")->GetComponent<Relic_Notification>())
		GameObject::FindWithName("InGame")->GetComponent<Relic_Notification>()->TriggerRelic(player, this->name, this->description, attack);
}

// AttackRelic
AttackRelic::AttackRelic() : Relic()
{
}

AttackRelic::~AttackRelic()
{
}

void AttackRelic::OnPickUp(PlayerController* _player, std::string attack)
{
	std::vector<std::string> attack_pool = _player->attacks->GetFinalAttacks();

	int random_index = Random::GetRandomIntBetweenTwo(0, attack_pool.size() - 1);
	attack_name = attack_pool[random_index];

	AttackEffect* effect = new AttackEffect();
	effect->SetAttackIdentifier(attack_name);
	effect->on_hit_effect = effect_to_apply;

	switch (relic_effect)
	{
	case Relic_Effect::FIRE:
		effect->OnHit = &ApplyEffectOnHit;
		break;
	case Relic_Effect::ICE:
		effect->OnHit = &ApplyEffectOnHit;
		break;
	case Relic_Effect::EARTH:
		effect->OnHit = &ApplyEffectOnHit;
		effect->AddMultiplicativeModifier(valor, "Attack_Damage");
		break;
	case Relic_Effect::LIGHTNING:
		effect->OnHit = &ApplyEffectOnHit;
		break;
	case Relic_Effect::POISON:
		effect->OnHit = &ApplyEffectOnHit;
		break;
	}

	effects.push_back(effect);

	Relic::OnPickUp(_player, attack_name);
}

// DashRelic
DashRelic::DashRelic() : Relic()
{
}

DashRelic::~DashRelic()
{
}

void DashRelic::OnPickUp(PlayerController* _player, std::string attack)
{
	DashEffect* effect = new DashEffect();
	effect->on_dash_effect = effect_to_apply;
	effect->name = name;

	switch (relic_effect)
	{
	case Relic_Effect::FIRE:
		effect->OnDash = &ApplyEffectOnDash;
		break;
	case Relic_Effect::ICE:
		effect->OnDash = &ApplyEffectOnDash;
		break;
	case Relic_Effect::EARTH:
		effect->OnDash = &ApplyEffectOnDash;
		break;
	case Relic_Effect::POISON:
		effect->OnDash = &ApplyEffectOnDash;
		break;
	}

	effects.push_back(effect);

	Relic::OnPickUp(_player);
}

// RelicBehaviour
RelicBehaviour::RelicBehaviour() : Alien()
{
}

RelicBehaviour::~RelicBehaviour()
{
}

void RelicBehaviour::Start()
{
	std::string json_str;
	
	switch (relic_type)
	{
	case Relic_Type::ATTACK:
		relic = new AttackRelic();
		json_str = "ATTACK";
		break;
	case Relic_Type::DASH:
		relic = new DashRelic();
		json_str = "DASH";
		break;
	default:
		break;
	}

	if (relic)
	{
		audio_emitter = GetComponent<ComponentAudioEmitter>();
		relic->relic_effect = relic_effect;
		SetRelic(json_str.data());
	}

}

void RelicBehaviour::Update()
{
	float3 rot = {0, 2, 0};
	transform->AddRotation(rot);
}

void RelicBehaviour::SetRelic(const char* json_array)
{	
	JSONfilepack* relic_json = JSONfilepack::GetJSON("GameData/Relics.json");

	JSONArraypack* type_array = relic_json->GetArray(json_array);

	if (type_array)
	{
		type_array->GetFirstNode();

		for (uint i = 0; i < type_array->GetArraySize(); i++)
		{
			if (type_array->GetNumber("effect") != (int)relic_effect)
				type_array->GetAnotherNode();
			else
				break;
		}
		relic->name = type_array->GetString("name");
		relic->description = type_array->GetString("description");

		if (std::strcmp(json_array, "ATTACK") == 0)
		{
			EffectData* _effect = new EffectData();
			_effect->name = type_array->GetString("hit_effect.name");
			_effect->vfx_on_apply = type_array->GetString("hit_effect.vfx_on_apply");
			_effect->vfx_position = type_array->GetNumber("hit_effect.vfx_position");
			_effect->time = type_array->GetNumber("hit_effect.time");
			_effect->ticks_time = type_array->GetNumber("hit_effect.ticks_time");

			JSONArraypack* flat_modifiers = type_array->GetArray("hit_effect.flat_modifiers");
			if (flat_modifiers)
			{
				flat_modifiers->GetFirstNode();
				for (uint i = 0; i < flat_modifiers->GetArraySize(); i++)
				{
					Modifier mod;
					mod.identifier = flat_modifiers->GetString("identifier");
					mod.amount = flat_modifiers->GetNumber("value");
					_effect->additive_modifiers.push_back(mod);
					flat_modifiers->GetAnotherNode();
				}
			}

			JSONArraypack* mult_modifiers = type_array->GetArray("hit_effect.multiplicative_modifiers");
			if (mult_modifiers)
			{
				mult_modifiers->GetFirstNode();
				for (uint i = 0; i < mult_modifiers->GetArraySize(); i++)
				{
					Modifier mod;
					mod.identifier = mult_modifiers->GetString("identifier");
					mod.amount = mult_modifiers->GetNumber("value");
					_effect->multiplicative_modifiers.push_back(mod);
					mult_modifiers->GetAnotherNode();
				}
			}

			((AttackRelic*)relic)->effect_to_apply = _effect;
		}
		else if (std::strcmp(json_array, "DASH") == 0)
		{
			EffectData* _effect = new EffectData();
			_effect->name = type_array->GetString("dash_effect.name");
			_effect->time = type_array->GetNumber("dash_effect.time");
			_effect->ticks_time = type_array->GetNumber("dash_effect.ticks_time");

			JSONArraypack* flat_modifiers = type_array->GetArray("dash_effect.flat_modifiers");
			if (flat_modifiers)
			{
				flat_modifiers->GetFirstNode();
				for (uint i = 0; i < flat_modifiers->GetArraySize(); i++)
				{
					Modifier mod;
					mod.identifier = flat_modifiers->GetString("identifier");
					mod.amount = flat_modifiers->GetNumber("value");
					_effect->additive_modifiers.push_back(mod);
					flat_modifiers->GetAnotherNode();
				}
			}

			JSONArraypack* mult_modifiers = type_array->GetArray("dash_effect.multiplicative_modifiers");
			if (mult_modifiers)
			{
				mult_modifiers->GetFirstNode();
				for (uint i = 0; i < mult_modifiers->GetArraySize(); i++)
				{
					Modifier mod;
					mod.identifier = mult_modifiers->GetString("identifier");
					mod.amount = mult_modifiers->GetNumber("value");
					_effect->multiplicative_modifiers.push_back(mod);
					mult_modifiers->GetAnotherNode();
				}
			}

			((DashRelic*)relic)->effect_to_apply = _effect;
		}
	}

	JSONfilepack::FreeJSON(relic_json);
}

void RelicBehaviour::OnTriggerEnter(ComponentCollider* collider)
{
	if (strcmp(collider->game_object_attached->GetTag(), "Player") == 0)
	{
		if (collider->game_object_attached->GetComponent<PlayerController>())
		{
			std::vector<Effect*> effects = collider->game_object_attached->GetComponent<PlayerController>()->effects;

			for (auto it = effects.begin(); it != effects.end(); ++it)
			{
				if (dynamic_cast<DashEffect*>(*it) != nullptr && relic_type == Relic_Type::DASH)
				{
					relic = new AttackRelic();
					SetRelic("ATTACK");
				}
			}

			relic->OnPickUp(collider->game_object_attached->GetComponent<PlayerController>());

			audio_emitter->StartSound("Play_Collect_Runestone");

			Destroy(this->game_object);		
		}
	}
}