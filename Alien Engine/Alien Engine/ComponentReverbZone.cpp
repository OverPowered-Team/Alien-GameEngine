#include "Application.h"
#include "ComponentReverbZone.h"
#include "ComponentTransform.h"
#include "ModuleAudio.h"

ReverbZone::ReverbZone(GameObject * parent) : Component(parent)
{
	type = ComponentType::A_REVERB;
}

ReverbZone::~ReverbZone()
{

}

void ReverbZone::Update(float dt)
{
	/*sphere.pos = ((ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM))->GetGlobalPosition();
	
	AABB box = sphere.MinimalEnclosingAABB();
	OBB boundingBox(box);
	//box.Transform(((ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM))->matr);

	DebugDrawSphere();
	for (int i = 0; i < App->scene_intro->root->children.size(); ++i) {
		AudioEmitter* audio_emitter = App->scene_intro->root->children[i]->GetComponentAudioEmitter();

		if (audio_emitter != nullptr && App->scene_intro->root->children.size() >= 0)
		{
			for (int j = 0; j < App->scene_intro->root->children[i]->children.size(); ++j) {
				if (sphere.Intersects(App->scene_intro->root->children[i]->children[j]->bounding_box) == true) {
					audio_emitter->source->ApplyEnvReverb(1.0f, "Rain");
				}
				else {
					audio_emitter->source->ApplyEnvReverb(0.0f, "Rain");
				}
			}
		}
	}*/

}

void ReverbZone::SetRadius(float rad) 
{
	radius = rad;
	sphere.r = rad;
}

void ReverbZone::DebugDrawSphere()
{
	glLineWidth(3.0f);
	glColor3f(2.0f, 2.0f, 2.0f);

	float radius = sphere.r;
	float3 pos = sphere.pos;
	float degInRad = 360.0f / 12;
	degInRad = DegToRad(degInRad);
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(cos(degInRad * i) * radius + pos.x, pos.y, sin(degInRad * i) * radius + pos.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(cos(degInRad * i) * radius + pos.x, sin(degInRad * i) * radius + pos.y, pos.z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < 12; i++)
		glVertex3f(pos.x, sin(degInRad * i) * radius + pos.y, cos(degInRad * i) * radius + pos.z);
	glEnd();

	glLineWidth(1.0f);
}

void ReverbZone::SaveComponent(JSONArraypack* to_save)
{
	to_save->SetNumber("Type", (int)type);
	to_save->SetNumber("Radius", radius);
}

void ReverbZone::LoadComponent(JSONArraypack* to_load)
{
	radius = to_load->GetNumber("Radius");
}
