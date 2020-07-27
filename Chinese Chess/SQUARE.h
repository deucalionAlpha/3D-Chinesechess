#ifndef  SQUARE_H
#define	SQUARE_H
#include<MYTOOLKIT.h>
class Square
{
private:
	float height;
	float centerX, centerZ;
	float width, length;
	glm::mat4 *groundMatrix;
	unsigned int VAO;
	unsigned int VBO;
	float gap;
	static Shader * groundShader;
public:
	Square(float cX, float cZ, float h, float wd, float lt)
	{
		centerX = cX;	centerZ = cZ;
		height = h;	width = wd; length = lt;
		lt *= 0.5; wd *= 0.5;
		groundMatrix = new glm::mat4;
		float vertices[]=
		{
			centerX - lt,height,centerZ + wd,0.0f,1.0f,0.0f,
			centerX+lt,height,centerZ+wd,0.0f,1.0f,0.0f,
			centerX-lt,height,centerZ - wd,0.0f,1.0f,0.0f,

			centerX - lt,height,centerZ - wd,0.0f,1.0f,0.0f,
			centerX + lt,height,centerZ + wd,0.0f,1.0f,0.0f,
			centerX + lt,height,centerZ - wd,0.0f,1.0f,0.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 36* sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void drawGround(Material *target)
	{
		groundShader->use();
		Toolkit::myCamera->updateMatrix(groundShader, 1);
		if (target == NULL)
		{
			cout << "failed when loading materail" << endl;
		}
		else target->setMaterial(groundShader);

		groundShader->SetMat4("model", glm::value_ptr(*groundMatrix));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void setModelMatrix(glm::mat4 * model)
	{
		if(model!=NULL)groundMatrix = model;
		return;
	}
	static Shader* getShader()
	{
		return groundShader;
	}
	static void initChessBoardShader()
	{
		Square::groundShader = new Shader("./shader/chessBoardVertexShader.sd",
			"./shader/chessBoardFragmentShader.sd");
	}
};
Shader * Square::groundShader;
#endif // ! GROUND_H

