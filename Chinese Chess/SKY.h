#ifndef SKY_H
#define SKY_H
#include<MYTOOLKIT.h>
#include<MODEL.h>
#include<glm/gtc/type_ptr.hpp>
class Sky
{
private :
	Model *mySky;
	glm::mat4 *skyMatrix;
	Shader *skyShader;
public:
	Sky(string path)
	{
		mySky = new Model(path);
		skyMatrix = new glm::mat4;
		*skyMatrix = glm::translate(*skyMatrix, glm::vec3(0.0, -2.0, 0.0));
		*skyMatrix = glm::scale(*skyMatrix, glm::vec3(5.0, 5.0, 5.0));
		skyShader = new Shader("./shader/skyVertexShader.sd", "./shader/skyFragmentShader.sd");
		skyShader->use();
		
		
	}
	void drawSky()
	{
		skyShader->use();
		Toolkit::myCamera->updateMatrix(skyShader);
		skyShader->SetMat4("model", glm::value_ptr(*skyMatrix));
		mySky->Draw(*skyShader);
	}

};
#endif // !SKY_H