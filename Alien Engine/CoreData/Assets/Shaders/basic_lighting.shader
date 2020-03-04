#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 vNormal; 

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform float time;

out vec2 texCoords;
out vec3 normal; 
out vec3 fragPos; 

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 

    fragPos = vec3(model * vec4(position, 1.0f));
    texCoords = vec2(uvs.x, uvs.y);
    normal = vNormal;
};

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

uniform Material material;

uniform int hasTex;
uniform sampler2D tex;

uniform vec3 lightPos; 
uniform vec3 lightColor;

uniform vec3 objectColor;
uniform vec3 viewPos; 
uniform vec3 diffuse_color;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos; 

out vec4 FragColor;


void main()
{
	if (hasTex == 0)
	{
		vec3 ambient = light.ambient * material.ambient;

		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(lightPos - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse);

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * material.specular);

		
		vec3 result = (ambient + diffuse + specular) * diffuse_color;
		FragColor = vec4(result, 1.0f);
	}

}
