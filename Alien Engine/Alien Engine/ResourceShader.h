#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

#include <unordered_map>

#include "MathGeoLib/include/MathGeoLib.h"

#include "Resource_.h"

struct SHADER_PROGRAM_SOURCE
{
	std::string vertex_source;
	std::string fragment_source;
};

enum class SHADER_TYPE
{
	UNKNOWN = -1,
	VERTEX,
	FRAGMENT
};

enum class SHADER_TEMPLATE
{
	NO_TEMPLATE = -1,
	DIFUSSE,
	WAVE,
	BASIC_LIGHTING
};

struct ShaderInputs;

class ResourceShader : public Resource
{
	friend class ComponentMaterial;

public:
	ResourceShader();	
	ResourceShader(const char* path, const u64& id);
	ResourceShader(const char* path);
	virtual ~ResourceShader();

	bool LoadMemory();
	void FreeMemory();

	bool CreateMetaData(const u64& force_id = 0);
	bool DeleteMetaData();

	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);

	void TryToSetShaderType();

public:

	uint ParseAndCreateShader();

	bool ChangeTemplate();
	SHADER_TEMPLATE GetShaderType() const;


	void HierarchyUniforms();
	void UpdateUniforms(ShaderInputs inputs);
	void Bind() const;
	void Unbind() const;
	
	void SetUniform1i(const std::string& name, const int& value);
	void SetUniform1f(const std::string& name, const float& value);

	void SetUniformFloat3(const std::string& name, const float3& vec3);

	void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3);
	void SetUniform4f(const std::string& name, const float4& vec);

	void SetUniformMat4f(const std::string& name, const math::float4x4& matrix);

	void CreateShaderDoc(const int& type, const char* name);

private:
	SHADER_PROGRAM_SOURCE ParseShader(const std::string& path);
	uint CreateShader(const std::string& vertex_shader, const std::string& fragment_shader);
	
	uint CompileShader(const uint& shader_type, const std::string& shader_source);

	int GetUniformLocation(const std::string& name);

	// TODO: Create uniform cache for optimization and faster search.

private:

	uint shader_id;
	SHADER_TEMPLATE shaderType = SHADER_TEMPLATE::DIFUSSE;
	std::unordered_map<std::string, int> uniform_location_cache;
};

#endif /* __RESOURCE_SHADER_H__ */