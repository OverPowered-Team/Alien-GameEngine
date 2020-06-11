#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float speed;
uniform float movement;

uniform sampler2D t_channel;

float hash21(in vec2 n)
{
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

mat2 makem2(in float theta)
{
	float c = cos(theta);
	float s = sin(theta);

	return mat2(c, -s, s, c); 
}

float noise(in vec2 x)
{
	return texture(t_channel, x * .01).x;
}

vec2 gradn(vec2 p)
{
	float ep = .09;
	float grad_x = noise(vec2(p.x + ep, p.y)) - noise(vec2(p.x - ep, p.y));
	float grad_y = noise(vec2(p.x, p.y + ep)) - noise(vec2(p.x, p.y - ep));

	return vec2(grad_x, grad_y);
}

void main()
{
    vec4 pos = vec4(position, 1.0);
    vec3 frag_pos = vec3(model * pos);

    gl_Position = projection * view * vec4(frag_pos, 1.0f);
};


#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
