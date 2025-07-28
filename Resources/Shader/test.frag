#version 300 es
precision mediump float;  // Specify default precision for float

in vec4 vertexColor; // Input from the vertex shader
out vec4 FragColor;  // Output color

void main()
{
    FragColor = abs(vertexColor); // Set the fragment color
}