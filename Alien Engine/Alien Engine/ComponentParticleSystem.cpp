#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ComponentTransform.h"
#include "Time.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ReturnZ.h"
#include "imgui/imgui_internal.h"
#include "PanelProject.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* parent) : Component(parent)
{
	type = ComponentType::PARTICLES;
	particleSystem = new ParticleSystem();
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	if (particleSystem != nullptr)
	{
		delete particleSystem; 
		particleSystem = nullptr; 
	}
}

void ComponentParticleSystem::OnPlay()
{
	particleSystem->Restart();
}

void ComponentParticleSystem::PreUpdate()
{
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);

	particleSystem->emmitter.SetPosition(transform->GetGlobalPosition());
	particleSystem->emmitter.SetRotation(transform->GetGlobalRotation());

	if(particleSystem->isPlaying())
		particleSystem->PreUpdate(Time::GetCurrentDT());
}

void ComponentParticleSystem::Update()
{
	if (particleSystem->isPlaying())
		particleSystem->Update(Time::GetCurrentDT());
}

void ComponentParticleSystem::PostUpdate()
{
	if (particleSystem->isPlaying())
		particleSystem->PostUpdate(Time::GetCurrentDT());
}

void ComponentParticleSystem::DebugDraw()
{
	if (drawEmmitter && App->objects->GetSelectedObjects().back() == game_object_attached)
		particleSystem->DrawEmmitter();
}

void ComponentParticleSystem::Draw()
{
	particleSystem->DrawParticles();
}

void ComponentParticleSystem::OnEnable()
{
	particleSystem->Restart();
}

void ComponentParticleSystem::OnDisable()
{
	particleSystem->Pause(); 
}

bool ComponentParticleSystem::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	

	if (ImGui::CollapsingHeader("Particle System", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{

		static bool cntl_Z = true; // CTRLZ Checking

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Emmitter ----------------------------------------
		


		if (ImGui::TreeNodeEx("Emmitter", ImGuiTreeNodeFlags_Bullet  | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ParticleEmmitter* emmitter = &particleSystem->emmitter;

			ImGui::Spacing();
			ImGui::Checkbox("Draw Emmitter", &drawEmmitter);

			if (ImGui::TreeNode("Shape Options"))
			{
				shapeSelected = (int)particleSystem->emmitter.GetShape();
				if (ImGui::Combo("Shape", &shapeSelected, "Sphere\0Hemisphere\0Cube\0Cone\0\0")) { emmitter->SetShape((Emmitter_Shape)shapeSelected); }

				switch (emmitter->GetShape())
				{
				case Emmitter_Shape::Sphere:
				{
					float radius = emmitter->GetRadius();
					if (ImGui::DragFloat("Radius", &radius, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetRadius(radius); }

					int zoneSelected = (int)emmitter->GetZone();
					if (ImGui::Combo("Emmission Zone", &zoneSelected, "Center\0Volume\0\0")) { emmitter->SetZone((Emmitter_Zone)zoneSelected); }
				}
				break;

				case Emmitter_Shape::Cone:
				{
					// Not inner radius yet

					float outRadius = emmitter->GetOutRadius();
					if (ImGui::DragFloat("Outter Radius", &outRadius, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetOutRadius(outRadius); }

					float distance = emmitter->GetDistance();
					if (ImGui::DragFloat("Distance", &distance, 0.2f, 0.0f, FLT_MAX)) { emmitter->SetDistance(distance); }
				}
				break;

				case Emmitter_Shape::Cube:
					ImGui::DragFloat3("Cube Size", emmitter->GetCubeSize(), 1.0f, 0.0f, FLOAT_INF);
					break;

				}

				ImGui::TreePop();
			}


			float duration = emmitter->GetMaxLife();
			if (ImGui::DragFloat("Duration", &duration, 1.0F, 0.0F, FLT_MAX)) 
			{ 
				if (cntl_Z)
				 ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this); 

				cntl_Z = false;
				emmitter->SetMaxLife(duration);
			}
			else if (!cntl_Z && ImGui::IsMouseReleased(0)) {
				cntl_Z = true;
			}

			bool loop = emmitter->GetLoop();
			if (ImGui::Checkbox("Looping", &loop)) { emmitter->SetLoop(loop); }

			float spawnRate = emmitter->GetSpawnRate();
			if (ImGui::DragFloat("Spawn Rate", &spawnRate, 0.2f, 0.00f, FLT_MAX)) { emmitter->SetSpawnRate(spawnRate); }

			float3 pos = particleSystem->emmitter.GetRelativePosition();
			if (ImGui::DragFloat3("Position", (float*)&pos)) { emmitter->SetRelativePosition(pos); }

			float3 rot = particleSystem->emmitter.GetRelativeRotation();
			if (ImGui::DragFloat3("Rotation", (float*)&rot)) { emmitter->SetRelativeRotation(rot); }

			if (ImGui::TreeNodeEx("Bursts"))
			{
				for (int i = 0; i < emmitter->bursts.size(); ++i)
				{
					ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
					if (ImGui::TreeNodeEx((void*)(intptr_t)i, base_flags, "Burst %i", i))
					{
						ImGui::DragFloat("Time", &emmitter->bursts[i].timeToBurst, 0.1f, 0, emmitter->GetMaxLife());
						ImGui::DragInt("Particles", &emmitter->bursts[i].partsToInstantiate, 1.0f, 0, MAX_PARTICLES_TO_BURST);
						if (ImGui::Button("Remove")) emmitter->RemoveBurst(i);
						ImGui::TreePop();

					}
				}

				if (ImGui::Button("Add Burst")) { emmitter->bursts.push_back(Burst()); }

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Particle ----------------------------------------

		if (ImGui::TreeNodeEx("Particle", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::DragFloat("Life Time", &particleSystem->particleInfo.maxLifeTime, 1.0f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Speed", (float*)&particleSystem->particleInfo.speed, 0.2f);

			if (ImGui::RadioButton("Global", &transformSelected, 0)) { particleSystem->particleInfo.globalTransform = true; }
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", &transformSelected, 1)) { particleSystem->particleInfo.globalTransform = false; }

			// Initial State || Final State
			if (ImGui::TreeNodeEx("Start State", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::ColorPicker4("Color", (float*)&particleSystem->particleInfo.color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);
				ImGui::DragFloat("Size", (float*)&particleSystem->particleInfo.size, 0.1f, 0.0f, FLT_MAX);
				ImGui::DragFloat3("Gravity", (float*)&particleSystem->particleInfo.force);
				ImGui::Checkbox("Change Over Time", &particleSystem->particleInfo.changeOverLifeTime);

				ImGui::TreePop();
			}

			if (particleSystem->particleInfo.changeOverLifeTime)
			{
				if (ImGui::TreeNodeEx("Final State", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::ColorPicker4("Color", (float*)&particleSystem->endInfo.color,
						ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview);
					ImGui::DragFloat("Size", (float*)&particleSystem->endInfo.size, 0.1f, 0.0f, FLT_MAX);
					ImGui::DragFloat3("Gravity", (float*)&particleSystem->endInfo.force);
					ImVec2 size = ImGui::GetItemRectSize();
					if (ImGui::Button("Equalize Values", size))
						particleSystem->endInfo = particleSystem->particleInfo;
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ----------------------------------- Renderer ----------------------------------------

		


		if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			
		
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Orientation Mode ");
			ImGui::SameLine(200, 15);
			if (ImGui::Combo("Billboard", &bbTypeSelected, "Screen Aligned\0World Aligned\0Axially Aligned\0None\0\0"))
			{
				particleSystem->SetBillboardType((BillboardType)bbTypeSelected);
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Particle Texture ");

			//static ResourceTexture* selected_texture = nullptr;

			if (texture != nullptr)
			{
				ImGui::Spacing();
				ImGui::Spacing();

				static bool check;
				check = texture_activated;
				if (ImGui::Checkbox("Texture Active", &check)) {
					//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
					texture_activated = check;
				}

				ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->width);
				ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->height);
				ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", texture->GetAssetsPath());
				ImGui::Text("References:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", texture->references);
				ImGui::Image((ImTextureID)texture->id, {140 ,140 });

				ImGui::Spacing();

				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.65F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, { 0.8F,0,0,1 });
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, { 0.95F,0,0,1 });
				if (ImGui::Button("Delete", { 60,20 })) {
					//ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
					texture->DecreaseReferences();
					texture = nullptr;
					particleSystem->texture = texture;
					selected_texture = nullptr;	

					
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SameLine(80, 15);
				if (ImGui::Button("Change Texture", { 120,20 })) {
					change_texture_menu = true;
					selected_texture = texture;
				}
				ImGui::Spacing();
			}

			else {
				

				ImGui::SameLine(200, 15);
				if (ImGui::Button("Add Texture", { 120,20 })) {
					change_texture_menu = true;
					selected_texture = texture;
				}
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture assigned");
				ImGui::Spacing();
			}


			if (change_texture_menu) 
			{
				TextureBrowser();
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			
			

			static bool enable_blend = false;

			ImGui::Checkbox("##pptActiveBlend", &enable_blend);
			ImGui::SameLine();
			
			if (ImGui::TreeNodeEx("Blending Options", ImGuiTreeNodeFlags_Framed))
			{
				
				ImGui::Spacing();

				if (!enable_blend)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "Transparency:");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Alpha test: "); ImGui::SameLine(200, 15);
				ImGui::SliderFloat("##alpha test", (float*)&particleSystem->alpha_test, 0.1f, 1.0f);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "Blending Color:");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Source ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Source", &funcTypeSource, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendSource = (FunctionBlendType)funcTypeSource;
				ImGui::Text("Destination ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Destination", &funcTypeDest, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendDest = (FunctionBlendType)funcTypeDest;

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Blend Equation: ");
				ImGui::SameLine(200, 15);
				if (ImGui::Combo("##Equation Mode", &eqTypeSelected, "ADD\0SUBTRACT\0REVERSE SUBTRACT\0\0"))
					particleSystem->eqBlend = (EquationBlendType)eqTypeSelected;

				if (!enable_blend)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::TreePop();
			}
			

			ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::TreeNodeEx("Serialization", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Save Particle System ");
			ImGui::SameLine(200, 15);
			if (ImGui::Button("Save", { 120,20 })) {
				SaveParticles();
			}
				
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Load Particle System ");
			ImGui::SameLine(200, 15);
			if (ImGui::Button("Load", { 120,20 })) {
				LoadParticles();
			}

				

		
			if (ImGui::Button("Restart Particle System")) particleSystem->ResetSystem();


		ImGui::TreePop();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	return true;
}

void ComponentParticleSystem::TextureBrowser()
{
	ImGui::OpenPopup("Textures Loaded");
	ImGui::SetNextWindowSize({ 522,585 });

	if (ImGui::BeginPopupModal("Textures Loaded", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
		ImGui::Spacing();
		ImGui::NewLine();
		ImGui::SameLine(190);
		ImGui::Text("Texture Selected");
		ImGui::Text("");
		ImGui::SameLine(170);

		if (selected_texture != nullptr) {
			ImGui::Image((ImTextureID)selected_texture->id, { 150,150 });
			ImGui::Spacing();
			ImGui::Text("");
			ImGui::SameLine(150);

			ImGui::Text("Texture Size:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->width);
			ImGui::SameLine(); ImGui::Text("x"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->height);
			ImGui::Text("");
			ImGui::SameLine(150);
			ImGui::Text("References:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%i", selected_texture->references);
			ImGui::Text("");
			ImGui::SameLine(112);
			ImGui::Text("Path:"); ImGui::SameLine(); ImGui::TextColored({ 255, 216, 0, 100 }, "%s", selected_texture->GetAssetsPath());
		}
		ImGui::Spacing();

		if (ImGui::BeginChild("##TexturesSelectorChild", { 492,285 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
			ImGui::Columns(3, 0, false);
			ImGui::SetColumnWidth(0, 156);
			ImGui::SetColumnWidth(1, 156);
			ImGui::SetColumnWidth(2, 156);

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {

					if ((*item)->NeedToLoad())
						(*item)->IncreaseReferences();

					ImGui::ImageButton((ImTextureID)static_cast<ResourceTexture*>(*item)->id, { 140,140 });
					if (ImGui::IsItemClicked()) {
						selected_texture = static_cast<ResourceTexture*>(*item);
					}
					ImGui::NextColumn();
				}
			}

			ImGui::EndChild();
		}
		ImGui::Spacing();
		ImGui::Text("");
		ImGui::SameLine(377);
		if (ImGui::Button("Apply", { 120,20 })) {
			ReturnZ::AddNewAction(ReturnZ::ReturnActions::CHANGE_COMPONENT, this);
			texture = selected_texture;
			particleSystem->texture = texture;
			selected_texture = nullptr;
			change_texture_menu = false;

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {

					if (*item != texture)
						(*item)->DecreaseReferences();
				}
			}
		}
		ImGui::SameLine(237);
		if (ImGui::Button("Cancel", { 120,20 })) {
			selected_texture = nullptr;
			change_texture_menu = false;

			std::vector<Resource*>::iterator item = App->resources->resources.begin();
			for (; item != App->resources->resources.end(); ++item) {
				if (*item != nullptr && (*item)->GetType() == ResourceType::RESOURCE_TEXTURE && static_cast<ResourceTexture*>(*item)->is_custom) {
						(*item)->DecreaseReferences();
				}
			}
		}
		ImGui::EndPopup();
	}
}

void ComponentParticleSystem::SetTexture(ResourceTexture* tex)
{
	texture = tex;
	particleSystem->texture = texture;

}

void ComponentParticleSystem::Play()
{
	particleSystem->Play();
}

void ComponentParticleSystem::Pause()
{
	particleSystem->Pause();
}

void ComponentParticleSystem::Restart()
{
	particleSystem->Restart();
}

ParticleSystem* ComponentParticleSystem::GetSystem()
{
	return particleSystem;
}

void ComponentParticleSystem::SaveComponent(JSONArraypack* to_save)
{
	// --------------- General Info -------------------- //
	to_save->SetNumber("Type", (int)type);
	to_save->SetString("ID", std::to_string(ID));

	// ----------------------- Billboard Info ----------------------- //

	to_save->SetNumber("Billboard", (int)particleSystem->bbType);

	// --------------- Particle System Start Info -------------------- //

	// Position
	to_save->SetFloat3("Start.Position", particleSystem->particleInfo.position);
	// Rotation
	to_save->SetQuat("Start.Rotation", particleSystem->particleInfo.rotation); 
	// Global / Local
	to_save->SetBoolean("Start.Global", particleSystem->particleInfo.globalTransform);
	// Velocity
	to_save->SetFloat3("Start.Velocity", particleSystem->particleInfo.velocity);
	// Force
	to_save->SetFloat3("Start.Force", particleSystem->particleInfo.force);
	// Speed
	to_save->SetNumber("Start.Speed", (float)particleSystem->particleInfo.speed);
	// Color
	to_save->SetFloat4("Start.Color", particleSystem->particleInfo.color);
	// Size
	to_save->SetNumber("Start.Size", (float)particleSystem->particleInfo.size);
	// LightColor
	to_save->SetFloat4("Start.LightColor", particleSystem->particleInfo.lightColor);
	// MaxLifeTime
	to_save->SetNumber("Start.MaxLifeTime", (float)particleSystem->particleInfo.maxLifeTime);
	// changeOverLifeTime
	to_save->SetBoolean("Start.ChangeOverLifeTime", particleSystem->particleInfo.changeOverLifeTime);
	
	// ----------------- Particle System End Info -------------------- //

	// Color
	to_save->SetFloat4("End.Color", particleSystem->endInfo.color);
	// Size
	to_save->SetNumber("End.Size", (float)particleSystem->endInfo.size);
	// LightColor
	to_save->SetFloat4("End.LightColor", particleSystem->endInfo.lightColor);
	// Force
	to_save->SetFloat3("End.Force", particleSystem->endInfo.force);

	// ---------------------- Emitter Info --------------------------- //

	// Shape
	to_save->SetNumber("Emmitter.Shape", (int)particleSystem->emmitter.GetShape());
	// Radius
	to_save->SetNumber("Emmitter.Radius", particleSystem->emmitter.GetRadius());
	// OutterRadius
	to_save->SetNumber("Emmitter.OutRadius", particleSystem->emmitter.GetOutRadius());
	// MaxLife
	to_save->SetNumber("Emmitter.MaxLife", particleSystem->emmitter.GetMaxLife());
	// CurrentLife
	to_save->SetNumber("Emmitter.CurrentLife", particleSystem->emmitter.GetCurrentLife());
	// SpawnRate
	to_save->SetNumber("Emmitter.SpawnRate", particleSystem->emmitter.GetSpawnRate());
	// Delay
	to_save->SetNumber("Emmitter.Delay", particleSystem->emmitter.GetDelay());
	// Loop
	to_save->SetBoolean("Emmitter.Loop", particleSystem->emmitter.GetLoop());
	// Position
	to_save->SetFloat3("Emmitter.Position", particleSystem->emmitter.GetPosition());
	// RelativePosition
	to_save->SetFloat3("Emmitter.RelativePosition", particleSystem->emmitter.GetRelativePosition());
	// Rotation
	to_save->SetFloat3("Emmitter.Rotation", particleSystem->emmitter.GetRotation());
	// Relative Rotation
	to_save->SetFloat3("Emmitter.RelativeRotation", particleSystem->emmitter.GetRelativeRotation());
	// Scale
	to_save->SetFloat3("Emmitter.Scale", particleSystem->emmitter.GetScale());

	// ------------------------ Burst Info --------------------------- //

	// Burst List size
	to_save->SetNumber("Bursts.Size", particleSystem->emmitter.bursts.size());

	std::string name;
	for (int i = 0; i < particleSystem->emmitter.bursts.size(); ++i)
	{
		// TimeBurst
		name = "Burst.TimeBurst-";
		name += i + '0';
		to_save->SetNumber(name.c_str(), particleSystem->emmitter.bursts[i].timeToBurst);

		// PartsInstanciate
		name = "Burst.PartsInstanciate-";
		name += i + '0';
		to_save->SetNumber(name.c_str(), particleSystem->emmitter.bursts[i].partsToInstantiate);
	}

	// ---------------------- Blending Info -------------------------- //

	// Source
	to_save->SetNumber("Blending.Source", (int)particleSystem->funcBlendSource);
	// Destination
	to_save->SetNumber("Blending.Destination", (int)particleSystem->funcBlendDest);
	// Equation
	to_save->SetNumber("Blending.Equation", (int)particleSystem->eqBlend);

	// --------------- Material Resource Info -------------------- //

	
	to_save->SetBoolean("TextureEnabled", texture_activated);
	to_save->SetString("ID", std::to_string(ID));
	to_save->SetBoolean("HasTexture", (texture != nullptr) ? true : false);
	
	if (texture != nullptr) {
		to_save->SetString("TextureID", std::to_string(texture->GetID()));
	}
	
}

void ComponentParticleSystem::LoadComponent(JSONArraypack* to_load)
{
	// --------------- General Info -------------------- //
	//to_load->GetNumber("Type");
	ID = std::stoull(to_load->GetString("ID"));

	// ----------------------- Billboard Info ----------------------- //

	particleSystem->bbType = (BillboardType)(int)to_load->GetNumber("Billboard");

	// --------------- Particle System Start Info -------------------- //

	// Position
	particleSystem->particleInfo.position = to_load->GetFloat3("Start.Position");
	// Rotation
	particleSystem->particleInfo.rotation = to_load->GetQuat("Start.Rotation");
	// Global / Local
	particleSystem->particleInfo.globalTransform = to_load->GetBoolean("Start.Global");
	transformSelected = particleSystem->particleInfo.globalTransform ? 0 : 1;
	// Velocity
	particleSystem->particleInfo.velocity = to_load->GetFloat3("Start.Velocity");
	// Force
	particleSystem->particleInfo.force = to_load->GetFloat3("Start.Force");
	// Speed
	particleSystem->particleInfo.speed = to_load->GetNumber("Start.Speed");
	// Color
	particleSystem->particleInfo.color = to_load->GetFloat4("Start.Color");
	// Size
	particleSystem->particleInfo.size = to_load->GetNumber("Start.Size");
	// LightColor
	particleSystem->particleInfo.lightColor = to_load->GetFloat4("Start.LightColor");
	// MaxLifeTime
	particleSystem->particleInfo.maxLifeTime = to_load->GetNumber("Start.MaxLifeTime");
	// changeOverLifeTime
	particleSystem->particleInfo.changeOverLifeTime = to_load->GetBoolean("Start.ChangeOverLifeTime");

	// ----------------- Particle System End Info -------------------- //

	// Color
	particleSystem->endInfo.color = to_load->GetFloat4("End.Color");
	// Size
	particleSystem->endInfo.size = to_load->GetNumber("End.Size");
	// LightColor
	particleSystem->endInfo.lightColor = to_load->GetFloat4("End.LightColor");
	// Force
	particleSystem->endInfo.force = to_load->GetFloat3("End.Force");

	// ---------------------- Emitter Info --------------------------- //

	particleSystem->ResetSystem();

	// Shape
	particleSystem->emmitter.SetShape((Emmitter_Shape)(int)to_load->GetNumber("Emmitter.Shape"));
	// Radius
	particleSystem->emmitter.SetRadius(to_load->GetNumber("Emmitter.Radius"));
	// OutterRadius
	particleSystem->emmitter.SetOutRadius(to_load->GetNumber("Emmitter.OutRadius"));
	// MaxLife
	particleSystem->emmitter.SetMaxLife(to_load->GetNumber("Emmitter.MaxLife"));
	//// CurrentLife
	//particleSystem->emmitter.SetCurrentLife(to_load->GetNumber("Emmitter.CurrentLife"));
	// SpawnRate
	particleSystem->emmitter.SetSpawnRate(to_load->GetNumber("Emmitter.SpawnRate"));
	// Delay
	particleSystem->emmitter.SetDelay(to_load->GetNumber("Emmitter.Delay"));
	// Loop
	particleSystem->emmitter.SetLoop(to_load->GetBoolean("Emmitter.Loop"));
	// Position
	particleSystem->emmitter.SetPosition(to_load->GetFloat3("Emmitter.Position"));
	// RelativePosition
	particleSystem->emmitter.SetRelativePosition(to_load->GetFloat3("Emmitter.RelativePosition"));
	// Rotation
	particleSystem->emmitter.SetRotation(to_load->GetFloat3("Emmitter.Rotation"));
	// Relative Rotation
	particleSystem->emmitter.SetRelativeRotation(to_load->GetFloat3("Emmitter.RelativeRotation"));
	// Scale
	particleSystem->emmitter.SetScale(to_load->GetFloat3("Emmitter.Scale"));

	// ------------------------ Burst Info --------------------------- //

	// Clean Burst List 
	particleSystem->emmitter.bursts.clear();

	// Burst List size
	int burstListSize = to_load->GetNumber("Bursts.Size");

	std::string name;
	for (int i = 0; i < burstListSize; ++i)
	{
		particleSystem->emmitter.bursts.push_back(Burst());

		// TimeBurst
		name = "Burst.TimeBurst-";
		name += i + '0';
		particleSystem->emmitter.bursts[i].timeToBurst = to_load->GetNumber(name.c_str());

		// PartsInstanciate
		name = "Burst.PartsInstanciate-";
		name += i + '0';
		particleSystem->emmitter.bursts[i].partsToInstantiate = to_load->GetNumber(name.c_str());
	}

	// ---------------------- Blending Info -------------------------- //

	// Source
	particleSystem->funcBlendSource = (FunctionBlendType)(int)to_load->GetNumber("Blending.Source");
	// Destination
	particleSystem->funcBlendDest = (FunctionBlendType)(int)to_load->GetNumber("Blending.Destination");
	// Equation
	particleSystem->eqBlend = (EquationBlendType)(int)to_load->GetNumber("Blending.Equation");


	// ---------------------- Resource Info -------------------------- //

	
	texture_activated = to_load->GetBoolean("TextureEnabled");

	if (to_load->GetBoolean("HasTexture")) {
		u64 ID = std::stoull(to_load->GetString("TextureID"));
		texture = (ResourceTexture*)App->resources->GetResourceWithID(ID);
		if (texture != nullptr) 
		{
			texture->IncreaseReferences();
			particleSystem->texture = texture;
		}
	}
	ID = std::stoull(to_load->GetString("ID"));
}



void ComponentParticleSystem::LoadParticles()
{
	OPENFILENAME to_load;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);

	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Particles")).data();

	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_load, sizeof(to_load));

	to_load.lStructSize = sizeof(to_load);
	to_load.hwndOwner = NULL;
	to_load.lpstrFilter = "alienParticles\0*.alienParticles";
	to_load.lpstrFile = filename;
	to_load.nMaxFile = MAX_PATH;
	to_load.lpstrTitle = "Load a .alienParticles";
	to_load.lpstrInitialDir = dir.data();
	to_load.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (GetOpenFileNameA(&to_load))
	{
		SetCurrentDirectoryA(curr_dir);
		std::string name = filename;
		App->file_system->NormalizePath(name);


		JSON_Value* value = json_parse_file(name.data());
		JSON_Object* object = json_value_get_object(value);


		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* particles = new JSONfilepack(name.data(), object, value);

			JSONArraypack* properties = particles->GetArray("ParticleSystem.Properties");

			if (properties != nullptr)
				LoadComponent(properties);

			delete particles;
		}
		else {
			LOG_ENGINE("Error loading particle system %s", name.data());
		}

		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else {
		SetCurrentDirectoryA(curr_dir);
	}
}


void ComponentParticleSystem::SaveParticles()
{
	OPENFILENAME to_save;

	static char filename[MAX_PATH];

	// get the current game directory
	static char curr_dir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curr_dir);

	// add the local path of the scenes folder
	std::string dir = std::string(curr_dir + std::string("\\") + std::string("Assets\\Particles")).data();

	// fill eveything with 0  in order to avoid problems
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&to_save, sizeof(to_save));

	to_save.lStructSize = sizeof(to_save);
	to_save.hwndOwner = NULL;
	to_save.lpstrFilter = "alienParticles\0*.alienParticles";
	to_save.lpstrFile = filename;
	to_save.nMaxFile = MAX_PATH;
	to_save.lpstrTitle = "Save As New .alienParticles";
	to_save.lpstrInitialDir = dir.data();
	to_save.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&to_save))
	{
		SetCurrentDirectoryA(curr_dir);

		std::string extension;
		App->file_system->SplitFilePath(filename, nullptr, nullptr, &extension);

		//ResourceScene* scene = new ResourceScene();
		std::string path;
		if (!App->StringCmp("alienParticles", extension.data())) {
			path = std::string(filename + std::string(".alienParticles")).data();
		}
		else {
			path = filename;
		}
		//scene->SetAssetsPath(path.data());
		//scene->CreateMetaData();
		//App->objects->SaveScene(scene);


		JSON_Value* value = json_value_init_object();
		JSON_Object* object = json_value_get_object(value);

		json_serialize_to_file_pretty(value, path.data());

		if (value != nullptr && object != nullptr)
		{
			JSONfilepack* particles = new JSONfilepack(path.data(), object, value);

			particles->StartSave();

			App->file_system->NormalizePath(path);
			std::string name = App->file_system->GetBaseFileName(path.data());

			particles->SetString("ParticleSystem.Name", name);
			JSONArraypack* properties = particles->InitNewArray("ParticleSystem.Properties");
			
			properties->SetAnotherNode();

			SaveComponent(properties);

			particles->FinishSave();
			delete particles;
		}
		else {
			LOG_ENGINE("Could not save particle system, fail when creating the file");
		}


		// last of all, refresh nodes because I have no idea if the user has created folders or moved things in the explorer. Users are bad people creating folders without using the alien engine explorer :(
		App->ui->panel_project->RefreshAllNodes();
	}
	else {
		SetCurrentDirectoryA(curr_dir);
	}
}


