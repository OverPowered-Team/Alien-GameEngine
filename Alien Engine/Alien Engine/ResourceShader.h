#ifndef __RESOURCE_SHADER_H__
#define __RESOURCE_SHADER_H__

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

class ResourceShader : public Resource
{
	friend class ComponentMaterial;

public:
	ResourceShader() { type = ResourceType::RESOURCE_SHADER; }
	ResourceShader(const char* path, const u64& id);
	ResourceShader(const char* path);
	virtual ~ResourceShader();

	bool LoadMemory();
	void FreeMemory();

	bool CreateMetaData(const u64& force_id = 0);
	bool DeleteMetaData();

	bool ReadBaseInfo(const char* assets_file_path);
	void ReadLibrary(const char* meta_data);

public:
	void ParseAndCreateShader();

	void Bind() const;
	void Unbind() const;
	
	void SetUniform1i(const std::string& name, const int& value);
	void SetUniform1f(const std::string& name, const float& value);
	void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3);
	void SetUniformMat4f(const std::string& name, const math::float4x4& matrix);

private:
	SHADER_PROGRAM_SOURCE ParseShader(const std::string& path);
	uint CreateShader(const std::string& vertex_shader, const std::string& fragment_shader);
	uint CompileShader(const uint& shader_type, const std::string& shader_source);

	int GetUniformLocation(const std::string& name) const;

	// TODO: Create uniform cache for optimization and faster search.

private:
	uint renderer_id = 0u;
};

#endif /* __RESOURCE_SHADER_H__ */