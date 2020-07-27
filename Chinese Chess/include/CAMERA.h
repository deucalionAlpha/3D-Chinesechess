#ifndef CAMERA_H
#define CAMERA_H
#include<glad/glad.h>
#include<iostream>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"SHADER.h"
#include<cmath>

using namespace std;
class Camera
{
public:
	float sensitivity;
	float yaw, pitch, fov;
	glm::vec3 cameraPos;
	//接受来自chessgo.h信息
	glm::vec2 pos[9][10];
	int type[33];
	int locX[33];
	int locY[33];
	float sizeoftype[15][6];
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::mat4 projection;
	glm::mat4 view;
	float NearDis;
	float FarDis;
	float minX, minY, minZ, maxX, maxY, maxZ;
	float verticalSpeed;
	bool freeMode;
	
private:
	unsigned int width, height;
public:
	Camera(const unsigned int &width , const unsigned int &height)
	{
		yaw = -90.0f; pitch = 0; fov = 45.0f;
		cameraPos = glm::vec3(0.0f, 4.0f, 15.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		NearDis = 0.1f;
		FarDis = 10000.f;
		projection = glm::perspective(glm::radians(45.0f), (float)width/ (float)height, NearDis, FarDis);
		this->width = width;
		this->height = height;
		sensitivity = 0.05;
		freeMode = true;
	}
	void updateMatrix(Shader * myShader,int type=0)
	{
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		myShader->SetMat4("view", glm::value_ptr(view));
		projection = glm::perspective(glm::radians(fov), (float)width / (float)height, NearDis, FarDis);
		myShader->SetMat4("projection", glm::value_ptr(projection));
		if (type == 1)
		{
			myShader->setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		}
	}
	
	float getSensitivity()
	{
		return sensitivity;
	}
	void setSensitivity(float x)
	{
		sensitivity = x;
	}
	glm::mat4 getViewMatrix()
	{
		return view;
	}
	glm::mat4 getProjectionMatrix()
	{
		return projection;
	}
	void setSpace(float cX, float cZ, float h, float wd, float lt, float maxH)
	{
		minX = cX - 0.5f*lt;
		maxX = cX + 0.5f*lt;
		minZ = cZ - 0.5f*wd;
		maxZ = cZ + 0.5f*wd;
		minY = h+2.0f;
		maxY =140.0f;
	}
	void jump()
	{
		if (fabs(cameraPos.y - minY) <= 0.001)verticalSpeed = 10.0;
	}
	void checkJump()
	{
		
		if (freeMode) { verticalSpeed = 0; return; }
		static glm::vec3 temp;
		if (fabs(cameraPos.y - minY) <= 0.001&&verticalSpeed<0)verticalSpeed = 0;
		else
		{
			temp.x = cameraPos.x;
			temp.z = cameraPos.z;
			temp.y = cameraPos.y + verticalSpeed * (deltatime);
			for (int i = 0; i < 33; i++)
			{
				//判断各自棋子类型的大小 sizeoftype[][]存储了该类棋子的大小
				if (type[i] >= 0)//判断是否该棋子还在棋盘中
				{
					if (pos[locX[i]][locY[i]].x + sizeoftype[type[i]][0] < temp.x	&&	 temp.x < pos[locX[i]][locY[i]].x + sizeoftype[type[i]][1])
					{
						if (pos[locX[i]][locY[i]].y + sizeoftype[type[i]][2] < temp.z	&&	temp.z < pos[locX[i]][locY[i]].y + sizeoftype[type[i]][3])
						{
							{
								if (sizeoftype[type[i]][4] < temp.y&&temp.y < sizeoftype[type[i]][5])
								{
									return;
								}
							}
						}
					}
				}

			}
			verticalSpeed -= gravity * (deltatime);
			if (temp.y < minY)
			{
				temp.y = minY; verticalSpeed = 0;
			}
			cameraPos.y = temp.y;
		}
		
	}
	void updatePos(glm::vec3 &temp)
	{
		int flag = 0;
		for (int i = 0; i < 33; i++)
		{
			//判断各自棋子类型的大小 sizeoftype[][]存储了该类棋子的大小
			if (type[i] >= 0)//判断是否该棋子还在棋盘中
			{
				if (pos[locX[i]][locY[i]].x + sizeoftype[type[i]][0] < temp.x	&&	 temp.x < pos[locX[i]][locY[i]].x + sizeoftype[type[i]][1])
				{
					if (pos[locX[i]][locY[i]].y + sizeoftype[type[i]][2] < temp.z	&&	temp.z < pos[locX[i]][locY[i]].y + sizeoftype[type[i]][3])
					{
						{
							if (sizeoftype[type[i]][4] < temp.y&&temp.y < sizeoftype[type[i]][5])
							{
								//cout << temp.y << "\n";
								return;
							}
						}
					}
				}
			}

		}
		
		if (temp.x <= maxX && temp.x >= minX)cameraPos.x = temp.x;
		else if (temp.x < minX)cameraPos.x = minX;
		else if (temp.x > maxX)cameraPos.x = maxX;

		if (temp.y <= maxY && temp.y >= minY)cameraPos.y = temp.y;
		else if (temp.y < minY)cameraPos.y = minY;
		else if (temp.y > maxY)cameraPos.y = maxY;
		
		if (temp.z <= maxZ && temp.z >= minZ)cameraPos.z = temp.z;
		else if (temp.z < minZ)cameraPos.z = minZ;
		else if (temp.z > maxZ) cameraPos.z = maxZ;
		
		return;


	}
	void changeMode()
	{
		freeMode = !freeMode;
	}

	static Camera *currentCamera;
	static GLFWwindow *currentWindow;
	static float deltatime;
	static float lastFrame;
	static float gravity;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		static bool firstMouse=true;
		static double lastX = 0;
		static double lastY = 0;
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			return;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		xoffset *= currentCamera->sensitivity;
		yoffset *= currentCamera->sensitivity;

		currentCamera->yaw += xoffset;
		currentCamera->pitch += yoffset;

		if (currentCamera->pitch > 89.0f)currentCamera->pitch = 89.0f;
		if (currentCamera->pitch < -89.0f)currentCamera->pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(currentCamera->yaw)) * cos(glm::radians(currentCamera->pitch));
		front.y = sin(glm::radians(currentCamera->pitch));
		front.z = sin(glm::radians(currentCamera->yaw)) * cos(glm::radians(currentCamera->pitch));
		currentCamera->cameraFront = glm::normalize(front);
	}
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (currentCamera->fov >= 1.0f &&currentCamera->fov <= 45.0f)
			currentCamera->fov -= yoffset;
		if (currentCamera->fov <= 1.0f)currentCamera->fov = 1.0f;
		if (currentCamera->fov >= 45.0f)currentCamera->fov = 45.0f;
	}
	static void registerCamera(GLFWwindow *window,Camera *targetCamera)
	{
		bool firstRegister = true;
		if (firstRegister)
		{
			currentCamera = targetCamera;
			currentWindow = window;
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
			firstRegister = false;
		}
		else
		{
			currentCamera = targetCamera;
			currentWindow = window;
		}
	}

	static void processInput()
	{
		if (currentCamera == NULL)
		{
			cout << "The camera doesn't exist!" << endl;
			return;
		}

		if (glfwGetKey(currentWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(currentWindow, true);

		static float cameraSpeed;
		static glm::vec3 temp;
		static bool fDown = false;
		static bool spaceDown = false;


		if (glfwGetKey(currentWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)cameraSpeed = 15*deltatime;
		else cameraSpeed = 7.5*deltatime;

		
		
		if (glfwGetKey(currentWindow, GLFW_KEY_W) == GLFW_PRESS)
		{
			temp = currentCamera->cameraPos + cameraSpeed * currentCamera->cameraFront;
			currentCamera->updatePos(temp);
		}
			
		if (glfwGetKey(currentWindow, GLFW_KEY_S) == GLFW_PRESS)
		{
			temp = currentCamera->cameraPos - cameraSpeed * currentCamera->cameraFront;
			currentCamera->updatePos(temp);
		}
		if (glfwGetKey(currentWindow, GLFW_KEY_A) == GLFW_PRESS)
		{
			temp = currentCamera->cameraPos - cameraSpeed * glm::normalize(glm::cross(currentCamera->cameraFront, currentCamera->cameraUp));
			currentCamera->updatePos(temp);
		}
		if (glfwGetKey(currentWindow, GLFW_KEY_D) == GLFW_PRESS)
		{
			temp=currentCamera->cameraPos + cameraSpeed * glm::normalize(glm::cross(currentCamera->cameraFront, currentCamera->cameraUp));
			currentCamera->updatePos(temp);
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (glfwGetKey(currentWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (currentCamera->freeMode)
			{
				temp = currentCamera->cameraPos + cameraSpeed * currentCamera->cameraUp;
				currentCamera->updatePos(temp);
			}
			else
			{
				if (spaceDown == false)
				{
					currentCamera->jump();
					spaceDown = true;
				}
			}
		}
		if (glfwGetKey(currentWindow, GLFW_KEY_SPACE) == GLFW_RELEASE)spaceDown = false;
		if (glfwGetKey(currentWindow, GLFW_KEY_C) == GLFW_PRESS)
		{
			temp=currentCamera->cameraPos - cameraSpeed * currentCamera->cameraUp;
			currentCamera->updatePos(temp);
		}

		if (glfwGetKey(currentWindow, GLFW_KEY_F) == GLFW_PRESS)
		{
			if (fDown == false)
			{
				currentCamera->changeMode();

				fDown = true;
			}
		}
		if (glfwGetKey(currentWindow, GLFW_KEY_F) == GLFW_RELEASE)fDown = false;
		currentCamera->checkJump();
		
	}
};
Camera *Camera::currentCamera;
GLFWwindow *Camera::currentWindow;
float Camera::deltatime=0;
float Camera::lastFrame=0;
float Camera::gravity=9.8;
#endif