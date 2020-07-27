#ifndef WATERFLOW_H
#define WATERFLOW_H
#include<MYTOOLKIT.h>
#include<ctime>
#include"LIGHT.h"
class waterFlow
{
private:
	static Light *myLight;
	int maxI, maxJ;
	float D;
	unsigned int VAO, VBO, EBO;
	//float * buffer;
	unsigned int index[6];
	Shader *myShader;
	glm::mat4 waterMatrix;
	float waveSeita;
	float buffer[12];
	static float PI;
	
	void setWave(int i, float steepness, float amplitude, float frequency, float speed, float directionX,float directionY)
	{
		static string begin = "gGerstnerWave[i";
		begin.replace(14, 1, 1, i + '0');
		myShader->use();
		myShader->setFloat1(begin + "].Steepness", steepness);
		myShader->setFloat1(begin + "].Amplitude", amplitude);
		myShader->setFloat1(begin + "].Frequency", frequency);
		myShader->setFloat1(begin + "].Speed", speed);
		myShader->setFloat2(begin +"].Direction", directionX, directionY);
	}
	void initialWave()
	{
		srand((unsigned)time(0));
		myShader->use();
		myShader->SetMat4("model", glm::value_ptr(waterMatrix));
		myShader->setInt1("gGerstnerWaveCount", 4);
		setWave(0, 0.1, 0.3, 0.4, 2.0, 0.2,-1 );
		setWave(1, 0.2, 0.2, 0.6, 1.5, 0.7, -1);
		setWave(2, 0.2, 0.4, 0.7, 2.5, -1,0.4);

		waveSeita = 0;
		setWave(3, 0.2, 0.3, 1.0, 1.5, 1.0*cosf(waveSeita/PI), 1.0*sinf(waveSeita / PI));

		myShader->setFloat1("material.shininess",64.0);
		myShader->setBool("light.pointlight", false);
		myShader->setFloat3("light.direction", 1.0, -1.0, 0.0);

		myShader->setFloat3("light.ambient", 0.5f, 0.5f, 0.5f);
		myShader->setFloat3("light.diffuse", 0.0f,0.63f,0.914f);
		myShader->setFloat3("light.specular", 1.0f, 1.0f, 1.0f);
		
		myShader->setBool("material.texture_diffuse0_exist", false);
		myShader->setBool("material.texture_specular0_exist", false);
		myShader->setFloat4("material.texture_specular_color", 1.0f, 1.0f, 1.0f, 1.0f);
		myShader->setFloat4("material.texture_diffuse_color", 0.53f,0.808f,0.98f,1.0f);

		myShader->setFloat1("D", this->D);
		myShader->setInt1("maxI", this->maxI);
		myShader->setBool("instanced", true);
	}
	void waveRot()
	{
		waveSeita += 0.005f;
		if (waveSeita >= 360.0f)waveSeita = 0.0f;
		setWave(3, 0.2, 0.3, 1.0, 1.5, 1.0*cosf(waveSeita / PI), 1.0*sinf(waveSeita / PI));
	}
public:
	waterFlow(int maxI, int maxJ, float D, float cX, float cZ,float height=1.5)
	{
		this->maxI = maxI;
		this->maxJ = maxJ;
		this->D = D;
		float x = cX - D * (maxI*0.5) , z = cZ + D * (maxJ*0.5);
		cout << x << " " << z << endl;
		float temp[] = { x,height,z,x + D,height,z,x,height,z - D,x + D,height,z - D };
		for (int i = 0; i < 12; i++)buffer[i] = temp[i];
		index[0] = 0; index[1] = 1; index[2] = 2;
		index[3] = 2; index[4] = 1; index[5] = 3;
		myShader= new Shader("./shader/waterVertexShader.sd", "./shader/waterFragmentShader.sd");
		myLight->setLight(myShader);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), buffer, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), index, GL_STATIC_DRAW);

		glBindVertexArray(0);
		initialWave();
	}
	
	void draw()
	{
		static int k = maxI * maxJ;

		waveRot();
		Toolkit::myCamera->updateMatrix(myShader);
		myShader->setFloat1("gTime", glfwGetTime());
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, this->maxI*maxJ);
		glBindVertexArray(0);
	}
	static void setLight(Light *light)
	{
		myLight = light;
	}
	/*static Shader *getShader()
	{
		return myShader;
	}
	static void initwaterShader()
	{
		myShader = new Shader("./shader/waterVertexShader.sd", "./shader/waterFragmentShader.sd");
	}*/
};
float waterFlow::PI=3.1415926;
Light *waterFlow::myLight; 

#endif