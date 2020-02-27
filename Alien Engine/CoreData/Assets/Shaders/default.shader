#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float time;
out vec2 texCoords;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 
    texCoords = vec2(uvs.x, uvs.y);
};

#shader fragment
#version 330 core

uniform sampler2D tex;
uniform vec4 custom_color;

out vec4 FragColor;
in vec2 texCoords;

void main()
{
    FragColor = vec4(texture(tex, texCoords).rgb,1.0) * custom_color;
}
