#include "ComponentTerrain.h"
#include "GameObject.h"

//#include "glew/include/glew.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ComponentMaterial.h"

#include "MathGeoLib/include/MathGeoLib.h"

#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "FileNode.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#include "ModuleRenderer3D.h"
#include "mmgr/mmgr.h"
#include "ResourceShader.h"
#include "Optick/include/optick.h"
#include "ComponentTransform.h"


ComponentTerrain::ComponentTerrain(GameObject* attach) : Component(attach)
{
	type = ComponentType::TERRAIN;
	name = "Terrain";

	material = new ComponentMaterial(attach);
	attach->AddComponent(material);


#ifndef GAME_VERSION
	App->objects->debug_draw_list.emplace(this, std::bind(&ComponentTerrain::DrawScene, this));
#endif 
}

ComponentTerrain::~ComponentTerrain()
{
	if (game_object_attached != nullptr && game_object_attached->HasComponent(ComponentType::MATERIAL))
	{
		//static_cast<ComponentMaterial*>(game_object_attached->GetComponent(ComponentType::MATERIAL))->not_destroy = false;
	}
}

bool ComponentTerrain::DrawInspector()
{
	static bool check;

	ImGui::PushID(this);
	check = enabled;
	if (ImGui::Checkbox("##CmpActive", &check)) {
		enabled = check;
	}
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Terrain Editor", &not_destroy, ImGuiTreeNodeFlags_DefaultOpen))
	{

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();


		/*ImGui::SetCursorPosX(50);
		ImGui::BeginGroup();
		{
			ImGui::Button("Height map", ImVec2(110,25));
			ImGui::SameLine();
			ImGui::Button("Scupt", ImVec2(110, 25));
			ImGui::SameLine();
			ImGui::Button("Paint", ImVec2(110, 25));
			ImGui::SameLine();
			ImGui::Button("Object", ImVec2(110, 25));
			
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("First group hovered");
		}
		ImGui::EndGroup();

		ImGui::Separator();*/

		static int terrain_mode;
		ImGui::Text("Terrain Mode ");
		ImGui::SameLine(200, 15);

		if (ImGui::Combo("##", &terrain_mode, "Height map\0Sculpt\0Paint\0Object\0\0"))
		{
			SetTerrainMode((TERRAIN_MODE)terrain_mode);
		}
		ImGui::Spacing();
		ImGui::Separator();

		switch (mode)
		{
		case TERRAIN_MODE::MODE_HEIGHTMAP:
			HeighMapMode();
			break;
		case TERRAIN_MODE::MODE_SCULPT:
			ScuptMode();
			break;
		case TERRAIN_MODE::MODE_PAINT:
			PaintMode();
			break;
		case TERRAIN_MODE::MODE_OBJECT:
			ObjectMode();
			break;
		default:
			break;


		}
		
	}

	return true;
}



void ComponentTerrain::SetHeigthMapTexture(ResourceTexture* tex)
{
	heigthmapTexture = tex;
	heigthmapTexture->IncreaseReferences();
}

uint ComponentTerrain::GetHeightMapID()
{
	return heightmap_id;
}

float2 ComponentTerrain::GetHeightMapSize()
{
	return float2(m_Width, m_Height);
}

void ComponentTerrain::GenerateHeightMap(const char* path, int iterations)
{
	//SetPixelData(path);
	//GaussianBlur(iterations);

	ILuint new_image_id = 0;
	ilGenImages(1, &new_image_id);
	ilBindImage(new_image_id);

	ilutRenderer(ILUT_OPENGL);

	if (ilutGLLoadImage((char*)path))
	{

		m_Width = ilGetInteger(IL_IMAGE_WIDTH);
		m_Height = ilGetInteger(IL_IMAGE_HEIGHT);

		int w = m_Width;
		int h = m_Height;

		terrainDataHeight = new float[m_Width * m_Height];
		terrainData = new float[m_Width * m_Height];
		BYTE* tmp = new BYTE[m_Width * m_Height * 3];
		ilCopyPixels(0, 0, 0, m_Width, m_Height, 1, IL_RGB, IL_UNSIGNED_BYTE, tmp);

		float* R = new float[m_Width * m_Height];
		float* G = new float[m_Width * m_Height];
		float* B = new float[m_Width * m_Height];

		for (int n = 0; n < m_Width * m_Height; n++)
		{
			R[n] = tmp[n * 3];
			G[n] = tmp[n * 3 + 1];
			B[n] = tmp[n * 3 + 2];
		}
	
		float* buf = new float[m_Width * m_Height];
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				buf[y * w + x] = (max(max(R[y * w + x], G[y * w + x]), B[y * w + x]));
			}
		}
		float value = 0.0f;
		int n = 0;

		//Iterating all image pixels
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{

				value = 0.0f;
				n = 0;
				//Iterating all nearby pixels and checking they actually exist in the image
				for (int _y = y - 1; _y <= y + 1; _y++)
				{
					if (_y > 0 && _y < h)
					{
						for (int _x = x - 1; _x <= x + 1; _x++)
						{
							if (_x > 0 && _x < w)
							{
								n++;
								value += buf[_y * w + _x];
							}
						}
					}
				}
				value /= n;
				value /= 255;
				terrainData[y * w + x] = value;
				terrainDataHeight[y * w + x] = value * maxHeight;
			}
		}

		delete[] R;
		delete[] G;
		delete[] B;

		delete[] buf;
		delete[] tmp;

		ilBindImage(0);
		ilDeleteImages(1, &new_image_id);
	}


	AllocateHeightMapBuffer();
}

void ComponentTerrain::SetPixelData(const char* path)
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

void ComponentTerrain::AllocateHeightMapBuffer()
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

void ComponentTerrain::GaussianBlur(int iterations)
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

	//// Raw terrain data
	//terrainData = new float3[m_Width * m_Height];
	//terrainDataHeight = new float3[m_Width * m_Height];

	//for (int y = 0; y < m_Height; y++)
	//{
	//	for (int x = 0; x < m_Width; x++)
	//	{
	//		terrainData[y * m_Width + x] = m_data.at(x)/255;
	//		terrainDataHeight[y * m_Width + x] = (m_data.at(x)/255) * maxHeight;
	//	}
	//}
}

void ComponentTerrain::GenerateTerrainMesh()
{
	GenerateVertices();
	GenerateIndices();
	GenerateNormals();
	GenerateTexCoords();

	AllocateMeshBuffers();
}

void ComponentTerrain::GenerateVertices()
{

	int w = m_Width;
	int h = m_Height;

	num_vertices = w * h;
	vertices = new float3[num_vertices];

	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			vertices[z * w + x] = float3(x - w / 2, terrainData[z * w + x] * maxHeight, z - h / 2);
		}
	}

}

void ComponentTerrain::GenerateIndices()
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
				iter_x->second.AllocateIndex(((z + 1) * w + x));
				iter_x->second.AllocateIndex((z * w + x + 1));
				iter_x->second.AllocateIndex((z * w + x));
			
				//Second triangle
				iter_x->second.AllocateIndex((z * w + x + 1));
				iter_x->second.AllocateIndex(((z + 1) * w + x));
				iter_x->second.AllocateIndex(((z + 1) * w + x + 1));
			
		}
	}


	for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
		for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
			iter_x->second.UpdateAABB();





}

void ComponentTerrain::GenerateNormals()
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

void ComponentTerrain::GenerateTexCoords()
{
	int w = m_Width;
	int h = m_Height;

	num_uvs = w * h;
	scaled_uvs = new float3[num_uvs];

	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			float uv_x = ((float)x / (float)w);
			float uv_y = 1 - (((float)z / (float)h));
			scaled_uvs[z * w + x] = float3(uv_x, uv_y, 1.0f);
		}
	}

	uvs = new float3[num_uvs];

	for (int z = 0; z < h - 1; z++)
	{
		for (int x = 0; x < w - 1; x++)
		{
			uvs[z * w + x] = float3(((float)x / (float)w), (1 - ((float)z / (float)h)), 1.0f);
		}
	}
}

void ComponentTerrain::AllocateMeshBuffers()
{
	// VAO BUFFER
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VERTEX BUFFERS
	glGenBuffers(1, (GLuint*) & (vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m_Width * m_Height, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//SCALED UV BUFFERS
	if (scaled_uvs != nullptr)
	{
		glGenBuffers(1, &(scaled_uv_id));
		glBindBuffer(GL_ARRAY_BUFFER, scaled_uv_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m_Width * m_Height, scaled_uvs, GL_STATIC_DRAW);
	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);										
		glEnableVertexAttribArray(1);
		delete[] scaled_uvs;
	}

	//NORMAL BUFFERS
	if (normals != nullptr)
	{
		glGenBuffers(1, (GLuint*) & (normals_id));
		glBindBuffer(GL_ARRAY_BUFFER, normals_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m_Width * m_Height, normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
	}

	////UV BUFFERS
	//if (uvs != nullptr)
	//{
	//	glGenBuffers(1, (GLuint*) & (uv_id));
	//	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * m_Width * m_Height, uvs, GL_STATIC_DRAW);
	//
	//	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//	glEnableVertexAttribArray(3);
	//	delete[] uvs;
	//}


	//INDEX BUFFERS
	for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
		for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
			iter_x->second.GenerateBuffer();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);

}

void ComponentTerrain::DeallocateMeshBuffers()
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

void ComponentTerrain::DrawGame()
{
	if (IsEnabled())
	{
		DrawTerrain();
	}
}

void ComponentTerrain::DrawScene()
{
	if (chunkDraw)
	{
		for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
			for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
				iter_x->second.Render();
	}
}

void ComponentTerrain::DrawTerrain()
{
	ComponentTransform* transform = game_object_attached->transform;
	if (transform->IsScaleNegative())
		glFrontFace(GL_CW);

	ComponentMaterial* mat = (ComponentMaterial*)game_object_attached->GetComponent(ComponentType::MATERIAL);
	if (mat == nullptr)
		return;
	ResourceMaterial* material = mat->material;

	material->ApplyMaterial();
	SetUniforms(material);



	glBindVertexArray(vao);
	for (std::map<int, std::map<int, Chunk>>::iterator it_z = chunks.begin(); it_z != chunks.end(); it_z++)
	{
		for (std::map<int, Chunk>::iterator it_x = it_z->second.begin(); it_x != it_z->second.end(); it_x++)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it_x->second.GetBufferID());
			glDrawElements(GL_TRIANGLES, it_x->second.GetNumIndices(), GL_UNSIGNED_INT, (void*)0);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	material->UnbindMaterial();
	if (transform->IsScaleNegative())
		glFrontFace(GL_CCW);
}

const float3* ComponentTerrain::GetVertices()
{
	return vertices;
}

void ComponentTerrain::SetUniforms(ResourceMaterial* resource_material)
{
	resource_material->used_shader->SetUniformMat4f("model", game_object_attached->transform->GetGlobalMatrix().Transposed());
	resource_material->used_shader->SetUniform1i("animate", animate);
}

void ComponentTerrain::SetTerrainMode(TERRAIN_MODE mode)
{
	this->mode = mode;
}

void ComponentTerrain::HeighMapMode()
{
		ImGui::Text("HEIGHT MAP");

		if (heigthmapTexture != nullptr)
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
		if (ImGui::Button("Create Blur") && heigthmapTexture != nullptr)
		{
			std::string lib_path = heigthmapTexture->GetLibraryPath();
			GenerateHeightMap(lib_path.data(), 2);
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



		ImGui::Text("MESH");

		if (ImGui::Button("Generate Mesh"))
		{
			if (terrainData != nullptr)
			{
				GenerateTerrainMesh();
			}
		}
		
	
}

void ComponentTerrain::ScuptMode()
{
	ImGui::Text("This is the Scupt tab!");
}

void ComponentTerrain::PaintMode()
{
	ImGui::Text("This is the Paint tab!");
}

void ComponentTerrain::ObjectMode()
{
	ImGui::Text("This is the Object tab!");
}
