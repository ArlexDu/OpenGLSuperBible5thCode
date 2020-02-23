//����ͼ�β���
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

GLFrame viewFrame;
GLFrustum viewFrustum;
GLTriangleBatch torusBatch;//Բ������
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager; //�洢��ɫ����

//��־λ
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
		//�������ģʽ
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 4:
		//������״ģʽ
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 5:
		//��ʼ��״ģʽ
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}
	//ˢ�´���
	glutPostRedisplay();
}

void RenderScene(void) {
	//�����ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�Ƿ����޳�Ч��
	if (iCull) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	//�Ƿ�����Ȳ���
	if (iDepth) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	//fprintf(stdout, "Render\n");
	modelViewMatrix.PushMatrix(viewFrame);
	GLfloat vRed[] = { 1.0f,0.0f,0.0f,1.0f };
	//����ʹ�õ���Ĭ�Ϲ�Դ��ɫ����������ģ����������Ч����������Ӱ�͹��գ���Ҫ����Ĳ�����ģ����ͼ����ͶӰ�������ɫֵ
	shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);

	torusBatch.Draw();

	modelViewMatrix.PopMatrix();

	//˫���壬��̨��������û���ǰ̨
	glutSwapBuffers();
}

void SetupRC() {
	//���ñ���ɫ
	glClearColor(0.3f,0.3f,0.3f,1.0f);
	//��ʼ���洢��ɫ��
	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(7.0f);

	//��������
	gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
	glPointSize(4.0f);
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		//RotateWorld������������ĳһ����Ϊ����ת��m3dDegToRad�Ƕ�ת����
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

	//����ˢ�´���
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
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//���ù����ռ�
	glutInit(&argc, argv);//��ʼ��glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//������ȾģʽΪ˫���壬RGBA��ɫ����Ȳ��Ժ�ģ�����
	glutInitWindowSize(800, 600);//���ô��ڴ�С
	glutCreateWindow("Geometry Test Program");//���ô��ڱ���
	glutReshapeFunc(ChangeSize);//���ڴ�С�任�ص�����
	glutSpecialFunc(SpecialKeys);//���ⰴ������ص�����
	glutDisplayFunc(RenderScene);//��Ⱦ�ص�����

	//�����˵�
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Toggle depth test",1);
	glutAddMenuEntry("Toggle cull backface", 2);
	glutAddMenuEntry("Set Fill Mode", 3);
	glutAddMenuEntry("Set Line Mode", 4);
	glutAddMenuEntry("Set Point Mode", 5);

	//����Ҽ������˵�
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	GLenum err = glewInit();//��ʼ��GLEW,��ȡ��չOPENGL����ָ��
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();//������Ⱦ����

	glutMainLoop();//������Ⱦѭ��
	return 0;

}