#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 uvs;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 
};

#shader fragment
#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}

