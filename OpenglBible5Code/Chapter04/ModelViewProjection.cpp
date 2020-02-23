// ModelviewProjection.cpp
// OpenGL SuperBible
// Demonstrates OpenGL the ModelviewProjection matrix
// Program by Richard S. Wright Jr.
// �������ģ����ͼ�������ɱ任���Ӿ����꣬����Ӿ������ٳ���ͶӰ�������ɲü����꣬�ü������λ����Ļ�ɼ���-1��1�ĵ�λ����ϵ��
#include <GLTools.h>	// OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <GLBatch.h>
#include <StopWatch.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")


// Global view frustum class
GLFrustum           viewFrustum;

// The shader manager
GLShaderManager     shaderManager;

// The torus
GLTriangleBatch     torusBatch;


// Set up the viewport and the projection matrix
void ChangeSize(int w, int h)
{
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 1000.0f);
}


// Called to draw scene
void RenderScene(void)
{
	// Set up time based animation
	//GLTools�����CStopWatch�����ھ���ʱ�䳤��������ת�ٶ�
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Matrix Variables
	M3DMatrix44f mTranslate, mRotate, mModelview, mModelViewProjection;

	// ����һ��ƽ�ƾ��󣬱�ʾ����������Z�Ḻ�����ƶ�2.5����λ����
	m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);

	// ����һ����ת���󣬱�ʾ�ԣ�0,1,0��Ϊ����תyRot��
	m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);

	// Add the rotation to the translation, store the result in mModelView����ƽ������ת
	m3dMatrixMultiply44(mModelview, mTranslate, mRotate);

	// Add the modelview matrix to the projection matrix, 
	// the final matrix is the ModelViewProjection matrix.
	//��Ļ��ʾ�Ĵ���������-1��1��������ҪͶӰ����ģ��ת���������ʾ������
	m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(), mModelview);

	// Pass this completed matrix to the shader, and render the torus
	GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	shaderManager.UseStockShader(GLT_SHADER_FLAT, mModelViewProjection, vBlack);
	torusBatch.Draw();


	// Swap buffers, and immediately refresh
	glutSwapBuffers();
	glutPostRedisplay();
}

// This function does any needed initialization on the rendering
// context. 
void SetupRC()
{
	// Black background
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	shaderManager.InitializeStockShaders();

	// �������е�Batch
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

	//��״ģʽ
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("ModelViewProjection Example");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);


	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();

	glutMainLoop();
	return 0;
}
