#include <iostream>
#include <fstream>
#include <sstream>

#include "glew/include/glew.h"

#include "ResourceShader.h"
#include "Globals.h"

ResourceShader::ResourceShader(const char* path, const u64& id) : Resource()
{
	this->path = path;
	type = ResourceType::RESOURCE_SHADER;
	ID = id;

	renderer_id = -1;
}

ResourceShader::~ResourceShader()
{
	glDeleteProgram(renderer_id);
}

bool ResourceShader::CreateMetaData(const u64& force_id)
{
	// TODO
	return true;
}

bool ResourceShader::DeleteMetaData()
{
	// TODO
	return true;
}

bool ResourceShader::ReadBaseInfo(const char* assets_file_path)
{
	// TODO
	return true;
}

void ResourceShader::ReadLibrary(const char* meta_data)
{
	// TODO
}

void ResourceShader::ParseAndCreateShader()
{
	SHADER_PROGRAM_SOURCE source = ParseShader(path);
	renderer_id = CreateShader(source.vertex_source, source.fragment_source);
}

void ResourceShader::Bind() const
{
	glUseProgram(renderer_id);
}

void ResourceShader::Unbind() const
{
	glUseProgram(NULL);
}

void ResourceShader::SetUniform1f(const std::string& name, const float& value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void ResourceShader::SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void ResourceShader::SetUniformMat4f(const std::string& name, const math::float4x4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix.ptr());
}

SHADER_PROGRAM_SOURCE ResourceShader::ParseShader(const std::string& path)
{
	std::ifstream stream(path);

	std::string line;
	std::stringstream ss[2]; // one for each shader type

	SHADER_TYPE shader_type = SHADER_TYPE::UNKNOWN;

	while (getline(stream, line))
	{
		if (line.find("shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				shader_type = SHADER_TYPE::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				shader_type = SHADER_TYPE::FRAGMENT;
			}
		}
		else
		{
			ss[(int)shader_type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

uint ResourceShader::CreateShader(const std::string& vertex_shader, const std::string& fragment_shader)
{
	uint program = glCreateProgram();

	uint vertex_s = CompileShader(GL_VERTEX_SHADER, vertex_shader);
	uint fragment_s = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

	glAttachShader(program, vertex_s);
	glAttachShader(program, fragment_s);

	glLinkProgram(program);
	glValidateProgram(program);

	glDetachShader(program, vertex_s);
	glDetachShader(program, fragment_s);

	glDeleteShader(vertex_s);
	glDeleteShader(fragment_s);

	return program;
}

uint ResourceShader::CompileShader(const uint& shader_type, const std::string& shader_source)
{
	uint shader_id = glCreateShader(shader_type);

	const char* source = shader_source.c_str();
	glShaderSource(shader_id, 1, &source, nullptr);
	glCompileShader(shader_id);

	int result;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader_id, length, &length, message);

		std::string type_string;
		switch (shader_type)
		{
		case GL_VERTEX_SHADER:
		{
			type_string = "vertex";
		}
		break;
		case GL_FRAGMENT_SHADER:
		{
			type_string = "fragment";
		}
		break;
		default:
			type_string = "WE_NULL";
			break;
		}

		LOG_ENGINE("ERROR: Can't compile %s shader...", type_string.c_str());
		LOG_ENGINE(message);

		glDeleteShader(shader_id);

		return 0;
	}

	return shader_id;
}

int ResourceShader::GetUniformLocation(const std::string& name) const
{
	int location = glGetUniformLocation(renderer_id, name.c_str());
	if (location == -1)
	{
		LOG_ENGINE("WARNING: Uniform %s doesn't exist...", name.c_str());
	}

	return location;
}