#version 300 es
precision highp float;

in vec3 vNormal;    
in vec3 vWorldPos;
out vec4 FragColor;

uniform mat4 uCamera; // camera world transform (inverse view)

void main()
{
    // Camera position
    vec3 camPos = uCamera[3].xyz;

    // Direction to light (camera)
    vec3 lightDir = normalize(camPos - vWorldPos);

    // Lambertian diffuse term
    float diffuse = max(dot(normalize(vNormal), lightDir), 0.0);

    // Distance attenuation
    float dist = length(vWorldPos - camPos);
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

    // Combine
    float intensity = diffuse * attenuation;

    FragColor = vec4(vec3(intensity), 1.0);
}