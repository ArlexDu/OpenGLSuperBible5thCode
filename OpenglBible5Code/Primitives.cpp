//基础几何图形测试
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

GLShaderManager shaderManager; //存储着色器类
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrame cameraFrame;
GLFrame objectFrame;
GLFrustum viewFrustum;


GLBatch pointBatch;//点
GLBatch lineBatch;//线，将两个相邻的顶点相连
GLBatch lineStripBatch;//线带，连续的从一个顶点连接到下一个顶点
GLBatch lineLoopBatch;//线环，在线带的基础上再把首尾顶点相连
GLBatch triangleBatch;//三角形
GLBatch triangleStripBatch;//将顶点逆时针连接成三角形
GLBatch triangleFanBatch;//第一个顶点为圆心，依次连接剩下的顶点组成扇形

GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;

GLfloat vGreen[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat vBlack[] = { 0.0f,0.0f,0.0f,1.0f };

int nStep = 0;

void SetupRC() {
	//设置背景色
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	//初始化存储着色器
	shaderManager.InitializeStockShaders();
	glEnable(GL_DEPTH_TEST);
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
	cameraFrame.MoveForward(-15.0f);
	// Some points, more or less in the shape of Florida
	GLfloat vCoast[24][3] = { { 2.80, 1.20, 0.0 },{ 2.0,  1.20, 0.0 },
							{ 2.0,  1.08, 0.0 },{ 2.0,  1.08, 0.0 },
							{ 0.0,  0.80, 0.0 },{ -.32, 0.40, 0.0 },
							{ -.48, 0.2, 0.0 },{ -.40, 0.0, 0.0 },
							{ -.60, -.40, 0.0 },{ -.80, -.80, 0.0 },
							{ -.80, -1.4, 0.0 },{ -.40, -1.60, 0.0 },
							{ 0.0, -1.20, 0.0 },{ .2, -.80, 0.0 },
							{ .48, -.40, 0.0 },{ .52, -.20, 0.0 },
							{ .48,  .20, 0.0 },{ .80,  .40, 0.0 },
							{ 1.20, .80, 0.0 },{ 1.60, .60, 0.0 },
							{ 2.0, .60, 0.0 },{ 2.2, .80, 0.0 },
							{ 2.40, 1.0, 0.0 },{ 2.80, 1.0, 0.0 } };
	pointBatch.Begin(GL_POINTS,24);
	pointBatch.CopyVertexData3f(vCoast);
	pointBatch.End();

	lineBatch.Begin(GL_LINES, 24);
	lineBatch.CopyVertexData3f(vCoast);
	lineBatch.End();

	lineStripBatch.Begin(GL_LINE_STRIP, 24);
	lineStripBatch.CopyVertexData3f(vCoast);
	lineStripBatch.End();
	
	lineLoopBatch.Begin(GL_LINE_LOOP,24);
	lineLoopBatch.CopyVertexData3f(vCoast);
	lineLoopBatch.End();

	// For Triangles, we'll make a Pyramid
	GLfloat vPyramid[12][3] = { -2.0f, 0.0f, -2.0f,
								2.0f, 0.0f, -2.0f,
								0.0f, 4.0f, 0.0f,

								2.0f, 0.0f, -2.0f,
								2.0f, 0.0f, 2.0f,
								0.0f, 4.0f, 0.0f,

								2.0f, 0.0f, 2.0f,
								-2.0f, 0.0f, 2.0f,
								0.0f, 4.0f, 0.0f,

								-2.0f, 0.0f, 2.0f,
								-2.0f, 0.0f, -2.0f,
								0.0f, 4.0f, 0.0f };
	triangleBatch.Begin(GL_TRIANGLES, 12);
	triangleBatch.CopyVertexData3f(vPyramid);
	triangleBatch.End();

	// For a Triangle fan, just a 6 sided hex. Raise the center up a bit
	GLfloat vPoints[100][3];    // Scratch array, more than we need
	int nVerts = 0;
	GLfloat r = 3.0f;
	vPoints[nVerts][0] = 0.0f;
	vPoints[nVerts][1] = 0.0f;
	vPoints[nVerts][2] = 0.0f;

	for (GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
		nVerts++;
		vPoints[nVerts][0] = float(cos(angle)) * r;
		vPoints[nVerts][1] = float(sin(angle)) * r;
		vPoints[nVerts][2] = -0.5f;
	}

	// Close the fan
	nVerts++;
	vPoints[nVerts][0] = r;
	vPoints[nVerts][1] = 0;
	vPoints[nVerts][2] = 0.0f;

	// Load it up
	triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
	triangleFanBatch.CopyVertexData3f(vPoints);
	triangleFanBatch.End();

	// For triangle strips, a little ring or cylinder segment
	int iCounter = 0;
	GLfloat radius = 3.0f;
	for (GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f)
	{
		GLfloat x = radius * sin(angle);
		GLfloat y = radius * cos(angle);

		// Specify the point and move the Z value up a little	
		vPoints[iCounter][0] = x;
		vPoints[iCounter][1] = y;
		vPoints[iCounter][2] = -0.5;
		iCounter++;

		vPoints[iCounter][0] = x;
		vPoints[iCounter][1] = y;
		vPoints[iCounter][2] = 0.5;
		iCounter++;
	}

	// Close up the loop
	vPoints[iCounter][0] = vPoints[0][0];
	vPoints[iCounter][1] = vPoints[0][1];
	vPoints[iCounter][2] = -0.5;
	iCounter++;

	vPoints[iCounter][0] = vPoints[1][0];
	vPoints[iCounter][1] = vPoints[1][1];
	vPoints[iCounter][2] = 0.5;
	iCounter++;

	// Load the triangle strip
	triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
	triangleStripBatch.CopyVertexData3f(vPoints);
	triangleStripBatch.End();
}
void DrawWireFramedBatch(GLBatch* pBatch) {
	//画纯色的几何图形
	shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vGreen);
	pBatch->Draw();

	//画每个三角形的黑色轮廓线
	//如果我们在同一个深度的位置化不同的贴花，比如显示每个三角网格的纯色几何图形（注意不是线状结构），那么这样的情况下就会引发z-flighting,针对这样的情况
	//可以在第二次绘制的时候稍微调节一下片段的深度值，使深度偏移而不改变实际的3D空间中的物理位置。
	glPolygonOffset(-1.0f,-1.0f);
	//除了使用glPolygonOffset设置偏移值之外，还必须启用多变性单独偏移来填充几何图形
	glEnable(GL_POLYGON_OFFSET_LINE);

	//设置抗锯齿
	glEnable(GL_LINE_SMOOTH);
	//启用混合效果
	glEnable(GL_BLEND);
	//设置混合函数参数
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//按照线状模式画几何体
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(2.5f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
	pBatch->Draw();

	//将各个状态位还原
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glLineWidth(1.0f);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

void RenderScene(void) {
	//清除颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelViewMatrix.PushMatrix();
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.MultMatrix(mCamera);

	M3DMatrix44f mObjectFrame;
	objectFrame.GetMatrix(mObjectFrame);
	modelViewMatrix.MultMatrix(mObjectFrame);

	//这里使用的是平面着色器，它是将Identity着色器进行了拓展，允许为几何图形变换指定一个4x4的变换矩阵，典型的就是左乘模型视图矩阵和投影矩阵，即是模型视图投影矩阵
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);

	switch (nStep) {
	case 0:
		glPointSize(4.0f);
		pointBatch.Draw();
		glPointSize(1.0f);
		break;
	case 1:
		glLineWidth(2.0f);
		lineBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 2:
		glLineWidth(2.0f);
		lineStripBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 3:
		glLineWidth(2.0f);
		lineLoopBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 4:
		DrawWireFramedBatch(&triangleBatch);
		break;
	case 5:
		DrawWireFramedBatch(&triangleStripBatch);
		break;
	case 6:
		DrawWireFramedBatch(&triangleFanBatch);
		break;
	}

	modelViewMatrix.PopMatrix();

	//双缓冲，后台缓冲完成置换到前台
	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		//RotateWorld世界坐标下以某一条线为轴旋转；m3dDegToRad角度转弧度
		objectFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
	}
	if (key == GLUT_KEY_DOWN) {
		objectFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
	}
	if (key == GLUT_KEY_LEFT) {
		objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
	}
	if (key == GLUT_KEY_RIGHT) {
		objectFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
	}

	//重新刷新窗口
	glutPostRedisplay();
}

void KeyPressFunc(unsigned char key,int x,int y) {
	if (key == 32) {//ASCII码32为SPACE
		nStep++;
		if (nStep > 6) {
			nStep = 0;
		}
	}
	switch (nStep) {
	case 0:
		glutSetWindowTitle("GL_POINT");
		break;
	case 1:
		glutSetWindowTitle("GL_LINES");
		break;
	case 2:
		glutSetWindowTitle("GL_LINE_STRIP");
		break;
	case 3:
		glutSetWindowTitle("GL_LINE_LOOP");
		break;
	case 4:
		glutSetWindowTitle("GL_TRIANGLES");
		break;
	case 5:
		glutSetWindowTitle("GL_TRIANGLE_STRIP");
		break;
	case 6:
		glutSetWindowTitle("GL_TRIANGLE_FAN");
		break;
	}
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
	modelViewMatrix.LoadIdentity();
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//设置工作空间
	glutInit(&argc, argv);//初始化glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//设置渲染模式为双缓冲，RGBA颜色，深度测试和模板测试
	glutInitWindowSize(800, 600);//设置窗口大小
	glutCreateWindow("GL_POINT");//设置窗口标题
	glutReshapeFunc(ChangeSize);//窗口大小变换回调函数
	glutSpecialFunc(SpecialKeys);//特殊按键点击回调函数
	glutDisplayFunc(RenderScene);//渲染回调函数
	glutKeyboardFunc(KeyPressFunc);//点击事件

	GLenum err = glewInit();//初始化GLEW,获取拓展OPENGL函数指针
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();//设置渲染环境

	glutMainLoop();//开启渲染循环
	return 0;

}