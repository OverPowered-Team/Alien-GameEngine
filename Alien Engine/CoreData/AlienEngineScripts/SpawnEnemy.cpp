#include "GameManager.h"
#include "BlockerObstacle.h"
#include "SpawnEnemy.h"
#include "EnemyManager.h"
#include "Enemy.h"

SpawnEnemy::SpawnEnemy() : Alien()
{
}

SpawnEnemy::~SpawnEnemy()
{
}

void SpawnEnemy::InstantiateEnemy(GameObject* parent)
{
	Enemy* enemy = GameManager::instance->enemy_manager->CreateEnemy(enemy_type, game_object->transform->GetGlobalPosition(), (ExtraEnumType)extra_type, parent);
	enemy->character_ctrl->SetPosition(game_object->transform->GetGlobalPosition());
	parent->GetComponent<BlockerObstacle>()->AddChildren(enemy->game_object);
	Destroy(game_object);
}

void SpawnEnemy::InstantiateEnemy()
{
	Enemy* enemy = GameManager::instance->enemy_manager->CreateEnemy(enemy_type, game_object->transform->GetGlobalPosition(), (ExtraEnumType)extra_type);
	enemy->character_ctrl->SetPosition(game_object->transform->GetGlobalPosition());
	Destroy(game_object);
}