#version 300 es
precision mediump float;  // Specify default precision for float

in vec4 vertexColor; // Input from the vertex shader
out vec4 FragColor;  // Output color

void main()
{   
    if (gl_FrontFacing) FragColor= vec4(1,0,0,1);
    else FragColor= vec4(1,1,1,1);;    
    
    //FragColor = abs(vertexColor); // Set the fragment color
    FragColor = abs(vec4(vertexColor.xyz, 1.0));
}