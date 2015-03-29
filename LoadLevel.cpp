#include"LoadLevel.h"
#include"DoTiles.h"
#include<iostream>
#include<fstream>

bool LoadLevel(std::string Filename)
{
	std::ifstream Input;
	Input.open(Filename);
	if (Input.fail()) return false;
	LevelVector.erase(LevelVector.begin(),LevelVector.end());

	Input >> LevelWidth;
	Input >> LevelHeight;
	Input >> LevelColour;
	LevelColour = 0x000000;

	int TempX;
	int TempY;
	int TempW;
	int TempH;

	while (!(Input.peek(), Input.eof()))
	{
		Input >> TempX;
		Input >> TempY;
		Input >> TempW;
		Input >> TempH;

		if (LevelVector.size() < 2 || !(LevelVector.at(LevelVector.size() - 1) == LevelVector.at(LevelVector.size() - 2)))
			CreateTile(TempX, TempY, TempW, TempH);
	}
	return true;
}