// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.
// 利用自定义的顶点和片元着色器画一个三角形

#include <GLTools.h>            // OpenGL toolkit
#include <GLShaderManager.h>    // Shader Manager Class

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>            // Windows FreeGlut equivalent
#endif
#pragma comment(lib,"gltools.lib")

GLBatch	triangleBatch;
GLShaderManager	shaderManager;

GLint	myIdentityShader;

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
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
    
	shaderManager.InitializeStockShaders();

	// Load up a triangle
	GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f, 
		                  0.5f, 0.0f, 0.0f,
						  0.0f, 0.5f, 0.0f };

	triangleBatch.Begin(GL_TRIANGLES, 3);
	triangleBatch.CopyVertexData3f(vVerts);
	triangleBatch.End();
	//加载一对着色器，并进行编译连接到一起
	//为每个着色器指定完整的源文本
	//在着色器之后，指定参数数量，然后指定索引和每个参数的参数名
	//返回一个着色器程序对象
	myIdentityShader = shaderManager.LoadShaderPairWithAttributes("Chapter06/Triangle/Identity.vp", "Chapter06/Triangle/Identity.fp", 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
	}


///////////////////////////////////////////////////////////////////////////////
// Cleanup
void ShutdownRC()
   {
   glDeleteProgram(myIdentityShader);

   }


///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//要使用GLSL着色器，必须使用glUseProgram函数选定，这样就可以将着色器设置为活动的，之后顶点着色器
	//和片元着色器就会处理所有提交的几何图形
	glUseProgram(myIdentityShader);
	//在着色器中寻找uniform值的位置
	GLint iColorLocation = glGetUniformLocation(myIdentityShader, "vColor");
	GLfloat vColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//在着色器中寻找uniform的位置并且赋值
	//glUniform4fv(GLint location, GLuint count, GLfloat* v)
	// 第一个参数是uniform的位置，第二个参数是每个含有x个分量的数组有多少个元素，一维数组为1，二维数组为2，第三个参数是指向数组的指针
	glUniform4fv(iColorLocation, 1, vColor);

	triangleBatch.Draw();

	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
	}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
	{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Triangle");
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
