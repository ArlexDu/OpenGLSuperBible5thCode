// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

uniform vec4    ambientColor;
uniform vec4    diffuseColor;   
uniform vec4    specularColor;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;


void main(void)
    {
    // ͨ����˻��������ǿ��
    float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));

    // ������������ɫֵ����������͸����Ϊ1
    vFragColor = diff * diffuseColor;

    // ���뻷����
    vFragColor += ambientColor;


    // ���㷴���������
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
	// ���㷴��ǿ��
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) {
        float fSpec = pow(spec, 128.0);
        vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
        }
    }
    