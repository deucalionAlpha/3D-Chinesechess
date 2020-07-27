#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	bool enablegeometry;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath,const char * geometryPath=NULL)
	{
		enablegeometry = false;
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;


		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);



		if (geometryPath != NULL)
		{
			std::string geometryCode;
			std::ifstream geometryFile;
			geometryFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				// open files
				geometryFile.open(geometryPath);
				std::stringstream geometryStream;
				// read file's buffer contents into streams
				geometryStream << geometryFile.rdbuf();
				// close file handlers
				geometryFile.close();
				// convert stream into string
				geometryCode = geometryStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}

			unsigned int geometry;
			const char* GShaderCode = geometryCode.c_str();

			geometry= glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &GShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
			glAttachShader(ID, geometry);
			enablegeometry = true;
		}

		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt1(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt2(const std::string &name, int v1,int v2) const
	{
		glUniform2i(glGetUniformLocation(ID, name.c_str()), v1,v2);
	}
	void setInt3(const std::string &name, int v1, int v2, int v3) const
	{
		glUniform3i(glGetUniformLocation(ID, name.c_str()), v1,v2,v3);
	}
	void setInt4(const std::string &name, int v1 , int v2, int v3,int v4) const
	{
		glUniform4i(glGetUniformLocation(ID, name.c_str()), v1,v2,v3,v4);
	}
	// ------------------------------------------------------------------------
	void setFloat1(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat2(const std::string &name, float v1, float v2) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), v1,v2);
	}
	void setFloat3(const std::string &name, float v1, float v2 , float v3) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), v1,v2,v3);
	}
	void setFloat4(const std::string &name, float v1, float v2, float v3, float v4) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), v1,v2,v3,v4);
	}
	void SetMat4(const std::string &name, float *mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif