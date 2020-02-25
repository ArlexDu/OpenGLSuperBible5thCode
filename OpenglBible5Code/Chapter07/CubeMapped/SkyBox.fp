// Skybox Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;
// 纹理采样器设置为立方体纹理采样
uniform samplerCube  cubeMap;

in vec3 vVaryingTexCoord;

void main(void)
    { 
    vFragColor = texture(cubeMap, vVaryingTexCoord);
    }
    