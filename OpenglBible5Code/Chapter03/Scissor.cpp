//���в��ԣ�ֻˢ����Ļ�Ϸ����仯�Ĳ��֣���opengl��Ⱦ�����ڴ�����һ����С�ľ��δ�����
#include <GLTools.h>

#ifdef __APPLE__
#include <glut/glut>
#else
#define FREEGULT_STATIC
#include <GL\glut.h>
#endif

#pragma comment(lib,"gltools.lib");

void RenderScene(void) {
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(1.0f,0.0f,0.0f,0.0f);
	//���ü��ÿռ䣬ǰ�������������˼��ÿ�����½ǣ�����Ⱥ͸߶ȷֱ�Ϊ���ÿ��Ӧ�ĳߴ�
	glScissor(100,100,600,400);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(0.0f,1.0f,0.0f,0.0f);
	glScissor(200, 200, 400, 200);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);

	glutSwapBuffers();
		
}

void ChangeSize(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	//���ô��ڵĴ�С��λ��
	glViewport(0, 0, w, h);
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//���ù����ռ�
	glutInit(&argc, argv);//��ʼ��glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//������ȾģʽΪ˫���壬RGBA��ɫ����Ȳ��Ժ�ģ�����
	glutInitWindowSize(800, 600);//���ô��ڴ�С
	glutCreateWindow("OPENGL Scissor");//���ô��ڱ���
	glutReshapeFunc(ChangeSize);//���ڴ�С�任�ص�����
	glutDisplayFunc(RenderScene);//��Ⱦ�ص�����

	glutMainLoop();//������Ⱦѭ��
	return 0;

}