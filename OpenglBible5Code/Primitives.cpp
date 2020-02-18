//��������ͼ�β���
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>//ͶӰ����������,��׵��
#include <GLGeometryTransform.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")

GLShaderManager shaderManager; //�洢��ɫ����
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrame cameraFrame;
GLFrame objectFrame;
GLFrustum viewFrustum;


GLBatch pointBatch;//��
GLBatch lineBatch;//�ߣ����������ڵĶ�������
GLBatch lineStripBatch;//�ߴ��������Ĵ�һ���������ӵ���һ������
GLBatch lineLoopBatch;//�߻������ߴ��Ļ������ٰ���β��������
GLBatch triangleBatch;//������
GLBatch triangleStripBatch;//��������ʱ�����ӳ�������
GLBatch triangleFanBatch;//��һ������ΪԲ�ģ���������ʣ�µĶ����������

GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;

GLfloat vGreen[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat vBlack[] = { 0.0f,0.0f,0.0f,1.0f };

int nStep = 0;

void SetupRC() {
	//���ñ���ɫ
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	//��ʼ���洢��ɫ��
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
	//����ɫ�ļ���ͼ��
	shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vGreen);
	pBatch->Draw();

	//��ÿ�������εĺ�ɫ������
	//���������ͬһ����ȵ�λ�û���ͬ��������������ʾÿ����������Ĵ�ɫ����ͼ�Σ�ע�ⲻ����״�ṹ������ô����������¾ͻ�����z-flighting,������������
	//�����ڵڶ��λ��Ƶ�ʱ����΢����һ��Ƭ�ε����ֵ��ʹ���ƫ�ƶ����ı�ʵ�ʵ�3D�ռ��е�����λ�á�
	glPolygonOffset(-1.0f,-1.0f);
	//����ʹ��glPolygonOffset����ƫ��ֵ֮�⣬���������ö���Ե���ƫ������伸��ͼ��
	glEnable(GL_POLYGON_OFFSET_LINE);

	//���ÿ����
	glEnable(GL_LINE_SMOOTH);
	//���û��Ч��
	glEnable(GL_BLEND);
	//���û�Ϻ�������
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//������״ģʽ��������
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(2.5f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
	pBatch->Draw();

	//������״̬λ��ԭ
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glLineWidth(1.0f);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}

void RenderScene(void) {
	//�����ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelViewMatrix.PushMatrix();
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.MultMatrix(mCamera);

	M3DMatrix44f mObjectFrame;
	objectFrame.GetMatrix(mObjectFrame);
	modelViewMatrix.MultMatrix(mObjectFrame);

	//����ʹ�õ���ƽ����ɫ�������ǽ�Identity��ɫ����������չ������Ϊ����ͼ�α任ָ��һ��4x4�ı任���󣬵��͵ľ������ģ����ͼ�����ͶӰ���󣬼���ģ����ͼͶӰ����
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

	//˫���壬��̨��������û���ǰ̨
	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		//RotateWorld������������ĳһ����Ϊ����ת��m3dDegToRad�Ƕ�ת����
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

	//����ˢ�´���
	glutPostRedisplay();
}

void KeyPressFunc(unsigned char key,int x,int y) {
	if (key == 32) {//ASCII��32ΪSPACE
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
	//���ô��ڵĴ�С��λ��
	glViewport(0, 0, w, h);
	//����ƽ��ͷ�壬����Ϊ��ֱ�����ϵ��г��Ƕȣ����ڵĿ����߶ȵ��ݺ�ȣ����ü��棬Զ�ü���
	viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelViewMatrix.LoadIdentity();
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//���ù����ռ�
	glutInit(&argc, argv);//��ʼ��glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//������ȾģʽΪ˫���壬RGBA��ɫ����Ȳ��Ժ�ģ�����
	glutInitWindowSize(800, 600);//���ô��ڴ�С
	glutCreateWindow("GL_POINT");//���ô��ڱ���
	glutReshapeFunc(ChangeSize);//���ڴ�С�任�ص�����
	glutSpecialFunc(SpecialKeys);//���ⰴ������ص�����
	glutDisplayFunc(RenderScene);//��Ⱦ�ص�����
	glutKeyboardFunc(KeyPressFunc);//����¼�

	GLenum err = glewInit();//��ʼ��GLEW,��ȡ��չOPENGL����ָ��
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();//������Ⱦ����

	glutMainLoop();//������Ⱦѭ��
	return 0;

}