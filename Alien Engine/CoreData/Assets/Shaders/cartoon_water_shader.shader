#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

const float tiling = 6.0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 textureCoords;

void main() {
    vec4 pos = vec4(position.x, 0.0, position.y, 1.0f);
    textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * tiling;

    gl_Position = projection * view * model * pos;
};



#shader fragment
#version 330 core

struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    sampler2D normalMap;
    bool hasNormalMap;
};


uniform float waveStrength;

uniform vec4 water_color;
uniform float moveFactor;

uniform Material objectMaterial;

in vec2 textureCoords;

void main() {

    if(objectMaterial.hasNormalMap == true && objectMaterial.hasDiffuseTexture == true)
    {
        vec2 distortion1 = (texture(objectMaterial.normalMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
        vec2 distortion2 = (texture(objectMaterial.normalMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
        vec2 totalDistorcion = distortion1 + distortion2;

        vec4 final_color = texture(objectMaterial.diffuseTexture,totalDistorcion);
        gl_FragColor = mix(final_color,water_color,0.2);
    }
    else
    {
        gl_FragColor = water_color;
    }
}
