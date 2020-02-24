// The TexturedIdentity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

// sampler实际上是一个整数，代表将要采样的纹理单元，2D表示是2D纹理，目前的话，我们将这个值一直设置为0
uniform sampler2D colorMap;

out vec4 vFragColor;
smooth in vec2 vVaryingTexCoords;


void main(void)
   { 
   // texture函数使用插值纹理坐标对纹理进行采样，将颜色值直接分配给像素
   vFragColor = texture(colorMap, vVaryingTexCoords.st);
   }