// SphereWorld.cpp
// OpenGL SuperBible
// New and improved (performance) sphere world
// Program by Richard S. Wright Jr.

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLShaderManager		shaderManager;			// Shader Manager
GLMatrixStack		modelViewMatrix;		// Modelview Matrix ģ����ͼ����
GLMatrixStack		projectionMatrix;		// Projection Matrix ͶӰ����
GLFrustum			viewFrustum;			// View Frustum �Ӿ���
GLGeometryTransform	transformPipeline;		// Geometry Transform Pipeline ���α任����

GLTriangleBatch		torusBatch;
GLBatch				floorBatch;
GLTriangleBatch     sphereBatch;
GLFrame             cameraFrame; //ȫ�������


								 //////////////////////////////////////////////////////////////////
								 // This function does any needed initialization on the rendering
								 // context. 
void SetupRC()
{
	// Initialze Shader Manager
	shaderManager.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// This makes a torus
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

	// This make a sphere
	gltMakeSphere(sphereBatch, 0.1f, 26, 13);


	floorBatch.Begin(GL_LINES, 324);
	for (GLfloat x = -20.0; x <= 20.0f; x += 0.5) {
		floorBatch.Vertex3f(x, -0.55f, 20.0f);
		floorBatch.Vertex3f(x, -0.55f, -20.0f);

		floorBatch.Vertex3f(20.0f, -0.55f, x);
		floorBatch.Vertex3f(-20.0f, -0.55f, x);
	}
	floorBatch.End();

	// �������С���λ��
	for (int i = 0; i < NUM_SPHERES; i++) {
		GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		spheres[i].SetOrigin(x, 0.0f, z);
	}
}


///////////////////////////////////////////////////
// Screen changes size or is initialized
void ChangeSize(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);

	// ����ͶӰ���󣬲��������뵽ͶӰ�����ջ��
	viewFrustum.SetPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	// ���ñ任���ߣ�ʹ�����������ջ 
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	float linear = 0.1f;
	float angular = float(m3dDegToRad(5.0f));

	if (key == GLUT_KEY_UP)
		cameraFrame.MoveForward(linear);

	if (key == GLUT_KEY_DOWN)
		cameraFrame.MoveForward(-linear);

	if (key == GLUT_KEY_LEFT)
		cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);

	if (key == GLUT_KEY_RIGHT)
		cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
}

// Called to draw scene
void RenderScene(void)
{
	//��ɫֵ
	static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// ����ʱ��Ķ���
	static CStopWatch	rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// �����ɫ����������Ȼ�����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// ���浱ǰ��ģ����ͼ���󣨵�λ����
	modelViewMatrix.PushMatrix();

	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.PushMatrix(mCamera);

	// ���Ƶ���
	shaderManager.UseStockShader(GLT_SHADER_FLAT,
		transformPipeline.GetModelViewProjectionMatrix(),
		vFloorColor);
	floorBatch.Draw();

	for (int i = 0; i < NUM_SPHERES; i++) {
		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(spheres[i]);
		shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
			vSphereColor);
		sphereBatch.Draw();
		modelViewMatrix.PopMatrix();
	}

	// ������ת�Ļ��У���modelViewMatrix�Ķ����������ƽ����ת����
	modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);
	modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
		vTorusColor);
	torusBatch.Draw();

	// �ָ�֮ǰ����ģ����ͼ��ջ�ĵ�λ����
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	// Do the buffer Swap
	glutSwapBuffers();

	// Tell GLUT to do it again
	glutPostRedisplay();
}


int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);

	glutCreateWindow("OpenGL SphereWorld");
	glutSpecialFunc(SpecialKeys);
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