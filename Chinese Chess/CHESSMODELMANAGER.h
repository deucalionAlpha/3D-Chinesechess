#ifndef  CHESSMODELMANAGER_H
#define CHESSMODELMANAGER_H
#include<MYTOOLKIT.h>
#include"NURBSCAR.h"
class chessModel
{
private:
	float rotAngle;
	float scaleFactor;
	Model *chess;
	nurbsCar *nurbs;
	glm::mat4 model;
	bool Red;
	bool nurbsModel;
public:
	chessModel(Model *chess, float scaleFactor, float rotAngle,bool Red,glm::vec3 *initshift)
	{
		this->chess = chess;
		this->scaleFactor = scaleFactor;
		this->rotAngle = rotAngle;
		this->Red = Red;
		model =	glm::scale(model,glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		translate(initshift);
		nurbsModel = false;
	}
	chessModel(nurbsCar *chess, float scaleFactor, float rotAngle, bool Red, glm::vec3 *initshift)
	{
		this->nurbs = chess;
		this->scaleFactor = scaleFactor;
		this->rotAngle = rotAngle;
		this->Red = Red;
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		translate(initshift);
		nurbsModel = true;
	}
	void translate(glm::vec3 *trans)
	{
		if (rotAngle != 0)
		{
			model = glm::rotate(model, glm::radians(-rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, (*trans) / scaleFactor);
			model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else model = glm::translate(model, (*trans) / scaleFactor);
	}
	void draw(Shader *myShader)
	{
		myShader->use();
		myShader->SetMat4("model", glm::value_ptr(model));
		if (nurbsModel)nurbs->draw(myShader);
		else chess->Draw(*myShader);
	}
};
#endif // ! CHSESSMODELMANAGER_H
