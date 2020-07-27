#ifndef CHESSMANAGER_H
#define CHESSMANAGER_H
#include"CHESSGO.h"
#include "WATERFLOW.h"
class boardCross
{
private:
	Square *cross[2];
	glm::mat4 *modelMatrix;
	Material *crossMaterial;
public:
	boardCross(float cx, float cz, float gap)
	{
		cross[0] = new Square(0, 0, 2.2, 0.5, gap * 2 * sqrt(2.0));
		cross[1] = new Square(0, 0, 2.2, gap * 2 * sqrt(2.0), 0.5);

		modelMatrix = new glm::mat4;
		*modelMatrix = glm::translate(*modelMatrix, glm::vec3(cx, 0, cz));
		*modelMatrix = glm::rotate(*modelMatrix, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
		//*modelMatrix = glm::translate(*modelMatrix, glm::vec3(cx, 0, cz));

		cross[0]->setModelMatrix(modelMatrix);
		cross[1]->setModelMatrix(modelMatrix);
		crossMaterial = Material::getMaterialByName("emerald");
	}
	void drawCross()
	{
		cross[0]->drawGround(crossMaterial);
		cross[1]->drawGround(crossMaterial);
	}
};

class Tag
{
private:
	Square *tag[8];
	static int offsetX[4];
	static int offsetZ[4];
	glm::mat4 * model[8];
	Material *tagMaterial;
public:
	Tag(float cx, float cz, float gap, bool split = false,float height=2.2,string materialName="brass")
	{
		float offsetL, offsetH;
		float len = gap / 6.0;
		float thin = gap / 15.0;
		offsetL = gap / 20.0;
		offsetH = gap / 12.0 + offsetL;

		for (int i = 0; i < 4; i++)
		{
			tag[i] = new Square(0, 0,height , thin, len);
			model[i] = new glm::mat4;
			*model[i] = glm::translate(*model[i], glm::vec3(offsetH*offsetX[i] + cx, 0, offsetL*offsetZ[i] + cz));
			//tag[i]->setModelMatrix(model[i]);
		}
		for (int i = 4; i < 8; i++)
		{
			tag[i] = new Square(0, 0, height, len, thin);
			model[i] = new glm::mat4;
			*model[i] = glm::translate(*model[i], glm::vec3(offsetL*offsetX[i - 4] + cx, 0, offsetH*offsetZ[i - 4] + cz));
			//tag[i]->setModelMatrix(model[i]);
		}
		if (split == true)
		{
			for (int i = 0; i < 2; i++)
			{
				*model[i] = glm::translate(*model[i], glm::vec3(0, 0, -8 * gap));
				*model[i + 4] = glm::translate(*model[i + 4], glm::vec3(0, 0, -8 * gap));
			}
		}
		for (int i = 0; i < 8; i++)tag[i]->setModelMatrix(model[i]);
		tagMaterial = Material::getMaterialByName(materialName);
	}
	void drawTag()
	{
		for (int i = 0; i < 8; i++)tag[i]->drawGround(tagMaterial);
	}
	void translateTag(glm::vec3&trans)
	{
		for(int i=0;i<8;i++)
			*model[i]= glm::translate(*model[i], trans);
	}
};
class Focus
{
private:

	glm::vec3 trans;
	int row, col;
	float gap;
	Tag *myFocus;
	Square *chosen;
	chessGo *myChess;
	bool focusComfirmed;
	int locX, locY;
	int targetId;
	int turn;
	Material * focusMaterial;
public:
	Focus(int row, int col, float gap,chessGo* myChess)
	{
		this->row = row; this->col = col;
		this->gap = gap; 
		this->myChess = myChess;

		trans.x = (col - 4.5)*gap;
		trans.y = 0;
		trans.z = (row - 4.0)*gap;
		myFocus = new Tag(0, 0, 2 * gap,false,2.4, "copper");
		myFocus->translateTag(trans);
		focusComfirmed = false;
		turn = 0;
		focusMaterial = Material::getMaterialByName("jade");
	}
	void drawFocus()
	{
		myFocus->drawTag();
		if (focusComfirmed )chosen->drawGround(focusMaterial);
	}
	void moveFocus(int dRow, int dCol)
	{
		static int trow, tcol;
		trow = row + dRow;
		tcol = col + dCol;
		if (trow < 0 || trow>8)return;
		if (tcol < 0 || tcol>9)return;
		row = trow;
		col = tcol;
		trans.x = dCol * gap;
		trans.z = dRow * gap;
		myFocus->translateTag(trans);
	}
	bool targetConfirm()
	{
		if (myChess->focusLock ||!focusComfirmed)return false;

		focusComfirmed = false;
		delete chosen;
		if (myChess->moveChess(targetId, row, col) == true) { turn = !turn; return true; }
		else
		{
			trans.x = (locY - col)*gap;
			trans.z = (locX - row)*gap;
			myFocus->translateTag(trans);
			row = locX;
			col = locY;
			return false;
		}
	}
	bool focusConfirm()
	{
		static int k;
		if (myChess->focusLock ||focusComfirmed)return false;

		if (myChess->target(row, col) == 0)return false;
		targetId = myChess->target(row, col);
		k = myChess->getType(targetId);
		if ((!turn&&k > 7)||(turn&&k<8))return false;
		
		focusComfirmed = true;
		chosen = new Square((col - 4.5)*gap, (row - 4)*gap, 2.3, gap, gap);
		locX = row; locY = col;
		return true;
	}
};


class chessManager
{
private:
	static glm::vec3 upDir;
	GLFWwindow* window;
	int frontDeltaRow, frontDeltaCol;
	int rightDeltaRow, rightDeltaCol;
	chessGo *myChess;
	float gap;
	Focus *myFocus;
	waterFlow *myWaterFlow[2];
	void updateDirSystem()
	{
		static float x, z;
		x = Toolkit::myCamera->cameraFront.x;
		z = Toolkit::myCamera->cameraFront.z;
		if (fabs(x) > fabs(z))
		{
			if (x > 0) { frontDeltaRow = 0; frontDeltaCol = 1; }
			else { frontDeltaRow = 0; frontDeltaCol = -1; }
		}
		else
		{
			if (z > 0) { frontDeltaRow = 1; frontDeltaCol = 0; }
			else { frontDeltaRow = -1; frontDeltaCol = 0; }
		}
		rightDeltaRow = frontDeltaCol;
		rightDeltaCol = -frontDeltaRow;
	}
public:
	chessManager(float gap)
	{
		this->gap = gap;
		myChess = new chessGo(gap);

		updateDirSystem();
		myFocus = new Focus(4, 3, gap,myChess);
		window = Camera::currentWindow;
		//cout <<"gap:"<< gap << endl;
		myWaterFlow[0] = new waterFlow(45, 500, gap / 50.0 , 0, 0);
		myWaterFlow[1] = new waterFlow(200, 200, gap / 10.0, 0, 0,0.0f);
	}
	void processInput()
	{
		static bool leftPressed=false, rightPressed=false, upPressed=false, downPressed=false;
		static bool enterPressed = false;

		updateDirSystem();
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			if (leftPressed == false)
			{
				myFocus->moveFocus(-rightDeltaRow, -rightDeltaCol);
				leftPressed = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) ==GLFW_RELEASE)leftPressed = false;

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if (rightPressed == false)
			{
				myFocus->moveFocus(rightDeltaRow, rightDeltaCol);
				rightPressed = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)rightPressed = false;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			if (upPressed == false)
			{
				myFocus->moveFocus(frontDeltaRow, frontDeltaCol);
				upPressed = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)upPressed = false;

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			if (downPressed == false)
			{
				myFocus->moveFocus(-frontDeltaRow, -frontDeltaCol);
				downPressed = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)downPressed = false;

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			if (enterPressed == false)
			{
				if (myFocus->focusConfirm() == false)
				{
					myFocus->targetConfirm();
				}
				enterPressed = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)enterPressed = false;
	}
	void display()
	{
		processInput();
		myWaterFlow[0]->draw();
		myWaterFlow[1]->draw();
		myFocus->drawFocus();
		myChess->drawChess();
	}
};


int Tag::offsetX[4] = { -1,1,-1,1 };
int Tag::offsetZ[4] = { 1,1,-1,-1 };

glm::vec3 chessManager::upDir(0.0f,1.0f,0.0f);
#endif // !