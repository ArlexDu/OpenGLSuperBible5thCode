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
	//这里的纹理采样也不需要纹理坐标，因为一个点就是一个单独的顶点，不需要以任何的其他方式在点的表面进行插值，直接利用gl_PointCoord即可
	//gl_PointCoord是一个二分量向量，在顶点上对纹理坐标进行插值
    vFragColor = texture(starImage, gl_PointCoord) * vStarColor;
    }
    