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

//out float visibility;

void main()
{
    // --------------- OUTS ---------------
    vec4 pos = vec4(position, 1.0);
    vec3 frag_pos = vec3(model * pos);    // ---------------------------------------

    gl_Position = projection * view * vec4(frag_pos, 1.0f);
};


#shader fragment
#version 330 core

// Ins

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
