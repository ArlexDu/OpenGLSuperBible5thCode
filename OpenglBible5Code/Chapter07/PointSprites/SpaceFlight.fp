// SpaceFlight Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

in vec4 vStarColor;

uniform sampler2D  starImage;

void main(void)
    { 
	//������������Ҳ����Ҫ�������꣬��Ϊһ�������һ�������Ķ��㣬����Ҫ���κε�������ʽ�ڵ�ı�����в�ֵ��ֱ������gl_PointCoord����
	//gl_PointCoord��һ���������������ڶ����϶�����������в�ֵ
    vFragColor = texture(starImage, gl_PointCoord) * vStarColor;
    }
    