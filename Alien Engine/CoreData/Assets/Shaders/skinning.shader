#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;
uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];

uniform float time;
out vec2 texCoords;

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
    
    gl_Position =  view *  vec4(position, 1.0f); 
    texCoords = vec2(uvs.x, uvs.y);
};

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



