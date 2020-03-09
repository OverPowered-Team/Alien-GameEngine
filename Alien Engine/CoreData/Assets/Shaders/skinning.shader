#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec3 normals;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;


const int MAX_BONES = 100;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];
out vec2 texCoords;

void main()
{
  mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    vec4 PosL = BoneTransform * vec4(position, 1.0);
    gl_Position = projection * view * model * PosL;
    vec4 NormalL = BoneTransform * vec4(normals, 0.0);
    texCoords = vec2(uvs.x, uvs.y);
}

#shader fragment
#version 330 core

uniform sampler2D tex;
uniform vec3 diffuse_color;

out vec4 FragColor;
in vec2 texCoords;

void main()
{
    vec4 textureColor = vec4(texture(tex, texCoords).rgb,1.0);
    vec4 diffuse = vec4(diffuse_color,1.0f);

    if(textureColor == vec4(0,0,0,1))
        FragColor = diffuse;
    else
        FragColor = textureColor * diffuse;    
}



