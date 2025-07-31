#version 300 es
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0

uniform mat4 uTransform; // your transform matrix

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = uTransform*vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(aPos, 1.0); // set the output variable to a dark-red color
}