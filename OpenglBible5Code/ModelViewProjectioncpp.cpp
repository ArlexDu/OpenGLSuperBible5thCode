// ModelviewProjection.cpp
// OpenGL SuperBible
// Demonstrates OpenGL the ModelviewProjection matrix
// Program by Richard S. Wright Jr.
// 顶点乘以模型视图矩阵，生成变换的视觉坐标，随后视觉坐标再乘以投影矩阵，生成裁剪坐标，裁剪坐标就位于屏幕可见的-1到1的单位坐标系内
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
	//GLTools里面的CStopWatch：基于经过时间长短设置旋转速度
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Matrix Variables
	M3DMatrix44f mTranslate, mRotate, mModelview, mModelViewProjection;

	// 创建一个平移矩阵，表示将花托沿着Z轴负方向移动2.5个单位长度
	m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);

	// 创建一个旋转矩阵，表示以（0,1,0）为轴旋转yRot度
	m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);

	// Add the rotation to the translation, store the result in mModelView，先平移再旋转
	m3dMatrixMultiply44(mModelview, mTranslate, mRotate);

	// Add the modelview matrix to the projection matrix, 
	// the final matrix is the ModelViewProjection matrix.
	//屏幕显示的窗口区域是-1到1，所以需要投影矩阵将模型转化到这个显示窗口中
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

	// 创建花托的Batch
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

	//线状模式
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
