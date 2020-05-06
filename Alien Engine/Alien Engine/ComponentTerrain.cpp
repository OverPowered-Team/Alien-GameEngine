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

#include "Optick/include/optick.h"
ComponentTerrain::ComponentTerrain(GameObject* attach) : Component(attach)
{
	type = ComponentType::TERRAIN;
	name = "Terrain";
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
		ImGui::Text("Terrain Information");
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
	SetPixelData(path);
	GaussianBlur(iterations);
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

	// Raw terrain data
	terrainData = new float3[m_Width * m_Height];
	terrainDataHeight = new float3[m_Width * m_Height];

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			terrainData[y * m_Width + x] = m_data.at(x) / 255;
			terrainDataHeight[y * m_Width + x] = (m_data.at(x) / 255) * maxHeight;
		}
	}
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


	for (int z = 0; z < h; z++)
	{
		for (int x = 0; x < w; x++)
		{
			vertices[z * w + x] = float3(x - w / 2, terrainDataHeight[z * w + x].y * maxHeight, z - h / 2);
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

void ComponentTerrain::AllocateMeshBuffers()
{

	//VERTEX BUFFERS
	glGenBuffers(1, (GLuint*) & (vertex_id));

	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m_Width * m_Height, vertices, GL_STATIC_DRAW);

	//INDEX BUFFERS
	for (auto& iter_z = chunks.begin(); iter_z != chunks.end(); iter_z++)
		for (auto& iter_x = iter_z->second.begin(); iter_x != iter_z->second.end(); iter_x++)
			iter_x->second.GenerateBuffer();

	//NORMAL BUFFERS
	glGenBuffers(1, (GLuint*) & (normals_id));
	glBindBuffer(GL_ARRAY_BUFFER, normals_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m_Width * m_Height, normals, GL_STATIC_DRAW);

	//SCALED UV BUFFERS
	glGenBuffers(1, (GLuint*) & (scaled_uv_id));
	glBindBuffer(GL_ARRAY_BUFFER, scaled_uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * m_Width * m_Height, scaled_uvs, GL_STATIC_DRAW);

	delete[] scaled_uvs;

	//UV BUFFERS
	glGenBuffers(1, (GLuint*) & (uv_id));

	glBindBuffer(GL_ARRAY_BUFFER, uv_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * m_Width * m_Height, uvs, GL_STATIC_DRAW);

	delete[] uvs;

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

const float3* ComponentTerrain::GetVertices()
{
	return vertices;
}
