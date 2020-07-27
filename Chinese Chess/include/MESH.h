#ifndef MESH_H
#define MESH_H
#include<glad\glad.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include<SHADER.h>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include<vector>
using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex>vertices;
	vector<unsigned int>indices;
	vector<Texture>textures;
	glm::vec4 Color;
	unsigned int VAO;

	Mesh(vector<Vertex>vertices, vector<unsigned int>indices, vector<Texture>textures,glm::vec4 Color,glm::vec4* assignedColor=NULL,bool disableMap=false)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->Color = Color;
		this->disableMap = disableMap;
		this->assignedColor = assignedColor;
		setupMesh();
	}

	void Draw(Shader shader,string  TYPE)
	{
		unsigned int diffuseNr = 0;
		unsigned int specularNr = 0;

		shader.setBool("material.texture_diffuse0_exist", 0);
		shader.setBool("material.texture_specular0_exist", 0);
		shader.setFloat4("material.texture_diffuse_color", 0.7f, 0.7f, 0.7f, 1.0f);
		shader.setFloat4("material.texture_specular_color", 1.0f, 1.0f, 1.0f, 1.0f);

		if (!disableMap&&textures.size() > 0)
		{
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				if (textures[i].id == -1)continue;
				glActiveTexture(GL_TEXTURE0 + i);
				string Number;
				string Name = textures[i].type;
				if (Name == "texture_diffuse")Number = std::to_string(diffuseNr++);
				else if (Name == "texture_specular")Number = std::to_string(specularNr++);
				//cout << Name << endl;
				shader.setInt1("material." + (Name + Number), i);
				shader.setBool("material." + (Name + Number) + "_exist", 1);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}
		}
		else if (assignedColor)
		{
			shader.setFloat4("material.texture_diffuse_color", assignedColor->r, assignedColor->g, assignedColor->b, assignedColor->a);
		}
		else 
		{
			shader.setFloat4("material.texture_diffuse_color", Color.r,Color.g,Color.b,Color.a);
		}

		if (shader.enablegeometry == true)
		{
			shader.setFloat1("time", glfwGetTime());
		}

		glBindVertexArray(VAO);
		if(TYPE=="LINES")glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		else if (TYPE == "POINTS")glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
		else glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VBO, EBO;
	bool disableMap;
	glm::vec4 *assignedColor;
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};



#endif // !MESH_H
