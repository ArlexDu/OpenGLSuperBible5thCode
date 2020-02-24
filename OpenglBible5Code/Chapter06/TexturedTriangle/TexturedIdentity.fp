// The TexturedIdentity Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

// samplerʵ������һ������������Ҫ����������Ԫ��2D��ʾ��2D����Ŀǰ�Ļ������ǽ����ֵһֱ����Ϊ0
uniform sampler2D colorMap;

out vec4 vFragColor;
smooth in vec2 vVaryingTexCoords;


void main(void)
   { 
   // texture����ʹ�ò�ֵ���������������в���������ɫֱֵ�ӷ��������
   vFragColor = texture(colorMap, vVaryingTexCoords.st);
   }