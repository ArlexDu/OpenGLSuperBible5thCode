#include <stdio.h>
#include <iostream>

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>
#include "../../Models/Ninja/sbm.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#pragma comment(lib,"gltools.lib")

static GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat vLightPos[] = { 0.0f, 3.0f, 0.0f, 1.0f };
static const GLenum windowBuff[] = { GL_BACK_LEFT };
static const GLenum fboBuffs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

GLsizei	 screenWidth;			// Desired window or desktop width
GLsizei  screenHeight;			// Desired window or desktop height

GLboolean bFullScreen;			// Request to run full screen
GLboolean bAnimated;			// Request for continual updates


GLShaderManager		shaderManager;			// Shader Manager
GLMatrixStack		modelViewMatrix;		// Modelview Matrix
GLMatrixStack		projectionMatrix;		// Projection Matrix
GLFrustum			viewFrustum;			// View Frustum
GLGeometryTransform	transformPipeline;		// Geometry Transform Pipeline
GLFrame				cameraFrame;			// Camera frame

GLTriangleBatch		torusBatch;
GLTriangleBatch		sphereBatch;
GLBatch				floorBatch;
GLBatch             screenQuad;

GLuint				textures[3];
GLuint				processProg;
GLuint				texBO[3];
GLuint				texBOTexture;
bool                bUseFBO;
GLuint              fboName;
GLuint              depthBufferName; 
GLuint				renderBufferNames[3];

SBObject            ninja;
GLuint              ninjaTex[1];

void MoveCamera(void);
void DrawWorld(GLfloat yRot);
bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);

static float* LoadFloatData(const char *szFile, int *count)
{
	GLint lineCount = 0;
	FILE *fp;
	float* data = 0;
	
    // Open the shader file
    fopen_s(&fp,szFile, "r");
    if(fp != NULL)
	{
		char szFloat[1024];
        while ( fgets ( szFloat, sizeof szFloat, fp ) != NULL )
            lineCount++;
		
        // Go back to beginning of file
        rewind(fp);
		
        // Allocate space for all data
		data = (float*)malloc((lineCount)*sizeof(float));
		if (data != NULL)
		{	
			int index = 0;
			while ( fgets ( szFloat, sizeof szFloat, fp ) != NULL )
			{
				data[index] = (float)atof(szFloat);
				index++;
			}
			count[0] = index;
		}
        fclose(fp);
	}
    else
        return 0;    
    
    return data;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Load in a BMP file as a texture. Allows specification of the filters and the wrap mode
bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)	
{
	GLbyte *pBits;
	GLint iWidth, iHeight;

	pBits = gltReadBMPBits(szFileName, &iWidth, &iHeight);
	if(pBits == NULL)
		return false;

	// Set Wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pBits);

    // Do I need to generate mipmaps?
	if(minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_NEAREST)
		glGenerateMipmap(GL_TEXTURE_2D);    

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// OpenGL related startup code is safe to put here. Load textures, etc.
void SetupRC()
{
    GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	// Initialze Shader Manager
	shaderManager.InitializeStockShaders();

	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	// Black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef __APPLE__
    ninja.LoadFromSBM("ninja.sbm",
                      GLT_ATTRIBUTE_VERTEX,
                      GLT_ATTRIBUTE_NORMAL,
                      GLT_ATTRIBUTE_TEXTURE0);    
#else
    ninja.LoadFromSBM("../../Models/Ninja/ninja.sbm",
        GLT_ATTRIBUTE_VERTEX,
        GLT_ATTRIBUTE_NORMAL,
        GLT_ATTRIBUTE_TEXTURE0);
#endif
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 35, 35);
	gltMakeSphere(sphereBatch, 0.1f, 26, 13);

	GLfloat alpha = 0.25f;
	floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(-20.0f, -0.41f, 20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 10.0f, 0.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(20.0f, -0.41f, 20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 10.0f, 10.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);

		floorBatch.Color4f(0.0f, 1.0f, 0.0f, alpha);
		floorBatch.MultiTexCoord2f(0, 0.0f, 10.0f);
		floorBatch.Normal3f(0.0, 1.0f, 0.0f);
		floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
	floorBatch.End();

	//生成纹理
	glGenTextures(1, textures);
	//绑定纹理
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	LoadBMPTexture("Chapter08/fbo_drawbuffers/marble.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    glGenTextures(1, ninjaTex);
	glBindTexture(GL_TEXTURE_2D, ninjaTex[0]);
#ifdef __APPLE__
	LoadBMPTexture("NinjaComp.bmp", GL_LINEAR, GL_LINEAR, GL_CLAMP);
#else
	LoadBMPTexture("../../Models/Ninja/NinjaComp.bmp", GL_LINEAR, GL_LINEAR, GL_CLAMP);
#endif
	//创建帧缓冲对象
	glGenFramebuffers(1,&fboName);
	
	//帧缓冲对象其实就是一个容器，里面需要连接不同的渲染缓冲对象RBO绑定到帧缓冲对象
	//RBO可以可以是一个颜色表面，模板表面，或者深度表面
	//创建深度渲染缓冲对象
	glGenRenderbuffers(1, &depthBufferName);
	//绑定渲染缓冲区RBO
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferName);
	//分配RBO的内存空间，第二个参数为数据的格式，第三个参数是渲染区域的宽度，第四个参数是渲染区域的高度
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
	
	//创建三个颜色渲染缓冲区
	glGenRenderbuffers(3, renderBufferNames);
	//绑定颜色渲染缓冲区
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[0]);
	//开辟缓冲区空间
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[2]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);

	// 将四个RBO连接到FBO,glFramebufferRenderbuffer的第一个参数是GL_DRAW_FRAMEBUFFER或者GL_READ_FRAMEBUFFER
	//第二个参数是缓冲区映射位置（后面需要调用路由来指定着色器的输出），第三个参数永远是GL_RENDERBUFFER，第四个参数就是着色器缓冲区名称
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboName);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferName);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBufferNames[0]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, renderBufferNames[1]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, renderBufferNames[2]);

	// 载入着色器
    processProg =  gltLoadShaderPairWithAttributes("Chapter08/fbo_drawbuffers/multibuffer.vs", "Chapter08/fbo_drawbuffers/multibuffer_frag_location.fs", 3,
								GLT_ATTRIBUTE_VERTEX, "vVertex", 
								GLT_ATTRIBUTE_NORMAL, "vNormal", 
								GLT_ATTRIBUTE_TEXTURE0, "texCoord0");
	//这里的片元着色器输出不采用gl_FragData[]的方式，而是利用glBindFragDataLocation将输出映射出来
	//9.2控制像素着色器表现，映射片段输出
	glBindFragDataLocation(processProg, 0, "oStraightColor");
	glBindFragDataLocation(processProg, 1, "oGreyscale");
	glBindFragDataLocation(processProg, 2, "oLumAdjColor"); 
	glLinkProgram(processProg);


	//创建缓冲区
	glGenBuffers(3,texBO);
	//创建纹理
	glGenTextures(1, &texBOTexture);
	
	int count = 0;
	float* fileData = 0;

	// Load first texBO with a tangent-like curve, 1024 values
	fileData = LoadFloatData("Chapter08/fbo_drawbuffers/LumTan.data", &count);
	
	if (count > 0)
	{
		//绑定缓冲区到GL_TEXTURE_BUFFER_ARB,着色器可以通过纹理单元拾取来访问缓冲区
		glBindBuffer(GL_TEXTURE_BUFFER_ARB, texBO[0]);
		//将数据传递到缓冲区，第四个参数是如何使用缓冲区，GL_STATIC_DRAW表示缓冲区的内容由应用程序进行一次设置
		//并且经常用于绘制或复制到其他图像
		glBufferData(GL_TEXTURE_BUFFER_ARB, sizeof(float)*count, fileData, GL_STATIC_DRAW);
		delete fileData;
	}

	// Load second texBO with a sine-like curve, 1024 values
	fileData = LoadFloatData("Chapter08/fbo_drawbuffers/LumSin.data", &count);
	if (count > 0)
	{
		glBindBuffer(GL_TEXTURE_BUFFER_ARB, texBO[1]);
		glBufferData(GL_TEXTURE_BUFFER_ARB, sizeof(float)*count, fileData, GL_STATIC_DRAW);
		delete fileData;
	}

	// Load third texBO with a linear curve, 1024 values
	fileData = LoadFloatData("Chapter08/fbo_drawbuffers/LumLinear.data", &count);
	if (count > 0)
	{
		glBindBuffer(GL_TEXTURE_BUFFER_ARB, texBO[2]);
		glBufferData(GL_TEXTURE_BUFFER_ARB, sizeof(float)*count, fileData, GL_STATIC_DRAW);
		delete fileData;
	}

	//解除纹理单元的buffer绑定
	glBindBuffer(GL_TEXTURE_BUFFER_ARB, 0);
	//纹理缓冲区是作为普通的缓冲区来创建的，当它被绑定到一个纹理或者GL_TEXTURE_BUFFER绑定点时会成为真正的纹理缓冲区
	//纹理缓冲区必须绑定到一个纹理单元上才能真正变得有用。要将一个纹理缓冲区绑定到一个纹理上，可以调用glTexBuffer
	//但是要确保这个使用的纹理已经被绑定
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER_ARB, texBOTexture);
	glTexBufferARB(GL_TEXTURE_BUFFER_ARB, GL_R32F, texBO[0]); 
	glActiveTexture(GL_TEXTURE0);

	// Reset framebuffer binding
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Make sure all went well
	gltCheckErrors();
}

///////////////////////////////////////////////////////////////////////////////
// Update the camera based on user input, toggle display modes
// 
void SpecialKeys(int key, int x, int y)
{
	static CStopWatch cameraTimer;
	float fTime = cameraTimer.GetElapsedSeconds();
	cameraTimer.Reset(); 

	float linear = fTime * 3.0f;
	float angular = fTime * float(m3dDegToRad(60.0f));

	if(key == GLUT_KEY_UP)
		cameraFrame.MoveForward(linear);

	if(key == GLUT_KEY_DOWN)
		cameraFrame.MoveForward(-linear);

	if(key == GLUT_KEY_LEFT)
		cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);

	if(key == GLUT_KEY_RIGHT)
		cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);

	static bool bF2IsDown = false;
	if(key == GLUT_KEY_F2)
	{
		if(bF2IsDown == false)
		{
			bF2IsDown = true;
			bUseFBO = !bUseFBO;
		}
	}
	else
	{
		bF2IsDown = false; 
	}

	if(key == GLUT_KEY_F3)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER_ARB, texBOTexture);
		glTexBufferARB(GL_TEXTURE_BUFFER_ARB, GL_R32F, texBO[0]); // FIX THIS IN GLEE
		glActiveTexture(GL_TEXTURE0);
	}
	else if(key == GLUT_KEY_F4)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER_ARB, texBOTexture);
		glTexBufferARB(GL_TEXTURE_BUFFER_ARB, GL_R32F, texBO[1]); // FIX THIS IN GLEE
		glActiveTexture(GL_TEXTURE0);
	}
	else if(key == GLUT_KEY_F5)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER_ARB, texBOTexture);
		glTexBufferARB(GL_TEXTURE_BUFFER_ARB, GL_R32F, texBO[2]); // FIX THIS IN GLEE
		glActiveTexture(GL_TEXTURE0);
	}
                        
	 // Refresh the Window
	 glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// Do your cleanup here. Free textures, display lists, buffer objects, etc.
void ShutdownRC(void)
{
	// Make sure default FBO is bound
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Cleanup textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER_ARB, 0);
	glActiveTexture(GL_TEXTURE0);
	
	glDeleteTextures(1, &texBOTexture);
	glDeleteTextures(1, textures);
    glDeleteTextures(1, ninjaTex);

	// Cleanup RBOs
	glDeleteRenderbuffers(3, renderBufferNames);
	glDeleteRenderbuffers(1, &depthBufferName);

	// Cleanup FBOs
	glDeleteFramebuffers(1, &fboName);

	// Cleanup Buffer objects
	glDeleteBuffers(3, texBO);

	// Cleanup Progams
	glUseProgram(0);
	glDeleteProgram(processProg);

    ninja.Free();
}


///////////////////////////////////////////////////////////////////////////////
// This is called at least once and before any rendering occurs. If the screen
// is a resizeable window, then this will also get called whenever the window
// is resized.
void ChangeSize(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
 
	viewFrustum.SetPerspective(35.0f, float(nWidth)/float(nHeight), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelViewMatrix.LoadIdentity();

	// update screen sizes
	screenWidth = nWidth;
	screenHeight = nHeight;

	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferName);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferNames[2]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenWidth, screenHeight);
}



///////////////////////////////////////////////////////////////////////////////
// Enable and setup the GLSL program used for 
// flushes, etc.
void UseProcessProgram(M3DVector4f vLightPos, M3DVector4f vColor, int textureUnit)
{
	glUseProgram(processProg);

	// Set Matricies for Vertex Program
	glUniformMatrix4fv(glGetUniformLocation(processProg, "mvMatrix"), 
				1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(processProg, "pMatrix"), 
				1, GL_FALSE, transformPipeline.GetProjectionMatrix());

	// Set the light position
	glUniform3fv(glGetUniformLocation(processProg, "vLightPos"), 1, vLightPos);

	// Set the vertex color for rendered pixels
	glUniform4fv(glGetUniformLocation(processProg, "vColor"), 1, vColor);

	// Set the texture unit for the texBO fetch
	glUniform1i(glGetUniformLocation(processProg, "lumCurveSampler"), 1);

	// If this geometry is textured, set the texture unit
	if(textureUnit != -1)
	{
		glUniform1i(glGetUniformLocation(processProg, "bUseTexture"), 1);
		glUniform1i(glGetUniformLocation(processProg, "textureUnit0"), textureUnit);
	}
	else
	{
		glUniform1i(glGetUniformLocation(processProg, "bUseTexture"), 0);
	}

	gltCheckErrors(processProg);
}

///////////////////////////////////////////////////////////////////////////////
// Draw the scene 
// 
void DrawWorld(GLfloat yRot)
{
	M3DMatrix44f mCamera;
	modelViewMatrix.GetMatrix(mCamera);
	
	// Need light position relative to the Camera
	M3DVector4f vLightTransformed;
	m3dTransformVector4(vLightTransformed, vLightPos, mCamera);

	// Draw the light source as a small white unshaded sphere
	modelViewMatrix.PushMatrix();
		modelViewMatrix.Translatev(vLightPos);

		if(bUseFBO)
			UseProcessProgram(vLightPos, vWhite, -1);
		else
			shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vWhite);

		sphereBatch.Draw();
	modelViewMatrix.PopMatrix();

	// Draw stuff relative to the camera
	modelViewMatrix.PushMatrix();
		modelViewMatrix.Translate(0.0f, 0.2f, -2.5f);

		modelViewMatrix.PushMatrix();
			modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
            modelViewMatrix.Translate(0.0, (GLfloat)-0.60, 0.0);
            modelViewMatrix.Scale((GLfloat)0.02, (GLfloat)0.006, (GLfloat)0.02);
            
            glBindTexture(GL_TEXTURE_2D, ninjaTex[0]);

			if(bUseFBO)
			{
				UseProcessProgram(vLightTransformed, vWhite, 0);
			}
			else
			{
                shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);
			}
            ninja.Render(0,0);
        modelViewMatrix.PopMatrix();

	modelViewMatrix.PopMatrix();
}


///////////////////////////////////////////////////////////////////////////////
// Render a frame. The owning framework is responsible for buffer swaps,
// flushes, etc.
void RenderScene(void)
{
	static CStopWatch animationTimer;
	float yRot = animationTimer.GetElapsedSeconds() * 60.0f;
//	MoveCamera();

	modelViewMatrix.PushMatrix();	
		M3DMatrix44f mCamera;
		cameraFrame.GetCameraMatrix(mCamera);
		modelViewMatrix.MultMatrix(mCamera);
		
		GLfloat vFloorColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		if(bUseFBO)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboName);
			//glDrawBuffers设置着色器的输出路由
			glDrawBuffers(3, fboBuffs);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Need light position relative to the Camera
			M3DVector4f vLightTransformed;
			m3dTransformVector4(vLightTransformed, vLightPos, mCamera);
			UseProcessProgram(vLightTransformed, vFloorColor, 0);
		}
		else
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffers(1, windowBuff);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, transformPipeline.GetModelViewProjectionMatrix(), vFloorColor, 0);
		}

        glBindTexture(GL_TEXTURE_2D, textures[0]); // Marble
		floorBatch.Draw();
		DrawWorld(yRot);

	modelViewMatrix.PopMatrix();

	if(bUseFBO)
	{
		// Direct drawing to the window
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffers(1, windowBuff);
		glViewport(0, 0, screenWidth, screenHeight);

		// Source buffer reads from the framebuffer object
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboName);

		// Copy greyscale output to the left half of the screen
		//glReadBuffer指定了读取的颜色缓冲区的位置
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		//复制源是glReadBuffer指定的区域，目标区域是glDrawBuffer指定的区域
		//因为源和目标区域的矩形不需要是一样大小的，所以可以使用这个函数对被复制的像素进行缩放
		//最后两个参数是蒙版参数和过滤器，其中如果复制深度或者模板数据的话过滤器必须是GL_LINEAR
		glBlitFramebuffer(0, 0, screenWidth/2, screenHeight,
						  0, 0, screenWidth/2, screenHeight,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST );
	
		// Copy the luminance adjusted color to the right half of the screen
		glReadBuffer(GL_COLOR_ATTACHMENT2);	
		glBlitFramebuffer(screenWidth/2, 0, screenWidth, screenHeight,
						  screenWidth/2, 0, screenWidth, screenHeight,
						  GL_COLOR_BUFFER_BIT, GL_NEAREST );

		// Scale the unaltered image to the upper right of the screen
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight,
						  (int)(screenWidth *(0.8)), (int)(screenHeight*(0.8)), 
						  screenWidth, screenHeight,
						  GL_COLOR_BUFFER_BIT, GL_LINEAR );

		glBindTexture(GL_TEXTURE_2D, 0);
	} 

    // Do the buffer Swap
    glutSwapBuffers();
        
    // Do it again
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    screenWidth = 800;
    screenHeight = 600;
    bFullScreen = false; 
    bAnimated = true;
    bUseFBO = true;
    fboName = 0;
    depthBufferName = 0;

	gltSetWorkingDirectory(argv[0]);
		
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth,screenHeight);
  
    glutCreateWindow("FBO Drawbuffers");
 
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);

    SetupRC();
    glutMainLoop();    
    ShutdownRC();
    return 0;
}
