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
GLfloat vVerts[] = { -blockSize,-blockSize, 0.0f,//���ö���,����Ϊȫ�ֱ����������Ϳ��԰���������
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
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);//���ñ�����ɫ

	shaderManager.InitializeStockShaders();//��ʼ���洢��ɫ��

	squareBatch.Begin(GL_TRIANGLE_FAN, 4);//���Ƕ������η�װ
	squareBatch.CopyVertexData3f(vVerts);
	squareBatch.End();
}



///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//���������

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);//����ɫ���ݸ�GLT_SHADER_IDENTITY�洢��ɫ���������ɫ����ʹ��ָ������ɫ��Ĭ�ϵѿ�������ϵ����Ļ����Ⱦ����ͼ��
	squareBatch.Draw();//������ͼ���ύ����ɫ��
	// Perform the buffer swap to display back buffer
	glutSwapBuffers();//˫���壺����̨������������Ⱦ��Ȼ�������ʱ�򽻻���ǰ̨
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

	//�����жϣ���ǰ�Ķ����ڿ��Ӵ��ڷ�Χ��
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
	
	//�������¸�ֵ
	vVerts[0] = blockX;
	vVerts[1] = blockY - 2 * blockSize;

	vVerts[3] = blockX + 2 * blockSize;
	vVerts[4] = blockY - 2 * blockSize;

	vVerts[6] = blockX + 2 * blockSize;
	vVerts[7] = blockY;

	vVerts[9] = blockX;
	vVerts[10] = blockY;

	//���µĶ������ݷ�װ������
	squareBatch.CopyVertexData3f(vVerts);
	//�ػ�ҳ��
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);//���õ�ǰ����Ŀ¼

	glutInit(&argc, argv);//��ʼ��GLUT��
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);//˫��������RGBA��ɫģʽ����Ȳ��ԣ�ģ�����
	glutInitWindowSize(800, 600);//���ڴ�С
	glutCreateWindow("Move Block with Arrow Keys");//���ڱ���
	glutReshapeFunc(ChangeSize);//ע�ᴰ�ڴ�С�仯�Ļص�����
	glutDisplayFunc(RenderScene);//ע����Ⱦ�ص�����
	glutSpecialFunc(SpecialKeys);//ע����̵���¼�

	GLenum err = glewInit();//��ʼ��GLEW���ṩOPENGL��չ����ָ��
	if (GLEW_OK != err) {//������������ʼ���Ƿ����
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();

	glutMainLoop();//�ú���������֮���������ڹر�֮ǰ�����᷵�أ����������в���ϵͳ���ض���Ϣ��
	return 0;
}
