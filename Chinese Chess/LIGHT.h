#ifndef LIGHT_H
#define LIGHT_H
#include<MYTOOLKIT.h>
class Light
{
private:
	bool	pointLight;
	glm::vec3 pos,ambient,diffuse,specular;
	Shader *lightShader;
	unsigned int VAO, VBO;
	glm::mat4 * model;
	void buildModel()
	{
		float vertices[] = {
			// positions          
			-0.5f, -0.5f, -0.5f, 
			 0.5f, -0.5f, -0.5f,  
			 0.5f,  0.5f, -0.5f, 
			 0.5f,  0.5f, -0.5f,  
			-0.5f,  0.5f, -0.5f,  
			-0.5f, -0.5f, -0.5f, 

			-0.5f, -0.5f,  0.5f,  
			 0.5f, -0.5f,  0.5f, 
			 0.5f,  0.5f,  0.5f,  
			 0.5f,  0.5f,  0.5f,  
			-0.5f,  0.5f,  0.5f,  
			-0.5f, -0.5f,  0.5f, 

			-0.5f,  0.5f,  0.5f, 
			-0.5f,  0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f, 
			-0.5f, -0.5f, -0.5f, 
			-0.5f, -0.5f,  0.5f, 
			-0.5f,  0.5f,  0.5f, 

			 0.5f,  0.5f,  0.5f,  
			 0.5f,  0.5f, -0.5f, 
			 0.5f, -0.5f, -0.5f, 
			 0.5f, -0.5f, -0.5f, 
			 0.5f, -0.5f,  0.5f,  
			 0.5f,  0.5f,  0.5f, 

			-0.5f, -0.5f, -0.5f,  
			 0.5f, -0.5f, -0.5f,  
			 0.5f, -0.5f,  0.5f, 
			 0.5f, -0.5f,  0.5f,  
			-0.5f, -0.5f,  0.5f,  
			-0.5f, -0.5f, -0.5f, 

			-0.5f,  0.5f, -0.5f,  
			 0.5f,  0.5f, -0.5f,  
			 0.5f,  0.5f,  0.5f, 
			 0.5f,  0.5f,  0.5f,  
			-0.5f,  0.5f,  0.5f, 
			-0.5f,  0.5f, -0.5f
		};
		model = new glm::mat4;
		*model = glm::translate(*model, pos);
		*model = glm::scale(*model, glm::vec3(5.0f, 5.0f, 5.0f));

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		lightShader->use();
		lightShader->SetMat4("model", glm::value_ptr(*model));
		lightShader->setFloat4("lightColor", diffuse.x, diffuse.y, diffuse.z, 1.0);
	}
public:
	Light(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, bool pointLight)
	{
		this->pointLight = pointLight;
		this->pos = pos;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		if (pointLight)
		{
			lightShader = new Shader("./shader/lightVertexShader.sd", "./shader/lightFragmentShader.sd");
			buildModel();
		}
	}
	void setLight(Shader *myShader)
	{
		myShader->use();
		myShader->setBool("light.pointlight", pointLight);
		if (pointLight)myShader->setFloat3("light.position", pos.x,pos.y,pos.z);
		else myShader->setFloat3("light.direction", pos.x, pos.y, pos.z);
		myShader->setFloat3("light.ambient", ambient.x, ambient.y, ambient.z);
		myShader->setFloat3("light.diffuse", diffuse.x, diffuse.y, diffuse.z);
		myShader->setFloat3("light.specular", specular.x, specular.y, specular.z);
		myShader->setFloat1("material.shininess", 32.0);
	}
	void draw()
	{
		if (!pointLight)return;
		lightShader->use();
		Toolkit::myCamera->updateMatrix(lightShader);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
	}
};
#endif // !LIGHT_H
