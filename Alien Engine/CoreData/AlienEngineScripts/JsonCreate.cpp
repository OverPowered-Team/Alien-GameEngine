#include "JsonCreate.h"

JsonCreate::JsonCreate() : Alien()
{
}

JsonCreate::~JsonCreate()
{
}

void JsonCreate::Start()
{
	JSONfilepack* json = JSONfilepack::GetJSON("myjson.json");

	int num = json->GetNumber("mynum");
	int j = 0;
}

void JsonCreate::Update()
{
}