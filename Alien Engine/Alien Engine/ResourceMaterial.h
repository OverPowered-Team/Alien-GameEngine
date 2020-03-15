#pragma once

#include "Resource_.h"
#include "Color.h"
#include "MathGeoLib/include/Math/float4.h"
#include <vector>

#define NO_TEXTURE_ID 0
/*

enum aiTextureType
{
	
	aiTextureType_NONE = 0,


	aiTextureType_DIFFUSE = 1,


	aiTextureType_SPECULAR = 2,


	aiTextureType_AMBIENT = 3,


	aiTextureType_EMISSIVE = 4,


	aiTextureType_HEIGHT = 5,


	aiTextureType_NORMALS = 6,


	aiTextureType_SHININESS = 7,


	aiTextureType_OPACITY = 8,


	aiTextureType_DISPLACEMENT = 9,


	aiTextureType_LIGHTMAP = 10,

	aiTextureType_REFLECTION = 11,


	aiTextureType_BASE_COLOR = 12,
	aiTextureType_NORMAL_CAMERA = 13,
	aiTextureType_EMISSION_COLOR = 14,
	aiTextureType_METALNESS = 15,
	aiTextureType_DIFFUSE_ROUGHNESS = 16,
	aiTextureType_AMBIENT_OCCLUSION = 17,

};
*/

enum class TextureType {
	NONE = -1,
	DIFFUSE = 0,
	SPECULAR,
	NORMALS,
	HEIGHT,
	AMBIENT,
	EMISSIVE,
	SHININESS,
	OPACITY,
	DISPLACEMENT,
	LIGHTMAP,
	REFLECTION,
	DIFFUSE_ROUGHNESS,
	AMBIENT_OCCLUSION,
	METALNESS,

	MAX
};

struct ShaderInputs
{
	struct StandardShaderProperties
	{
		float3 diffuse_color = float3::one();
		float shininess = 0.5f;
	} standardShaderProperties;

	struct WaveShaderProperties
	{
		float mult_time = 1.0f;
		float amplitude = 0.75f;
	} waveShaderProperties;

	struct IluminatedShaderProperties {
		float3 object_color = float3(1.f, 1.f, 1.f);
	} iluminatedShaderProperties;

	struct ParticleShaderProperties {
		float4 color = float4(1.f, 0.f, 0.8f, 1.f);
		float4 start_color = float4(1.f, 0.f, 0.8f, 1.f);
		float4 end_color = float4(1.f, 1.f, 1.f, 1.f);
	} particleShaderProperties;
};

class ResourceShader; 
class ResourceTexture; 
class JSONfilepack;

class ResourceMaterial : public Resource {

public:

	ResourceMaterial();
	virtual ~ResourceMaterial();

	/*bool LoadMemory() override;
	void FreeMemory() override;*/

	// meta data
	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();

	void SaveMaterialFiles();
	void SaveMaterialValues(JSONfilepack* file);
	void ReadMaterialValues(JSONfilepack* file);

	// Functionality
	void ApplyMaterial();

	void SetTexture(ResourceTexture* texture, TextureType texType = TextureType::DIFFUSE);
	const ResourceTexture* GetTexture(TextureType texType = TextureType::DIFFUSE) const;
	void RemoveTexture(TextureType texType = TextureType::DIFFUSE);
	
	bool HasTexture(TextureType texType = TextureType::DIFFUSE) const;

	void SetShader(ResourceShader* newShader);
	// ----- ImGui ----- 
	void DisplayMaterialOnInspector();
	void MaterialHeader();
	void ShaderSelectionHeader();
	void ShaderInputsSegment();
	void InputTexture(TextureType texType);
	void TexturesSegment();

	void TextureBrowser();
	// ------------------ 

public:

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	bool textureActivated = true;
	u64 texturesID[(uint)TextureType::MAX];
	ShaderInputs shaderInputs;
	ResourceShader* used_shader = nullptr; 
	const char* selectedShader = nullptr;
	ResourceTexture* selected_texture = nullptr;
	bool change_texture_menu = false;
};
