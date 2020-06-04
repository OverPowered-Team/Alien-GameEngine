#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor; 

in vec2 TexCoords;

uniform sampler2D uiRender;
uniform sampler2D sceneRender;

void main()
{             
    // UI Texture
    vec4 uiColor = texture(uiRender, TexCoords).rgba;
    // Postprocessed Scene Texture
    vec3 sceneColor = texture(sceneRender, TexCoords).rgb;

    // Blend both images depending on alpha UI 
    FragColor = mix(vec4(sceneColor,1.0), uiColor, uiColor.a);
}