#include "PolScriptSushi.h"

PolScriptSushi::PolScriptSushi() : Alien()
{
}

PolScriptSushi::~PolScriptSushi()
{
}

void PolScriptSushi::Start()
{
	LOG("%s", myString.GetString());
}

void PolScriptSushi::Update()
{
}
