#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Start()
{
	state = EnemyState::IDLE;
	animator = (ComponentAnimator*)game_object->GetComponent(ComponentType::ANIMATOR);
	LOG("Enemy Starting");
}
