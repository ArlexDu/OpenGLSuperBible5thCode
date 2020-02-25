// Rectangle Texture (replace) Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;
// 采样器类型送sampler2D改为sampler2DRect类型
uniform sampler2DRect  rectangleImage;

smooth in vec2 vVaryingTexCoord;

void main(void)
    { 
    vFragColor = texture(rectangleImage, vVaryingTexCoord);
    }
    