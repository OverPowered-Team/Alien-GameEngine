#include "PanelTerrainEditor.h"
#include "ShortCutManager.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "FileNode.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#include "Chunk.h"
PanelTerrainEditor::PanelTerrainEditor(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{

	shortcut = App->shortcut_manager->AddShortCut("Panel Terrain Editor", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
	
}



PanelTerrainEditor::~PanelTerrainEditor()
{



}



void PanelTerrainEditor::PanelLogic()
{

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_HorizontalScrollbar;

	if (ImGui::Begin("Terrain Editor", &enabled, flags))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Puta vida :(");

		if (ImGui::TreeNodeEx("HEIGHTMAP", ImGuiTreeNodeFlags_Framed))
		{

			if(heigthmapTexture != nullptr)
				ImGui::ImageButton((ImTextureID)heigthmapTexture->id, ImVec2(30, 30));
			else
				ImGui::ImageButton((ImTextureID)0, ImVec2(30, 30));

			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_PROJECT_NODE, ImGuiDragDropFlags_SourceNoDisableHover);
				if (payload != nullptr && payload->IsDataType(DROP_ID_PROJECT_NODE)) {
					FileNode* node = *(FileNode**)payload->Data;
					if (node != nullptr && node->type == FileDropType::TEXTURE) {
						std::string path = App->file_system->GetPathWithoutExtension(node->path + node->name);
						path += "_meta.alien";
						u64 ID = App->resources->GetIDFromAlienPath(path.data());
						if (ID != 0) {
							ResourceTexture* tex = (ResourceTexture*)App->resources->GetResourceWithID(ID);
							if (tex != nullptr) {
								SetHeigthMapTexture(tex);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
		
			if (ImGui::RadioButton("###", false))
			{
			}
			ImGui::Spacing();
			ImGui::Spacing();
			if(ImGui::Button("Create Blur") && heigthmapTexture != nullptr)
			{
				std::string lib_path = heigthmapTexture->GetLibraryPath();
				GenerateHeightMap(lib_path.data(), 10);
			}

		
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "HeightMap Texture:");

			if (heigthmapTexture != nullptr)
			{
				float2 size = GetHeightMapSize();

				float maxSize = max(size.x, size.y);
				if (maxSize > 500)
				{
					float scale = 500.0f / maxSize;
					size.x *= scale;
					size.y *= scale;
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Image((void*)heigthmapTexture->id, ImVec2(size.x, size.y));
			}
			else
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture loaded");
				ImGui::Spacing();
			}


			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "HeightMap Texture Blurred:");
		
			int hp = GetHeightMapID();
			if (hp != 0)
			{
				float2 size = GetHeightMapSize();

				float maxSize = max(size.x, size.y);
				if (maxSize > 500)
				{
					float scale = 500.0f / maxSize;
					size.x *= scale;
					size.y *= scale;
				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Image((void*)hp, ImVec2(size.x, size.y));
			}
			else
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.54f, 0.0f, 1.0f), "No Texture loaded");
				ImGui::Spacing();
			}


			ImGui::TreePop();
		}

		if (ImGui::MenuItem("Textures"))
		{

		}


		if (ImGui::Button("Generate Mesh"))
		{
			if (terrainData != nullptr)
			{
				GenerateTerrainMesh();
			}
		}

	}
	ImGui::End();

}

void PanelTerrainEditor::SetHeigthMapTexture(ResourceTexture* tex)
{
	heigthmapTexture = tex;
	heigthmapTexture->IncreaseReferences();
}

uint PanelTerrainEditor::GetHeightMapID()
{
	return heightmap_id;
}

float2 PanelTerrainEditor::GetHeightMapSize()
{
	return float2(m_Width, m_Height);
}

void PanelTerrainEditor::GenerateHeightMap(const char* path, int iterations)
{
	SetPixelData(path);
	GaussianBlur(iterations);
	AllocateHeightMapBuffer();
}

void PanelTerrainEditor::SetPixelData(const char* path)
{
	ILuint new_image_id = 0;
	ilGenImages(1, &new_image_id);
	ilBindImage(new_image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilutGLLoadImage((char*)path))
	{

		m_Width = ilGetInteger(IL_IMAGE_WIDTH);
		m_Height = ilGetInteger(IL_IMAGE_HEIGHT);


		m_data.resize(m_Width * m_Height, float3(0.f));

		ilCopyPixels(0, 0, 0, m_Width, m_Height, 1, IL_RGB, IL_FLOAT, m_data.data());

		ILenum err = ilGetError();
		if (err != IL_NO_ERROR)
		{
			const char* err_str = iluErrorString(err);
			std::cout << "Error copy pixels RGB" << std::endl;
		}


		ilBindImage(0);
		ilDeleteImages(1, &new_image_id);
	}


}

void PanelTerrainEditor::AllocateHeightMapBuffer()
{

	if (m_data.data())
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &heightmap_id);

		glBindTexture(GL_TEXTURE_2D, heightmap_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_data.data());

		
	}



}

void PanelTerrainEditor::GaussianBlur(int iterations)
{
	ILuint handle;
	ilGenImages(1, &handle);

	ilBindImage(handle);

	ilTexImage(m_Width, m_Height, 1, 3, IL_RGB, IL_FLOAT, m_data.data());
	iluBlurGaussian(iterations);

	ILenum err = ilGetError();
	if (err != IL_NO_ERROR)
	{
		const char* err_str = iluErrorString(err);
		std::cout << "Error gaussian blur: " << err_str << std::endl;
	}

	ilCopyPixels(0, 0, 0, m_Width, m_Height, 1, IL_RGB, IL_FLOAT, m_data.data());
	err = ilGetError();

	if (err != IL_NO_ERROR)
	{
		const char* err_str = iluErrorString(err);
		std::cout << "Error copy pixels" << std::endl;

	}

	// Raw terrain data
	terrainData = new float3[m_Width * m_Height];
	terrainDataHeight = new float3[m_Width * m_Height];

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			terrainData[y * m_Width + x] = m_data.at(x)/255;
			terrainDataHeight[y * m_Width + x] = (m_data.at(x)/255) * maxHeight;
		}
	}
}

void PanelTerrainEditor::GenerateTerrainMesh()
{
	GenerateVertices();
	GenerateIndices();
	GenerateNormals();
	GenerateTexCoords();

	AllocateMeshBuffers();
}

void PanelTerrainEditor::GenerateVertices()
{

	int w = m_Width;
	int h = m_Height;

	num_vertices = w * h;


	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			vertices[z * w + x] = float3(x - w / 2, terrainDataHeight[z * w + x].y * maxHeight, z - h / 2);
		}
	}

}

void PanelTerrainEditor::GenerateIndices()
{
	int w = m_Width;
	int h = m_Height;


	for (int z = 0; z < h - 1; z++)
	{
		for (int x = 0; x < w - 1; x++)
		{
			int chunkX = floor(x / CHUNK_W);
			int chunkZ = floor(z / CHUNK_H);

			std::map<int, std::map<int, Chunk>>::iterator iter_z = chunks.find(chunkZ);
			if (iter_z == chunks.end())
				iter_z = chunks.insert(std::pair<int, std::map<int, Chunk>>(chunkZ, std::map<int, Chunk>())).first;
			
			std::map<int, Chunk>::iterator iter_x = iter_z->second.find(chunkX);
			if (iter_x == iter_z->second.end())
			{
				Chunk new_chunk = Chunk(this);
				iter_x = iter_z->second.insert(std::pair<int, Chunk>(chunkX, new_chunk)).first;
			}
			//First triangle
			iter_x->second.AllocateIndex((z + 1) * w + x);
			iter_x->second.AllocateIndex(z * w + x + 1);
			iter_x->second.AllocateIndex(z * w + x);

			//Second triangle
			iter_x->second.AllocateIndex((z * w + x + 1));
			iter_x->second.AllocateIndex((z + 1) * w + x);
			iter_x->second.AllocateIndex((z + 1) * w + x + 1);
		}
	}


	for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
		for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
			iter_x->second.UpdateAABB();
	
		
	


}

void PanelTerrainEditor::GenerateNormals()
{
	int w = m_Width;
	int h = m_Height;

	num_normals = w * h;

	normals = new float3[num_normals];

	int x0 = 0;
	int y0 = 0;

	CAP(x0, 0, m_Width - 1);
	CAP(w, 0, m_Width - 1);
	CAP(y0, 0, m_Height - 1);
	CAP(h, 0, m_Height - 1);

	for (int z = y0; z < h; z++)
	{
		for (int x = x0; x < w; x++)
		{
			Triangle t;
			float3 norm = float3::zero();

			//Top left
			if (x - 1 > 0 && z - 1 > 0)
			{
				t.a = vertices[(z)*w + x];
				t.b = vertices[(z - 1) * w + x];
				t.c = vertices[(z)*w + x - 1];
				norm += t.NormalCCW();
			}
			//Top right
			if (x + 1 < m_Width && z - 1 > 0)
			{
				t.a = vertices[(z)*w + x];
				t.b = vertices[(z)*w + x + 1];
				t.c = vertices[(z - 1) * w + x];
				norm += t.NormalCCW();
			}
			//Bottom left
			if (x - 1 > 0 && z + 1 < m_Height)
			{
				t.a = vertices[(z)*w + x];
				t.b = vertices[(z)*w + x - 1];
				t.c = vertices[(z + 1) * w + x];
				norm += t.NormalCCW();
			}
			//Bottom right
			if (x + 1 < m_Width && z + 1 < m_Height)
			{
				t.a = vertices[(z)*w + x];
				t.b = vertices[(z + 1) * w + x];
				t.c = vertices[(z)*w + x + 1];
				norm += t.NormalCCW();
			}
			norm.Normalize();
			normals[z * w + x] = norm;
		}
	}

}

void PanelTerrainEditor::GenerateTexCoords()
{
	int w = m_Width;
	int h = m_Height;

	num_uvs = w * h;
	scaled_uvs = new float2[num_uvs];

	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			float uv_x = ((float)x / (float)w) / texScale;
			float uv_y = 1 - (((float)z / (float)h) / texScale);
			scaled_uvs[z * w + x] = float2(uv_x, uv_y);

		}
	}

	uvs = new float2[num_uvs];

	for (int z = 0; z < h - 1; z++)
	{
		for (int x = 0; x < w - 1; x++)
		{
			uvs[z * w + x] = float2(((float)x / (float)w), (1 - ((float)z / (float)h)));
		}
	}
}

void PanelTerrainEditor::AllocateMeshBuffers()
{

	//VERTEX BUFFERS
	glGenBuffers(1, (GLuint*) & (vertex_id));

	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* m_Width* m_Height, vertices, GL_STATIC_DRAW);

	//INDEX BUFFERS
	for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
		for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
			iter_x->second.GenerateBuffer();

	//NORMAL BUFFERS
	glGenBuffers(1, (GLuint*) & (normals_id));
	glBindBuffer(GL_ARRAY_BUFFER, normals_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3)* m_Width * m_Height, normals, GL_STATIC_DRAW);

	//SCALED UV BUFFERS
	glGenBuffers(1, (GLuint*) & (scaled_uv_id));
	glBindBuffer(GL_ARRAY_BUFFER, scaled_uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * m_Width * m_Height, scaled_uvs, GL_STATIC_DRAW);

	delete[] scaled_uvs;

	//UV BUFFERS
	glGenBuffers(1, (GLuint*) & (uv_id));

	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2)* m_Width * m_Height, uvs, GL_STATIC_DRAW);

	delete[] uvs;

}

void PanelTerrainEditor::DeallocateMeshBuffers()
{
	//VERTEX BUFFERS
	RELEASE_ARRAY(vertices);
	glDeleteBuffers(1, (GLuint*)&vertex_id);
	vertex_id = 0;

	//INDEX BUFFERS
	chunks.clear();

	//NORMAL BUFFERS
	RELEASE_ARRAY(normals);
	glDeleteBuffers(1, (GLuint*)&normals_id);
	normals_id = 0;

	//SCALED UV BUFFERS
	glDeleteBuffers(1, (GLuint*)&scaled_uv_id);
	scaled_uv_id = 0;

	//UV BUFFERS
	glDeleteBuffers(1, (GLuint*)&uv_id);
	uv_id = 0;
}

const float3* PanelTerrainEditor::GetVertices()
{
	return vertices;
}
