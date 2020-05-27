#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoord;

uniform mat4 model_matrix;
uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;
uniform float currUV;
uniform float nextUV;
uniform vec4 realColor;
uniform vec3 vertex1;
uniform vec3 vertex2;
uniform vec3 vertex3;
uniform vec3 vertex4;

out vec3 fPosition;
out vec3 fNormal;
out vec4 fColor;
out vec2 fTexCoord;

void main()
{
	fNormal = vec3(0, 1, 0);
	vec2 realCoord = texCoord;
	vec3 realPos = vec3(0);
	if (texCoord.x == 0)
	{
		realCoord.x = currUV;
		
		if (texCoord.y == 1)
			realPos = vertex1;
		else
			realPos = vertex2;
	}
	
	else
	{
		realCoord.x = nextUV;
		
		if (texCoord.y == 1)
			realPos = vertex3;
		else
			realPos = vertex4;
	}
	
	fColor = realColor;	
	fPosition = vec3(model_matrix * vec4(realPos, 1.0));
	fTexCoord = realCoord;
	gl_Position = mvp_matrix * vec4(realPos, 1.0);
}

#shader fragment
#version 330 core

in vec3 fPosition;
in vec3 fNormal;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

struct Material
{
	sampler2D albedo;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform float averageColor;


void main()
{
	vec4 albedo = texture(material.albedo, fTexCoord);
	if (albedo.a < 0.1)
		discard;

	FragColor = albedo * fColor;
}