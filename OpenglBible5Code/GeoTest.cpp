//几何图形测试
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>//投影矩阵容器类,视椎体
#include <GLGeometryTransform.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")

GLFrame viewFrame;
GLFrustum viewFrustum;
GLTriangleBatch torusBatch;//圆环面批
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager; //存储着色器类

//标志位
int iCull = 0;
int iDepth = 0;

void ProcessMenu(int value) {
	switch (value) {
	case 1:
		iDepth = !iDepth;
		break;
	case 2:
		iCull = !iCull;
		break;
	case 3:
		//开启填充模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 4:
		//开启线状模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 5:
		//开始点状模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}
	//刷新窗口
	glutPostRedisplay();
}

void RenderScene(void) {
	//清除颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//是否开启剔除效果
	if (iCull) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	//是否开启深度测试
	if (iDepth) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	//fprintf(stdout, "Render\n");
	modelViewMatrix.PushMatrix(viewFrame);
	GLfloat vRed[] = { 1.0f,0.0f,0.0f,1.0f };
	//这里使用的是默认光源着色器，它可以模拟出漫反射的效果，产生阴影和光照，需要传入的参数有模型视图矩阵，投影矩阵和颜色值
	shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);

	torusBatch.Draw();

	modelViewMatrix.PopMatrix();

	//双缓冲，后台缓冲完成置换到前台
	glutSwapBuffers();
}

void SetupRC() {
	//设置背景色
	glClearColor(0.3f,0.3f,0.3f,1.0f);
	//初始化存储着色器
	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(7.0f);

	//创建花托
	gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
	glPointSize(4.0f);
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		//RotateWorld世界坐标下以某一条线为轴旋转；m3dDegToRad角度转弧度
		viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
	}
	if (key == GLUT_KEY_DOWN) {
		viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
	}
	if (key == GLUT_KEY_LEFT) {
		viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
	}
	if (key == GLUT_KEY_RIGHT) {
		viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
	}

	//重新刷新窗口
	glutPostRedisplay();
}

void ChangeSize(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	//fprintf(stdout,"Change Size\n");
	//设置窗口的大小和位置
	glViewport(0, 0, w, h);
	//构建平截头体，参数为垂直方向上的市场角度，窗口的宽度与高度的纵横比，近裁剪面，远裁剪面
	viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//设置工作空间
	glutInit(&argc, argv);//初始化glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//设置渲染模式为双缓冲，RGBA颜色，深度测试和模板测试
	glutInitWindowSize(800, 600);//设置窗口大小
	glutCreateWindow("Geometry Test Program");//设置窗口标题
	glutReshapeFunc(ChangeSize);//窗口大小变换回调函数
	glutSpecialFunc(SpecialKeys);//特殊按键点击回调函数
	glutDisplayFunc(RenderScene);//渲染回调函数

	//创建菜单
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Toggle depth test",1);
	glutAddMenuEntry("Toggle cull backface", 2);
	glutAddMenuEntry("Set Fill Mode", 3);
	glutAddMenuEntry("Set Line Mode", 4);
	glutAddMenuEntry("Set Point Mode", 5);

	//鼠标右键触发菜单
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	GLenum err = glewInit();//初始化GLEW,获取拓展OPENGL函数指针
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();//设置渲染环境

	glutMainLoop();//开启渲染循环
	return 0;

}