#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];
//uniform mat4 view; 
uniform mat4 model;
//uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
//uniform int animate;

out vec3 frag_pos;
out vec2 texCoords;
out vec3 norms;

void main()
{
    vec4 pos = vec4(position, 1.0);
    vec4 normal4 = vec4(normals, 1.0);
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);

    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0); 
};


#shader fragment
#version 330 core

uniform sampler2D depthMap;
uniform vec3 diffuse_color;
// Ins
in vec2 texCoords;
in vec3 frag_pos;
in vec3 norms;

// Outs
out vec4 FragColor;

void main()
{
    // ----------------------- Object Color ---------------------
  
    // ----------------------------------------------------------


    // ------------------------- Light --------------------------
    vec4 ObjectColor = vec4(diffuse_color, 1.0);

    vec3 depthValue = texture(depthMap, texCoords).rgb;
    FragColor = ObjectColor * vec4(depthValue, 1.0);
    // ----------------------------------------------------------


}
