#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"
#include "Stat.h"
#include "State.h"

class PlayerAttacks;
class Relic;
class Effect;
class Enemy;
class CameraShake;
class InGame_UI;

class ALIEN_ENGINE_API PlayerController : public Alien {
	friend class IdleState;
	friend class RunningState;
	friend class JumpingState;
	friend class AttackingState;
	friend class CastingState;
	friend class RevivingState;
	friend class HitState;

public:
	enum (PlayerType,
		GERALT,
		YENNEFER,

		MAX
		);

	struct PlayerData {
		PlayerType type = PlayerType::GERALT;
		std::map<std::string, Stat> stats;

		//OTHER DATA
		float revive_range = 5.0f;

		//BASIC MOVEMENT DATA
		float3 velocity = float3::zero();
		float gravity = 9.8f;
		float slow_speed = -0.07f;
		float vertical_speed = 0;

		//RECOUNT
		float total_damage_dealt = 0.0f;
		uint total_kills = 0;
	};

	struct DashData
	{
		float accel_multi = 1.f; 
		float max_speed = 4.4f; 
		float min_speed = 2.0f; 
		float start_speed = 0.f; 
		float current_acel_multi = 0.f; 
	};

public:
	PlayerController();
	virtual ~PlayerController();

	void Start();
	void Update();

	void CheckGround();
	void UpdateInput();
	void UpdateVisualEffects(); 
	void SetState(StateType new_state);
	void SwapState(State* new_state);
	void ApplyRoot(float time);
	void ReleaseRoot();

	bool AnyKeyboardInput();

	void HandleMovement();
	void OnDrawGizmos();
	void EffectsUpdate();
	void Jump();
	void Fall();
	void OnAnimationEnd(const char* name);
	void PlayAttackParticle();
	void Die();
	void Revive(float minigame_value);
	void ReceiveDamage(float dmg, float3 knock_speed = { 0,0,0 }, bool knock = true);
	void PlayAllowParticle();

	void ReleaseAttackParticle();

	void HitByRock(float time, float damage);
	void RecoverFromRockHit();

	//Relics
	void PickUpRelic(Relic* _relic);
	void AddEffect(Effect* _effect);

	bool CheckBoundaries();
	void OnDrawGizmosSelected();
	float3 GetDirectionVector();
	bool CheckForPossibleRevive();

	void OnUltimateActivation(float value);
	void OnUltimateDeactivation(float value);
	void OnHit(Enemy* enemy, float dmg_dealt);
	void UpdateDashEffect();
	void OnEnemyKill();
	void OnTriggerEnter(ComponentCollider* col);

	void StartImmune() { is_immune = true; };
	void StopImmune() { is_immune = false; };

	void HitFreeze(float freeze_time);
	void RemoveFreeze(float speed);
	void PauseParticle();
	void ResumeParticle();
	void SpawnParticle(std::string particle_name, float3 pos = float3::zero(), bool local = true, float3 rotation = float3::zero(), GameObject* parent = nullptr);

	void ReleaseParticle(std::string particle_name);

	//Battle Circles
	void CheckEnemyCircle();
	// Terrain - particles
	void OnTerrainEnter(float4 initial_color, float4 final_color); 

	// Dash wonders
	void ToggleDashMultiplier(); 

private:
	void LoadStats();
	void InitKeyboardControls();
	void CalculateAABB();
	void AbsorbHit();
	std::vector<Effect*>::iterator RemoveEffect(std::vector<Effect*>::iterator it); //this feels dirty :(

public:
	State* state;

	PlayerAttacks* attacks = nullptr;
	PlayerData player_data;
	std::vector<GameObject*> particles;
	std::vector<GameObject*> particle_spawn_positions;
	ComponentAnimator* animator = nullptr;
	ComponentCharacterController* controller = nullptr;
	ComponentAudioEmitter* audio = nullptr;

	float2 movement_input;
	float3 direction;

	bool mov_input = false;
	bool is_immune = false;
	bool is_grounded = false;
	bool can_move = true;
	bool input_blocked = false;

	//Relics
	std::vector<Effect*> effects;
	std::vector<Relic*> relics;

	//DashCollider
	Prefab dash_collider;
	float3 last_dash_position = float3::zero();

	//UI 
	GameObject* HUD = nullptr;

	//Others
	float delay_footsteps = 0.2f;
	PlayerController* player_being_revived = nullptr;
	bool godmode = false;

	//Revive
	Prefab revive_world_ui;

	//Input Variables
	int controller_index = 1;

	//Keyboard input
	SDL_Scancode keyboard_move_up;
	SDL_Scancode keyboard_move_left;
	SDL_Scancode keyboard_move_right;
	SDL_Scancode keyboard_move_down;
	SDL_Scancode keyboard_jump;
	SDL_Scancode keyboard_dash;
	SDL_Scancode keyboard_light_attack;
	SDL_Scancode keyboard_heavy_attack;
	SDL_Scancode keyboard_spell_1;
	SDL_Scancode keyboard_spell_2;
	SDL_Scancode keyboard_spell_3;
	SDL_Scancode keyboard_spell_4;
	SDL_Scancode keyboard_revive;
	SDL_Scancode keyboard_ultimate;

	//Joystick input
	Input::CONTROLLER_BUTTONS controller_jump = Input::CONTROLLER_BUTTON_A;
	Input::CONTROLLER_BUTTONS controller_dash = Input::CONTROLLER_BUTTON_RIGHTSHOULDER;
	Input::CONTROLLER_BUTTONS controller_light_attack = Input::CONTROLLER_BUTTON_X;
	Input::CONTROLLER_BUTTONS controller_heavy_attack = Input::CONTROLLER_BUTTON_Y;
	Input::CONTROLLER_BUTTONS controller_spell = Input::CONTROLLER_BUTTON_DPAD_UP;
	Input::CONTROLLER_BUTTONS controller_ultimate = Input::CONTROLLER_BUTTON_LEFTSHOULDER;
	Input::CONTROLLER_BUTTONS controller_revive = Input::CONTROLLER_BUTTON_B;

	//Battle Circle
	std::vector<Enemy*> enemy_battle_circle;
	int current_attacking_enemies = 0;
	int max_attacking_enemies = 2;
	float battleCircle = 2.0f;

	AABB max_aabb;

	// Dash data
	DashData dashData; 

private:
	float angle = 0.0f;
	float timer = 0.f;

	ComponentCamera* camera = nullptr;

	CameraShake* shake = nullptr;
	float last_regen_tick = 0.0f;
};

ALIEN_FACTORY PlayerController* CreatePlayerController() {
	PlayerController* player = new PlayerController();

	// To show in inspector here
	SHOW_IN_INSPECTOR_AS_SLIDER_INT(player->controller_index, 1, 2);
	SHOW_IN_INSPECTOR_AS_ENUM(PlayerController::PlayerType, player->player_data.type);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.gravity);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.slow_speed);
	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->player_data.revive_range);

	SHOW_VOID_FUNCTION(PlayerController::PlayAttackParticle, player);
	SHOW_VOID_FUNCTION(PlayerController::PlayAllowParticle, player);
	SHOW_VOID_FUNCTION(PlayerController::StartImmune, player);
	SHOW_VOID_FUNCTION(PlayerController::StopImmune, player);
	SHOW_VOID_FUNCTION(PlayerController::ToggleDashMultiplier, player);

	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->delay_footsteps, 0.01f, 1.f);
	SHOW_IN_INSPECTOR_AS_PREFAB(player->dash_collider);
	SHOW_IN_INSPECTOR_AS_PREFAB(player->revive_world_ui);

	SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(player->battleCircle);
	SHOW_TEXT("Dash animation cool data"); 
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->dashData.max_speed, 3.f, 5.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->dashData.min_speed, 2.f, 3.f);
	SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(player->dashData.accel_multi, 1.f, 2.f);

	return player;
}