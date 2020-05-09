#include "col_test.h"

col_test::col_test() : Alien()
{
}

col_test::~col_test()
{
}

void col_test::Start()
{
}

void col_test::Update()
{
}

void col_test::OnCollisionEnter(const Collision& collision)
{
	LOG_ENGINE("collision enter with %s", collision.game_object->GetName());
}