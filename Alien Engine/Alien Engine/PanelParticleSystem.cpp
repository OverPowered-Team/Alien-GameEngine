#include "PanelParticleSystem.h"
#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ModuleCamera3D.h"
#include "Viewport.h"

PanelParticleSystem::PanelParticleSystem(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
}

PanelParticleSystem::~PanelParticleSystem()
{
}

void PanelParticleSystem::PanelLogic() 
{
	if (App->objects->GetSelectedObjects().empty())
		return;

	GameObject* selected = App->objects->GetSelectedObjects().back();

	if (selected == nullptr) 
		return;

	ComponentParticleSystem* compParticleSystem = (ComponentParticleSystem*)selected->GetComponent(ComponentType::PARTICLES);

	if (compParticleSystem == nullptr)
		return;

	particleSystem = compParticleSystem->GetSystem();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse  | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

	

	current_viewport_pos = App->camera->scene_viewport->GetPos();
	current_viewport_size = App->camera->scene_viewport->GetSize();

	ImGui::SetNextWindowPos(ImVec2(current_viewport_pos.x + current_viewport_size.x/1.20f, current_viewport_pos.y + current_viewport_size.y/1.20f));


	if (ImGui::Begin("Particle System", nullptr, flags))
	{
		/*ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.7f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.7f });*/
		static bool start = true;

		if (!start) {
			if (ImGui::Button("Play")) { start = true, particleSystem->Play(); }
		}
		else
			if (ImGui::Button("Pause")) { start = false, particleSystem->Pause(); }
		
		
		ImGui::SameLine();
		if (ImGui::Button("Restart")) { start = true, particleSystem->Restart(); } ImGui::SameLine();
		if (ImGui::Button("Stop")) { start = false, particleSystem->Stop(); }

		ImGui::Text("Playback Time: ");	ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.3f", particleSystem->emmitter.GetCurrentLife());
		ImGui::Text("Particles: ");	ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", particleSystem->GetTotalParticles());

	}
	ImGui::End();

}

ParticleSystem* PanelParticleSystem::GetSystem() const
{
	return particleSystem;
}

void PanelParticleSystem::SetSystem(ParticleSystem* system)
{
	particleSystem = system;
}
