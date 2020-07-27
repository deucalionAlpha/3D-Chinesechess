#ifndef TEXTURE_H
#define TEXTURE_H

#include<iostream>
#include<glad/glad.h>
#include<SHADER.h>
#include<CAMERA.H>
#include<vector>
#include<string>
#include"stb_image.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<istream>
#include<map>
using namespace std;

class MyTexture
{
private:
	GLuint ID;
public:
	MyTexture(string s,int type=0)
	{
		glGenTextures(1, &ID);
	
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true); 
		unsigned char *data = stbi_load(s.c_str(), &width, &height, &nrChannels, 0);
	
		if (data)
		{
			if (type == 0)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
	GLuint getID()
	{
		return ID;
	}
};
class Material
{
private:
	static map<string, Material*>materialTable;
	static bool hasInit;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float alpha;
public:
	static void initMaterial()
	{
		hasInit = true;
		materialTable.clear();
		Material *temp;
		glm::vec3 am, di, sp;
		string name;
		ifstream fin("./resource/material.txt");
		float shiness;
		for (int i = 0; i < 24; i++)
		{
			fin >> name;
			fin >> am.x >> am.y >> am.z;
			fin >> di.x >> di.y >> di.z;
			fin >> sp.x >> sp.y >> sp.z;
			fin >> shiness;
			temp = new Material(am, di, sp, shiness, 1.0);
			materialTable.insert(pair<string, Material*>(name,temp));
			cout << "Load material "<<i<<" : " << name <<" "<<am.x<<" "<<am.y<<" "<<am.z<<" "<<di.x<<" "<<di.y<<" "<<di.z<<" "<<sp.x<<" "<<sp.y<<" "<<sp.z<<endl;
		}
		fin.close();
	}
	static Material *getMaterialByName(string name)
	{
		if (materialTable.find(name) != materialTable.end())return materialTable[name];
		else return NULL;
	}
	Material(glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular,float shininess,float alpha)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
		this->alpha = alpha;
	}
	void setMaterial(Shader *myShader)
	{
		myShader->use();
		myShader->setBool("material.texture_diffuse0_exist", false);
		myShader->setBool("material.texture_specular0_exist", false);
		myShader->setFloat4("material.texture_diffuse_color", diffuse.x, diffuse.y, diffuse.z, alpha);
		myShader->setFloat4("material.texture_specular_color", specular.x, specular.y, specular.z, 1.0);
		myShader->setFloat4("material.texture_ambient_color", ambient.x, ambient.y, ambient.z, 1.0);
		myShader->setFloat1("material.shininess", shininess);
	}
};
class Skybox
{
private:
	vector<string>faces;
	Shader *skyBoxShader;
	unsigned int  skyBoxVAO;
	unsigned int skyBoxVBO;
	unsigned int id;
	static float skyBoxVertices[];
	glm::mat4 view;
	glm::mat4 projection;
public:
	Skybox(vector<string>faces)
	{
		this->faces = faces;
		skyBoxShader = new Shader("./shader/skyBoxVertexShaderSource.sd", "./shader/skyBoxFragmentShaderSource.sd");
		loadCubeMap();
		glGenVertexArrays(1, &skyBoxVAO);
		glGenBuffers(1, &skyBoxVBO);
		
		glBindVertexArray(skyBoxVAO); 
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
		glBufferData(GL_ARRAY_BUFFER,108*sizeof(float), skyBoxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		return;
	}
	void loadCubeMap()
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		int width, height, nrComponents;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

			cout << "load fixture at : " << faces[i].c_str() << endl;
			cout << "channel: " << nrComponents << endl;
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				cout << "Cubemap texture failed to load at path : " << faces[i] << endl;
			}
			stbi_image_free(data);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
	unsigned int getId()
	{
		return id;
	}
	void display(Camera x)
	{
		glDepthMask(GL_FALSE);
		this->skyBoxShader->use();
		
		
		view = glm::mat4(glm::mat3(x.getViewMatrix()));
		this->skyBoxShader->SetMat4("view",glm::value_ptr(view));
		projection = x.getProjectionMatrix();
		this->skyBoxShader->SetMat4("projection", glm::value_ptr(projection));

		glBindVertexArray(skyBoxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}
	~Skybox()
	{
		delete skyBoxShader;
		return;
	}
};
map<string, Material*>Material::materialTable;
bool Material::hasInit=false;
float Skybox::skyBoxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};
#endif