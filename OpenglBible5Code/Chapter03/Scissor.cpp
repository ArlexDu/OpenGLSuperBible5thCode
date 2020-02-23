//剪切测试，只刷新屏幕上发生变化的部分，将opengl渲染限制在窗口中一个较小的矩形窗口中
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
	//设置剪裁空间，前两个参数设置了剪裁框的左下角，而宽度和高度分别为剪裁框对应的尺寸
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
	//设置窗口的大小和位置
	glViewport(0, 0, w, h);
}

int main(int argc, char* argv[]) {
	gltSetWorkingDirectory(argv[0]);//设置工作空间
	glutInit(&argc, argv);//初始化glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//设置渲染模式为双缓冲，RGBA颜色，深度测试和模板测试
	glutInitWindowSize(800, 600);//设置窗口大小
	glutCreateWindow("OPENGL Scissor");//设置窗口标题
	glutReshapeFunc(ChangeSize);//窗口大小变换回调函数
	glutDisplayFunc(RenderScene);//渲染回调函数

	glutMainLoop();//开启渲染循环
	return 0;

}