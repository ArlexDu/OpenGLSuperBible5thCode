// PointSprites.cpp
// OpenGL SuperBible
// Demonstrates Point Sprites via a flythrough star field
// Program by Richard S. Wright Jr.

#include <GLTools.h>	// OpenGL toolkit
#include <GLFrustum.h>
#include <StopWatch.h>

#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")

#define NUM_STARS 10000

GLFrustum           viewFrustum;
GLBatch             starsBatch;

GLuint	starFieldShader;	// The point sprite shader
GLint	locMVP;				// The location of the ModelViewProjection matrix uniform
GLint   locTimeStamp;       // The location of the time stamp
GLint	locTexture;			// The location of the  texture uniform

GLuint	starTexture;		// The star texture texture object


// Load a TGA as a 2D Texture. Completely initialize the state
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
	return true;
}


// This function does any needed initialization on the rendering
// context. 
void SetupRC(void)
	{
	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	// A number of shipping drivers are not conformant to the current OpenGL
	// spec and require this. NVidia... in particular. The OpenGL specification
	// states that this is always "on", in fact you can't enable or disable it
	// anymore. Adding this lines "fixes" this on non-conformant drivers, but
	// be aware, if you have a pure core (and working correctly) GL context, 
	//you should not do this
	//OpenGL默认是开启点精灵的，一般情况下没有必要glEnable
	glEnable(GL_POINT_SPRITE);

	GLfloat fColors[4][4] = {{ 1.0f, 1.0f, 1.0f, 1.0f}, // White
                             { 0.67f, 0.68f, 0.82f, 1.0f}, // Blue Stars
                             { 1.0f, 0.5f, 0.5f, 1.0f}, // Reddish
	                         { 1.0f, 0.82f, 0.65f, 1.0f}}; // Orange


	// 精灵点选择随机的颜色并放置在随机的位置
    starsBatch.Begin(GL_POINTS, NUM_STARS);
    for(int i = 0; i < NUM_STARS; i++)
        {
		int iColor = 0;		// All stars start as white

		// One in five will be blue
        if(rand() % 5 == 1)
			iColor = 1;

		// One in 50 red
		if(rand() % 50 == 1)
			iColor = 2;

		// One in 100 is amber
		if(rand() % 100 == 1)
			iColor = 3;

		
		starsBatch.Color4fv(fColors[iColor]);
			    
		M3DVector3f vPosition;
		vPosition[0] = float(3000 - (rand() % 6000)) * 0.1f;
		vPosition[1] = float(3000 - (rand() % 6000)) * 0.1f;
		vPosition[2] = -float(rand() % 1000)-1.0f;  // -1 to -1000.0f

		starsBatch.Vertex3fv(vPosition);
		}
    starsBatch.End();

	//点精灵不需要设置纹理坐标
    starFieldShader = gltLoadShaderPairWithAttributes("Chapter07/PointSprites/SpaceFlight.vp", "Chapter07/PointSprites/SpaceFlight.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex",
			GLT_ATTRIBUTE_COLOR, "vColor");

	locMVP = glGetUniformLocation(starFieldShader, "mvpMatrix");
	locTexture = glGetUniformLocation(starFieldShader, "starImage");
    locTimeStamp = glGetUniformLocation(starFieldShader, "timeStamp");
    
	glGenTextures(1, &starTexture);
	glBindTexture(GL_TEXTURE_2D, starTexture);
	LoadTGATexture("Chapter07/PointSprites/Star.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    }

// Cleanup
void ShutdownRC(void)
{
    glDeleteTextures(1, &starTexture);
}


// Called to draw scene
void RenderScene(void)
	{
	static CStopWatch timer;

	// 这里仅仅清除了颜色缓冲区域，但是没有对深度缓冲区进行清除，这是因为需要使用附加混合来对行星和背景进行混合（这里不太懂）
	glClear(GL_COLOR_BUFFER_BIT);
		
    // Turn on additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

    // 让顶点着色器决定点的大小
	glEnable(GL_PROGRAM_POINT_SIZE);

	//也可以用下面的函数来设置点的大小，但是这样设置的点会导致所有点的大小都一样，不受透视除法的影响
	//glPointSize(50);

    // Bind to our shader, set uniforms
    glUseProgram(starFieldShader);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, viewFrustum.GetProjectionMatrix());
    glUniform1i(locTexture, 0);

	// fTime goes from 0.0 to 999.0 and recycles
	float fTime = timer.GetElapsedSeconds() * 10.0f;
	fTime = fmod(fTime, 999.0f);
    glUniform1f(locTimeStamp, fTime);

    // Draw the stars
    starsBatch.Draw();
    
    glutSwapBuffers();
	glutPostRedisplay();
	}



void ChangeSize(int w, int h)
	{
	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);
	}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
    {
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Spaced Out");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
    }
	
	SetupRC();    
	glutMainLoop();
	ShutdownRC();
	return 0;
    }
