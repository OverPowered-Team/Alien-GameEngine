#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 uvs;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float time;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 
};

#shader fragment
#version 330 core

uniform vec4 custom_color = vec4(0.3, 0.7, 0.1, 1.0);

out vec4 color;

void main()
{
	color = custom_color;
}
