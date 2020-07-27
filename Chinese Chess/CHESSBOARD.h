#ifndef  CHESSBOARD_H
#define CHESSBOARD_H
#include"SQUARE.h"
#include"CHESSMANAGER.h"
using namespace std;
class chessBoard
{
private:
	Square *Line[26];
	Material *groundMaterial;
	Material *lineMaterial;
	boardCross *cross[2];
	Tag *tag[12];
	chessManager *myManager;
	//chessGo *myChess;
	Square *groundObj[3];
	//Tag *focus;
	//glm::vec4 *focusColor;
public:
	chessBoard(float centerX, float centerZ,float heightGround, float lengthGround, float widthGround)
	{
		float gap = widthGround / 10.0;
		float temp;
		groundMaterial = Material::getMaterialByName("silver");
		lineMaterial = Material::getMaterialByName("redRubber");

		temp = centerX + 0.5*widthGround - gap * 0.5;

		for (int i = 0; i < 10; i++)
		{
			Line[i] = new Square(temp, centerZ, 2.2, lengthGround - 2 * gap, 0.5);
			temp -= gap;
		}

		temp = centerZ - 0.5*lengthGround + gap;

		Line[10] = new Square(centerX, temp, 2.2, 0.5, widthGround - gap);
		temp += gap;
		for (int i = 11; i < 25; i += 2)
		{
			Line[i] = new Square(centerX - 2.5*gap, temp, 2.2, 0.5, 4 * gap);
			Line[i + 1] = new Square(centerX + 2.5*gap, temp, 2.2, 0.5, 4 * gap);
			temp += gap;
		}
		Line[25] = new Square(centerX, temp, 2.2, 0.5, widthGround - gap);
		cross[0] = new boardCross(centerX - 3.5*gap, centerZ, gap);
		cross[1] = new boardCross(centerX + 3.5*gap, centerZ, gap);

		temp = centerZ + gap * 4;

		tag[0] = new Tag(centerX + 1.5*gap, temp, gap,true);
		tag[1] = new Tag(centerX - 1.5*gap, temp, gap,true);
		temp -= 2 * gap;
		for (int i = 1; i < 4; i++)
		{
			tag[i * 2] = new Tag(centerX + 1.5*gap, temp, gap);
			tag[i * 2 + 1] = new Tag(centerX - 1.5*gap, temp, gap);
			temp -= 2 * gap;
		}

		temp = centerZ +3 * gap;
		for (int i = 4; i < 6; i++)
		{
			tag[i * 2] = new Tag(centerX + 2.5*gap, temp, gap);
			tag[i * 2 + 1] = new Tag(centerX - 2.5*gap, temp, gap);
			temp -= 6 * gap;
		}
		
		//myChess = new chessGo(gap,myShader);
		groundObj[0]=new Square(centerX-3*gap, centerZ, heightGround, lengthGround, 5*gap);
		groundObj[1] = new Square(centerX+3*gap, centerZ, heightGround, lengthGround, 5*gap);
		groundObj[2] = new Square(centerX, centerZ, heightGround-1.9, lengthGround, gap);
		myManager = new chessManager(gap);
		//focus = new Tag(0, 0, 3 * gap, myShader);
		//focusColor = new glm::vec4(1.0, 1.0, 1.0, 1.0);
		
		//waterFlow(int maxI,int maxJ,int D,float cX,float cZ)
	}
	void drawChessBoard()
	{
		static int i;
		static glm::vec4 groundColor(0.7, 0.7, 0.7, 1.0);
		for (int i = 0; i < 26; i++)Line[i]->drawGround(lineMaterial);
		cross[0]->drawCross();
		cross[1]->drawCross();
		for (i = 0; i < 12; i++)tag[i]->drawTag();
		for(i=0;i<3;i++)
		groundObj[i]->drawGround(groundMaterial);
		
		myManager->display();
	}
};
#endif // ! CHESSBOARD_H

