#include<MYTOOLKIT.h>
#include<ctime>
#include"SKY.h"
#include"TEXT.h"
#include"CHESSBOARD.h"
#include"LIGHT.h"


const unsigned int SCR_WIDTH=800;
const unsigned int SCR_HEIGHT = 600;
glm::vec3 lightDir(1.0, 1.0f, 3.0f);

const float centerX = 0.0f;
const float centerZ = 0.0f;
const float heightGround = 2.0f;
const float lengthGround = 200.0f;
const float widthGround = 200.0f;
const float maxHeight = 400.0f;
//Ground groundObj(0.0f, 0.0f, 2.0f, 100.0f, 100.0f, Toolkit::myShader[modelShader]);
int main()
{
	srand(time(0));
	if (!Toolkit::InitOpenGL("Model", SCR_WIDTH, SCR_HEIGHT))
	{
		cout << "Failed to initialize OpenGL" << endl;
		return 0;
	}

	Material::initMaterial();
	Light myLight
	(glm::vec3(0, 100.0, 0.0), glm::vec3(0.4,0.4,0.4), glm::vec3(1.0,1.0,1.0),
		glm::vec3(1.0, 1.0, 1.0), true);

	Sky skyObj("./resource/skyobj/skybox.obj");
	unsigned int modelShader = Toolkit::createShader("./shader/objectVertexShader.sd", 
		"./shader/objectFragmentShader.sd",
			"./shader/objectGeometryShader.sd");
	Toolkit::modelShader = modelShader;

	waterFlow::setLight(&myLight);
	Square::initChessBoardShader();

	myLight.setLight(Toolkit::myShader[modelShader]);
	myLight.setLight(Square::getShader());

	unsigned int textShader = Toolkit::createShader("./shader/textVertexShader.sd", "./shader/textFragmentShader.sd");
	Text calibri("./resource/fonts/calibri.ttf");
	glm::mat4 projection = glm::ortho(0.0f, SCR_WIDTH*1.0f, 0.0f, SCR_HEIGHT*1.0f);
	Toolkit::myShader[textShader]->use();
	Toolkit::myShader[textShader]->SetMat4("projection", glm::value_ptr(projection));

	Toolkit::myCamera->setSpace(centerX, centerZ, heightGround, lengthGround, widthGround,maxHeight);

	chessBoard myBoard(centerX, centerZ, heightGround, lengthGround, widthGround);
	
	
	double currentFrame;
	glm::mat4 lightmat;
	lightmat = glm::rotate(lightmat, glm::radians(0.5f), glm::vec3(0.0, 1.0, 0.0));
	

	int cnt = 0; double totTime=0;
	int maxcnt = 800;

	char t[20];

	Toolkit::myShader[modelShader]->use();
	Toolkit::myShader[modelShader]->setBool("toExplode", false);

	while (!glfwWindowShouldClose(Toolkit::window))
	{
		currentFrame = glfwGetTime();
		Camera::deltatime = currentFrame - Camera::lastFrame;
		Camera::lastFrame = currentFrame;

		Camera::processInput();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skyObj.drawSky();
		myLight.draw();

		
		myBoard.drawChessBoard();
	

		cnt++; totTime += Camera::deltatime;
		if (cnt == 60)
		{
			cnt = 0;
			sprintf_s(t, "FPS : %.2lf", 60.0 / totTime);
			
			totTime = 0;
		}
		calibri.RenderText(*(Toolkit::myShader[textShader]), string(t), 25.0f, 25.0f, 1.0f, glm::vec3(1.0, 0.0, 0.0));
		glfwSwapBuffers(Toolkit::window);
		glfwPollEvents();
		Toolkit::checkScrShotRequest();
	}
	glfwTerminate();
	return 0;
}