// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.

#include <GLTools.h>            // OpenGL toolkit
#include <GLShaderManager.h>    // Shader Manager Class

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>            // Windows FreeGlut equivalent
#endif
#pragma comment(lib,"gltools.lib")

GLBatch	squareBatch;
GLShaderManager	shaderManager;

GLfloat blockSize = 0.1f;
GLfloat vVerts[] = { -blockSize,-blockSize, 0.0f,//设置顶点,设置为全局变量，这样就可以按键更改了
					blockSize,-blockSize, 0.0f,
					blockSize, blockSize, 0.0f,
					-blockSize, blockSize, 0.0f };
///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
	// Blue background
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);//设置背景颜色

	shaderManager.InitializeStockShaders();//初始化存储着色器

	squareBatch.Begin(GL_TRIANGLE_FAN, 4);//三角顶点批次封装
	squareBatch.CopyVertexData3f(vVerts);
	squareBatch.End();
}



///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//清除缓冲区

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);//将颜色传递给GLT_SHADER_IDENTITY存储着色器，这个着色器是使用指定的颜色以默认笛卡尔坐标系在屏幕上渲染几何图形
	squareBatch.Draw();//将几何图形提交给着色器
	// Perform the buffer swap to display back buffer
	glutSwapBuffers();//双缓冲：将后台缓冲区进行渲染，然后结束的时候交换到前台
}

void SpecialKeys(int key, int x,int y) {
	GLfloat stepSize = 0.025f;
	GLfloat blockX = vVerts[0];
	GLfloat blockY = vVerts[7];
	if (key == GLUT_KEY_UP) {
		blockY += stepSize;
	}
	if (key == GLUT_KEY_DOWN) {
		blockY -= stepSize;
	}
	if (key == GLUT_KEY_LEFT) {
		blockX -= stepSize;
	}
	if (key == GLUT_KEY_RIGHT) {
		blockX += stepSize;
	}

	//条件判断，当前的对象在可视窗口范围内
	if (blockX < -1.0) {
		blockX = -1.0;
	}
	if (blockX > (1.0 - 2 * blockSize)) {
		blockX = 1.0 - 2 * blockSize;
	}
	if (blockY > 1.0) {
		blockY = 1.0;
	}
	if (blockY < (-1.0+blockSize*2)) {
		blockY = -1.0 + blockSize * 2;
	}
	
	//顶点重新赋值
	vVerts[0] = blockX;
	vVerts[1] = blockY - 2 * blockSize;

	vVerts[3] = blockX + 2 * blockSize;
	vVerts[4] = blockY - 2 * blockSize;

	vVerts[6] = blockX + 2 * blockSize;
	vVerts[7] = blockY;

	vVerts[9] = blockX;
	vVerts[10] = blockY;

	//将新的顶点数据封装到批中
	squareBatch.CopyVertexData3f(vVerts);
	//重绘页面
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);//设置当前工作目录

	glutInit(&argc, argv);//初始化GLUT库
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//双缓冲区，RGBA颜色模式，深度测试，模板测试
	glutInitWindowSize(800, 600);//窗口大小
	glutCreateWindow("Move Block with Arrow Keys");//窗口标题
	glutReshapeFunc(ChangeSize);//注册窗口大小变化的回调函数
	glutDisplayFunc(RenderScene);//注册渲染回调函数
	glutSpecialFunc(SpecialKeys);//注册键盘点击事件

	GLenum err = glewInit();//初始化GLEW，提供OPENGL拓展函数指针
	if (GLEW_OK != err) {//检查驱动程序初始化是否出错
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();

	glutMainLoop();//该函数被调用之后，在主窗口关闭之前都不会返回，他负责所有操作系统的特定消息。
	return 0;
}
