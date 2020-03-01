#shader vertex
#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

const int MAX_BONES = 100;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = glm::mat4();
    for(int i = 0; i<4; ++i)
    {
        if(BoneIDs[i]!=-1)
        {
              BoneTransform += gBones[BoneIDs[i]] * Weights[i];
        }
    }
     

    vec4 PosL = BoneTransform * vec4(Position, 1.0);
    gl_Position = gWVP * PosL;
    TexCoord0 = TexCoord;
    vec4 NormalL = BoneTransform * vec4(Normal, 0.0);
    Normal0 = (gWorld * NormalL).xyz;
    WorldPos0 = (gWorld * PosL).xyz;
}