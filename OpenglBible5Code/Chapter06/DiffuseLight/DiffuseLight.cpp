// DiffuseLight.cpp
// OpenGL SuperBible
// Demonstrates simple diffuse lighting
// Program by Richard S. Wright Jr.

#include <GLTools.h>	// OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
#pragma comment(lib,"gltools.lib");

GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLTriangleBatch     sphereBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

GLuint	diffuseLightShader;	// The diffuse light shader
GLint	locColor;			// The location of the diffuse color
GLint	locLight;			// The location of the Light in eye coordinates
GLint	locMVP;				// The location of the ModelViewProjection matrix uniform
GLint	locMV;				// The location of the ModelView matrix uniform
GLint	locNM;				// The location of the Normal matrix uniform


// This function does any needed initialization on the rendering
// context. 
void SetupRC(void)
	{
	// Background
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(4.0f);

    // Make the sphere
    gltMakeSphere(sphereBatch, 1.0f, 26, 13);

	diffuseLightShader = shaderManager.LoadShaderPairWithAttributes("Chapter06/DiffuseLight/DiffuseLight.vp", "Chapter06/DiffuseLight/DiffuseLight.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex",
			GLT_ATTRIBUTE_NORMAL, "vNormal");
	//漫反射颜色
	locColor = glGetUniformLocation(diffuseLightShader, "diffuseColor");
	//视图坐标系下的光源位置
	locLight = glGetUniformLocation(diffuseLightShader, "vLightPosition");
	//模型视图投影矩阵
	locMVP = glGetUniformLocation(diffuseLightShader, "mvpMatrix");
	//模型视图矩阵
	locMV  = glGetUniformLocation(diffuseLightShader, "mvMatrix");
	//将法线从模型坐标系转化到视图坐标系下的矩阵
	locNM  = glGetUniformLocation(diffuseLightShader, "normalMatrix");
	}

// Cleanup
void ShutdownRC(void)
{

}


// Called to draw scene
void RenderScene(void)
	{
	static CStopWatch rotTimer;

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
    modelViewMatrix.PushMatrix(viewFrame);
		modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 10.0f, 0.0f, 1.0f, 0.0f);

		//这个EyeLight应该是在视觉坐标系下的光源坐标
		GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
		GLfloat vDiffuseColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

		glUseProgram(diffuseLightShader);
		//设置Uniform值
		glUniform4fv(locColor, 1, vDiffuseColor);
		glUniform3fv(locLight, 1, vEyeLight);
		//glUniformMatrix4fv(GLint location, GLint count, GLboolean transpose, const GLfloat *m)
		//这里的count表示指针数组存储矩阵的数量
		//如果当前矩阵已经按照列优先顺序（OpenGL推荐的方式）进行存储，那么设置transpose为GL_TRUE
		//如果transpose设置为GL_FALSE的话，那么会导致这个矩阵在复制到着色器的时候发生变换
		glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
		glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
		glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
		//如果使用glUniform传入uniform值之后，即使在绘制之前变传入的值是没用的，如下面代码，即使改变了颜色值，但是最终的光仍然是蓝色
		vDiffuseColor[0] = 1.0f;
		//如果想改变漫反射光的颜色，那么需要再一次调用glUniform赋值,如下语句
		//glUniform4fv(locColor, 1, vDiffuseColor);
    sphereBatch.Draw();

    modelViewMatrix.PopMatrix();


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

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
    {
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Simple Diffuse Lighting");
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
