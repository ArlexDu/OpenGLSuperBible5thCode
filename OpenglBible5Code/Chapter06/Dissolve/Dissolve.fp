// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

uniform vec4       ambientColor;
uniform vec4       diffuseColor;   
uniform vec4       specularColor;
uniform sampler2D  cloudTexture;
uniform float      dissolveFactor;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vVaryingTexCoord;

void main(void)
    { 
	//采样纹理颜色
    vec4 vCloudSample = texture(cloudTexture, vVaryingTexCoord);
	// 判断颜色的红色通道是否满足消散阈值
    if(vCloudSample.r < dissolveFactor)
        discard;
   

    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));

    // Multiply intensity by diffuse color, force alpha to 1.0
    vFragColor = diff * diffuseColor;

    // Add in ambient light
    vFragColor += ambientColor;

	// 叠加纹理颜色信息
	//vFragColor *= vCloudSample;

    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) {
        float fSpec = pow(spec, 128.0);
        vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
        }
    }
    