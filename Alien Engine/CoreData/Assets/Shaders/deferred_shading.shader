#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

const int MAX_BONES = 100;
const int MAX_SPACEMATRIX = 10;

uniform mat4 gBones[MAX_BONES];
uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform int animate;

out vec3 frag_pos;
out vec2 texCoords;
out vec3 norms;
out mat3 TBN; 

void main()
{
    vec4 pos = vec4(position, 1.0);
    texCoords = vec2(uvs.x, uvs.y);

    // --------------- Animation -------------
    if(animate == 1)
    {
          mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
            BoneTransform += gBones[BoneIDs[1]] * Weights[1];
            BoneTransform += gBones[BoneIDs[2]] * Weights[2];
            BoneTransform += gBones[BoneIDs[3]] * Weights[3];
            pos = BoneTransform * pos;
    }
    // --------------------------------------- 
    frag_pos = vec3(model * pos);
    
    for(int i = 0; i < num_space_matrix; i++)
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(frag_pos, 1.0);

    // --------------- Normals ---------------
    norms = mat3(transpose(inverse(model))) * normals;

    vec3 T = normalize(vec3(model * vec4(tangents,   0.0)));
    vec3 B = normalize(vec3(model * vec4(biTangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normals,    0.0)));

    TBN = mat3(T,B,N);
    // ---------------------------------------

    gl_Position = projection * view * vec4(frag_pos, 1.0f);
}

#shader fragment
#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec2 texCoords;
in vec3 frag_pos;
in vec3 norms;
in mat3 TBN;

struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    sampler2D specularMap;
    bool hasSpecularMap;

    sampler2D normalMap;
    bool hasNormalMap;

    float smoothness;
    float metalness;
};

uniform Material objectMaterial;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // Position
    gPosition = FragPos;

    // per-fragment normals into the gbuffer
    if(objectMaterial.hasNormalMap == true)
    {
        gNormal = texture(objectMaterial.normalMap, texCoords).rgb;
        gNormal = normal * 2.0 - 1.0;
        // normal = normal * vec3(-1, -1, 0);
        gNormal = normalize(TBN * normal);
    }
    else 
    {
        gNormal = normalize(norms);
    }

    // diffuse per-fragment color
    gColorSpec.rgb = objectMaterial.diffuse_color.xyz;
    if(objectMaterial.hasDiffuseTexture == true)
    {
        gColorSpec.rgb = gColorSpec.rgb * vec3(texture(objectMaterial.diffuseTexture, texCoords));
    }

    // specular intensity in gAlbedoSpec's alpha component
    gColorSpec.a = 1.f;
}  