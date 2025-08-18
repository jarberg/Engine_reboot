#version 300 es
precision highp float;

layout (location = 0) in vec3 aPos; // vertex position
layout (location = 1) in vec2 aUV; // vertex position

out vec2 vUV; // pass UVs to fragment shader
out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 uPersp; 
uniform mat4 uCamera; 
uniform mat4 uTransform; // model matrix

void main()
{
    // World-space position
    vec4 worldPos = uTransform * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    // Use position as the normal (quick hack for testing)
    vNormal = normalize(vWorldPos);

    vUV = aUV; // pass UVs to fragment shader

    // Final clip-space position
    gl_Position = uPersp * uCamera * worldPos;
}