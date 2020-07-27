#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <MESH.h>
#include <SHADER.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
	/*  Model Data */
	map<string,Texture>textures_loaded;
	//记录加载过的纹理，防止重复加载

	vector<Mesh> meshes; 
	//加载的网格单元
	string directory;
	//记录模型所在目录
	bool gammaCorrection;
	

	/*  Functions   */
	//默认不进行伽马校正
	Model(string const &path, glm::vec4* assignedColor=NULL,bool disableMap=false,bool gamma = false) : gammaCorrection(gamma)
	{
		this->disableMap = disableMap;
		this->assignedColor = assignedColor;
		loadModel(path); //启用loadModel加载指定路径处的模型。
	}

	// draws the model, and thus all its meshes
	void Draw(Shader shader,string TYPE="TRIANGLES")
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader,TYPE);
		
	}

private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	bool disableMap;
	glm::vec4* assignedColor;
	void loadModel(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		//用importer读入模型文件，片元三角化，翻转纹理，计算正切

		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		cout << "Success!" << endl;
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));
		//cout << directory << endl;
		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{

		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene)); //读网格
			
		}

		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);//递归访问子树
		}

	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;
		//要从网格中把数据读入我们定义的标准网格结构中

		// Walk through each of the mesh's vertices
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z); //读入顶点坐标
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);//读入顶点法线
			// texture coordinates
			if (!disableMap&&mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;	//读入顶点纹理坐标
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

	
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]); //读入索引坐标
		}

		if (!disableMap&&mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			if (!diffuseMaps.empty())textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			if (!specularMaps.empty())textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		

		aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D diffuse(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
		int success = aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		if (success == AI_SUCCESS) 
		{
			color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		}
		else 
		{
			std::cout << "[Error] Get mesh color" << std::endl;
		}
		return Mesh(vertices, indices, textures, color,assignedColor, disableMap);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		//从aiMaterial对象中读取具体的规范定义的textures
		vector<Texture> textures;
		
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			if (textures_loaded.count(str.C_Str()))skip = true;
			
			
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				
				texture.id = TextureFromFile(str.C_Str(), this->directory);
			
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.insert(make_pair(texture.path, texture));
			}
	

		}
		
		return textures;
	}
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + "/"+filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	cout << filename << endl;

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);

		cout << filename << endl;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return -1;
	}

	return textureID;
}
#endif