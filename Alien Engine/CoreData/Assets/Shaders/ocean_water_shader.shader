#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float iTime;

out vec2 textureCoords;

vec3 water(vec2 uv, vec3 cdir)
{
    uv *= vec2(0.25);
    
    // Parallax height distortion with two directional waves at
    // slightly different angles.
    vec2 a = 0.025 * cdir.xz / cdir.y; // Parallax offset
    float h = sin(uv.x + iTime); // Height at UV
    uv += a * h;
    h = sin(0.841471 * uv.x - 0.540302 * uv.y + iTime);
    uv += a * h;
    
    // Texture distortion
    float d1 = mod(uv.x + uv.y, 6.283185307);
    float d2 = mod((uv.x + uv.y + 0.25) * 1.3, 18.84955592);
    d1 = iTime * 0.07 + d1;
    d2 = iTime * 0.5 + d2;
    vec2 dist = vec2(
    	sin(d1) * 0.15 + sin(d2) * 0.05,
    	cos(d1) * 0.15 + cos(d2) * 0.05
    );

    vec2 dist = vec2(0.0);
    
    vec3 ret = mix(WATER_COL, WATER2_COL, waterlayer(uv + dist.xy));
    ret = mix(ret, FOAM_COL, waterlayer(vec2(1.0) - uv - dist.yx));
    return ret;
}

void main() {
    vec4 pos = vec4(position.x, 0.0, position.y, 1.0f);
    textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * tiling;

    gl_Position = projection * view * model * vec4(water(textureCoords, position),1.0);
};


#shader fragment
#version 330 core

struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;
};

uniform Material objectMaterial;

in vec2 textureCoords;

out vec4 FragColor;

void main()
{
    if(objectMaterial.hasDiffuseTexture == true){
        FragColor = texture(objectMaterial.diffuseTexture,textureCoords);
    }
    else{
        FragColor = objectMaterial.diffuse_color;
    }
}

