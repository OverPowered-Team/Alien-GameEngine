#include "Application.h"
#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "ComponentTransform.h"
#include "Time.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ReturnZ.h"
#include "imgui/imgui_internal.h"

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

void ComponentParticleSystem::PreUpdate()
{
	ComponentTransform* transform = (ComponentTransform*)game_object_attached->GetComponent(ComponentType::TRANSFORM);

	particleSystem->emmitter.SetPosition(transform->GetGlobalPosition());
	particleSystem->emmitter.SetRotation(transform->GetGlobalRotation());

	if(particleSystem->isPlaying)
		particleSystem->PreUpdate(Time::GetCurrentDT());
}

void ComponentParticleSystem::Update()
{
	if (particleSystem->isPlaying)
		particleSystem->Update(Time::GetCurrentDT());
}

void ComponentParticleSystem::PostUpdate()
{
	if (particleSystem->isPlaying)
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

void ComponentParticleSystem::Reset()
{
	particleSystem->ResetSystem(); 
	particleSystem->isPlaying = false;
}

void ComponentParticleSystem::OnEnable()
{
}

void ComponentParticleSystem::OnDisable()
{
	Reset();
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
			if (ImGui::DragFloat("Spawn Rate", &spawnRate, 0.2f, 0.01f, FLT_MAX)) { emmitter->SetSpawnRate(spawnRate); }

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
			if (ImGui::Button("Load", { 120,20 })) {
					//App->serialization->particleCallback = this;
					//App->editor->browser->OpenBrowser(BrowserState::SAVE_PARTICLE_SYSTEM);
			}
				
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Load Particle System ");
			ImGui::SameLine(200, 15);
			if (ImGui::Button("Save", { 120,20 })) {
					//App->serialization->particleCallback = this;
					//App->editor->browser->OpenBrowser(BrowserState::LOAD_PARTICLE_SYSTEM);
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
		}
		ImGui::SameLine(237);
		if (ImGui::Button("Cancel", { 120,20 })) {
			selected_texture = nullptr;
			change_texture_menu = false;
		}
		ImGui::EndPopup();
	}
}

void ComponentParticleSystem::SetTexture(ResourceTexture* tex)
{
	texture = tex;
	particleSystem->texture = texture;

}

ParticleSystem* ComponentParticleSystem::GetSystem() const
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

	/*if (currentResource != nullptr)
	{
		tmp_ps = name + "Resource Material Name";
		json_object_dotset_string(object, tmp_ps.c_str(), currentResource->GetName());

		tmp_ps = name + "Resource Material UUID";
		json_object_dotset_number(object, tmp_ps.c_str(), currentResource->GetUID());

		tmp_ps = name + "Resource Material File";
		json_object_dotset_string(object, tmp_ps.c_str(), currentResource->GetFile());

		tmp_ps = name + "Resource Material Path";
		json_object_dotset_string(object, tmp_ps.c_str(), currentResource->GetExportedFile());
	}
	else
	{
		tmp_ps = name + "Resource Material Name";
		json_object_dotset_string(object, tmp_ps.c_str(), "");
		tmp_ps = name + "Resource Material UUID";
		json_object_dotset_number(object, tmp_ps.c_str(), 0);
		tmp_ps = name + "Resource Material File";
		json_object_dotset_string(object, tmp_ps.c_str(), "");
		tmp_ps = name + "Resource Material Path";
		json_object_dotset_string(object, tmp_ps.c_str(), "");
	}*/
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
}


