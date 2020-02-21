#include "ComponentParticleSystem.h"
#include "ParticleSystem.h"
#include "Time.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ReturnZ.h"

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
	particleSystem->PreUpdate(Time::GetDT());
}

void ComponentParticleSystem::Update()
{
	particleSystem->Update(Time::GetDT());
}

void ComponentParticleSystem::PostUpdate()
{
	particleSystem->PostUpdate(Time::GetDT());
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

		// ----------------------------------- Emmitter ----------------------------------------

		if (ImGui::TreeNodeEx("Emmitter", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ParticleEmmitter* emmitter = &particleSystem->emmitter;

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

		// ----------------------------------- Particle ----------------------------------------


		if (ImGui::TreeNodeEx("Particle", ImGuiTreeNodeFlags_DefaultOpen))
		{
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

		// ----------------------------------- Renderer ----------------------------------------

		if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Billboarding ?
			if (ImGui::Combo("Billboard", &bbTypeSelected, "Screen Aligned\0World Aligned\0Axially Aligned\0None\0\0"))
			{
				particleSystem->SetBillboardType((BillboardType)bbTypeSelected);
			}

			// Material

			// Add texture
			if (ImGui::Button("Choose Texture"))
			{
			/*	App->editor->textureBrowser->SetActive(true);
				App->editor->textureBrowser->callback = this;*/
			}

			//if (resMat != nullptr)
			//{
			//	// If there is a texture, option to remove it
			//	ImGui::SameLine();
			//	if (ImGui::Button("Remove texture"))
			//	{
			//		App->editor->textureBrowser->callback = this;
			//		App->editor->textureBrowser->callback->AssignResource(0);
			//		App->editor->textureBrowser->callback = nullptr;
			//	}

			//	ImGui::Spacing();
			//	ImGui::Text(resMat->GetName());
			//	ImGui::Image((ImTextureID)resMat->gpu_id, ImVec2(ImVec2(PREVIEW_SIZE * 3, PREVIEW_SIZE * 3)), { 0,1 }, { 1,0 });
			//	ImGui::Text("Reference Counting: ");
			//	ImGui::SameLine();
			//	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", (resMat == nullptr) ? 0 : resMat->CountReferences() - 1);
			//	ImGui::Spacing();
			//	ImGui::Text("Transparency: ");
			//	ImGui::SliderFloat(" ", &particleSystem->particleInfo.color.w, 0.0f, 1.0f);
			//	ImGui::Spacing();
			//}
			//else
			{
				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture assigned");
				ImGui::Spacing();
			}

			if (ImGui::TreeNodeEx("Blending Options"))
			{
				ImGui::Text("Blending Color: ");
				if (ImGui::Combo("Source", &funcTypeSource, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendSource = (FunctionBlendType)funcTypeSource;

				if (ImGui::Combo("Destination", &funcTypeDest, "ZERO\0ONE\0SRC_COLOR\0ONE_MINUS_SRC_COLOR\0DST_COLOR\0ONE_MINUS_DST_COLOR\0SRC_ALPHA\0ONE_MINUS_SRC_ALPHA\0DST_ALPHA\0ONE_MINUS_DST_ALPHA\0CONSTANT_COLOR\0ONE_MINUS_CONSTANT_COLOR\0CONSTANT_ALPHA\0ONE_MINUS_CONSTANT_ALPHA\0\0"))
					particleSystem->funcBlendDest = (FunctionBlendType)funcTypeDest;

				ImGui::Spacing();

				ImGui::Text("Blend Equation: ");
				if (ImGui::Combo("Equation Mode", &eqTypeSelected, "ADD\0SUBTRACT\0REVERSE SUBTRACT\0\0"))
					particleSystem->eqBlend = (EquationBlendType)eqTypeSelected;

				ImGui::TreePop();
			}


			ImGui::TreePop();
		}

		/*if (ImGui::TreeNodeEx("Save & Load", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Save Particle System")) {
				App->serialization->particleCallback = this;
				App->editor->browser->OpenBrowser(BrowserState::SAVE_PARTICLE_SYSTEM);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Particle System")) {
				App->serialization->particleCallback = this;
				App->editor->browser->OpenBrowser(BrowserState::LOAD_PARTICLE_SYSTEM);
			}

			ImGui::TreePop();
		}*/

		ImGui::Spacing;
		if (ImGui::Button("Restart Particle System")) particleSystem->ResetSystem();

	}

	return true;
}

ParticleSystem* ComponentParticleSystem::GetSystem() const
{
	return nullptr;
}


