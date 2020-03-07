#pragma once

#include "Resource_.h"
#include "Color.h"
#include "MathGeoLib/include/Math/float4.h"
#include <vector>

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
	AMBIENT,
	EMISSIVE,
	HEIGHT,
	NORMALS,
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


struct UniformData
{
	struct StandardShaderProperties
	{
		float3 diffuse_color = float3::one();
	} standardShaderProperties;

	struct WaveShaderProperties
	{
		float mult_time = 1.0f;
		float amplitude = 0.75f;
	} waveShaderProperties;

	struct BasicLightingShaderProperties {
		float3 object_color = float3(1.f, 1.f, 1.f);

		// Lighting
		float ambient_strength = 0.1f;
		float specular_strength = 0.5f;
		float3 lightPosition = float3(5.f, 5.f, 5.f);
		float3 lightColor = float3(1.f, 1.f, 1.f);
	} basicLightingShaderProperties;
};


struct ShaderInputs
{
	struct StandardShaderProperties
	{
		float3 diffuse_color = float3::one();
	} standardShaderProperties;

	struct WaveShaderProperties
	{
		float mult_time = 1.0f;
		float amplitude = 0.75f;
	} waveShaderProperties;

	struct IluminatedShaderProperties {
		float3 object_color = float3(1.f, 1.f, 1.f);
	} iluminatedShaderProperties;
};

class ResourceShader; 
class ResourceTexture; 
class JSONfilepack;

class ResourceMaterial : public Resource {

public:

	ResourceMaterial();
	virtual ~ResourceMaterial();

	// meta data
	bool CreateMetaData(const u64& force_id = 0);
	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);
	bool DeleteMetaData();

	void CreateMaterialFile(const char* directory);
	void UpdateMaterialFiles();
	void SaveMaterialValues(JSONfilepack* file);
	void ReadMaterialValues(JSONfilepack* file);

	// Functionality
	void ApplyMaterial();

	void SetTexture(ResourceTexture* texture);
	void RemoveTexture();
	bool HasTexture() const; 

	// ----- ImGui ----- 
	void DisplayMaterialOnInspector();
	void MaterialHeader();
	void ShaderSelectionHeader();
	void ShaderInputsSegment();
	void TexturesSegment();
	// ------------------ 

	void ChangeShader(ResourceShader* newShader);

public:

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	u64 textureID = 0;
	ResourceTexture* texture = nullptr; // Provisional
	bool textureActivated = true;

	u64 texturesID[(uint)TextureType::MAX] = { 0 };
	ShaderInputs shaderInputs;
	ResourceShader* used_shader = nullptr; 
};
