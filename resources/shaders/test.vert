#version 300 es
precision highp float;

layout (location = 0) in vec3 aPos; // vertex position
layout (location = 1) in vec2 aUV; // vertex position
layout (location = 2) in vec3 aNormal; // vertex position

out vec2 vUV; // pass UVs to fragment shader
out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 uPersp; 
uniform mat4 uCamera; 
uniform mat4 uTransform; // model matrix

void main()
{
    // Final clip-space position (MVP * position)
    mat4 MVP = uPersp * uCamera * uTransform;

    // Model -> World
    vec4 worldPos4 = MVP * vec4(aPos, 1.0);
    vWorldPos = worldPos4.xyz;

    mat3 normalMat = mat3(transpose(inverse(uTransform)));
    vNormal = normalize(normalMat * aNormal);

    vUV = aUV;

    gl_Position = worldPos4;
}