// MoonShader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

// Another NVidia Driver non-conformance bug
// You should not have to put this here for a 3.3 driver.
//#extension GL_EXT_gpu_shader4: enable

out vec4 vFragColor;

uniform sampler2DArray moonImage;

smooth in vec3 vMoonCoords;

void main(void)
   { 
   //There is no method texture2DArray in any standard glsl version. 
   //There has been one in the EXT_texture_array extension,
   // but this has never been integrated since in glsl 130 all texture lookup functions (texture2D, texture3D, ...)
   // have been replaced by a overloaded texture command. If you are targeting 130 without extensions, you should use
	//texture(textures, vec3(UV, layer))
   //vFragColor = texture2DArray(moonImage, vMoonCoords.stp);
   vFragColor = texture(moonImage, vMoonCoords.stp);
   }