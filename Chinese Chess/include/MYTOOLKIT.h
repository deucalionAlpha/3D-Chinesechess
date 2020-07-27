#ifndef MYTOOLKIT_H
#define MYTOOLKIT_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include<CAMERA.h>
#include<SHADER.h>
#include<MODEL.h>
#include<TEXTURE.h>
#include <vector>
class Toolkit
{
public:
	static Camera *myCamera;
	static vector<Shader *>myShader;
	static int modelShader;
	static GLFWwindow * window;
	static int scrWidth, scrHeight;

public:
	static void createCamera(int width, int height)
	{
		if (myCamera != NULL)delete myCamera;
		myCamera = new Camera(width, height);
	}
	static int createShader(const char * vertexShaderSource, const char * fragmentShaderSource,const char *gshader=NULL)
	{
		Shader * temp = new Shader(vertexShaderSource, fragmentShaderSource,gshader);
		myShader.push_back(temp);
		return myShader.size() - 1;
	}

	static int InitOpenGL(const char *name,const unsigned int SCR_WIDTH,const unsigned int SCR_HEIGHT)
	{
		scrWidth = SCR_WIDTH;
		scrHeight = SCR_HEIGHT;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name, NULL, NULL);
		if (window == NULL)
		{
			cout << "Failed to create window" << endl;
			glfwTerminate();
			return 0;
		}
		Toolkit::createCamera(SCR_WIDTH, SCR_HEIGHT);
		if (myCamera == NULL)
		{
			cout << "Initializing Camera failed!" << endl;
			return 0;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		Camera::registerCamera(window, myCamera);
		
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Failed to initialize glad" << endl;
			return 0;
		}
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		return 1;
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
	static void checkScrShotRequest()
	{
		static bool gPressed = false;
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			if (gPressed == false)
			{
				gPressed = true;
				screenShot();
			}
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)gPressed = false;
	}
	static void screenShot()
	{
		static const int BMP_Header_Length = 54;
		FILE*    pDummyFile;
		FILE*    pWritingFile;
		GLubyte* pPixelData;
		GLubyte  BMP_Header[BMP_Header_Length];
		GLint    i, j;
		GLint    PixelDataLength;

		// 计算像素数据的实际长度
		i = scrWidth * 3;   // 得到每一行的像素数据长度
		while (i % 4 != 0)      // 补充数据，直到i是的倍数
			++i;               // 本来还有更快的算法，
							   // 但这里仅追求直观，对速度没有太高要求
		PixelDataLength = i * scrHeight;

		// 分配内存和打开文件
		pPixelData = (GLubyte*)malloc(PixelDataLength);
		if (pPixelData == 0)
			exit(0);
		fopen_s(&pDummyFile, "dummy.bmp", "rb");
		if (pDummyFile == 0)
			exit(0);
		fopen_s(&pWritingFile, "grab.bmp", "wb");
		if (pWritingFile == 0)
			exit(0);

		// 读取像素
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glReadPixels(0, 0, scrWidth, scrHeight,
			GL_BGR, GL_UNSIGNED_BYTE, pPixelData);

		// 把dummy.bmp的文件头复制为新文件的文件头
		fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
		fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
		
		fseek(pWritingFile, 0x0012, SEEK_SET);
		i = scrWidth;
		j = scrHeight;
	
		fwrite(&i, sizeof(i), 1, pWritingFile);
		fwrite(&j, sizeof(j), 1, pWritingFile);

	
		// 写入像素数据
		fseek(pWritingFile, 0, SEEK_END);
		fwrite(pPixelData, PixelDataLength, 1, pWritingFile);
		
		// 释放内存和关闭文件
		fclose(pDummyFile);
		fclose(pWritingFile);
		free(pPixelData);
		cout << "successfully catch the new frame!" << endl;
	}
	
};

vector<Shader *> Toolkit::myShader;
Camera *Toolkit::myCamera=NULL;
GLFWwindow *Toolkit::window = NULL;
int Toolkit::scrWidth;
int Toolkit::scrHeight;
int Toolkit::modelShader;

#endif // !MYTOOLKIT_H
