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

	int TempX;
	int TempY;
	int TempW;
	int TempH;

	while(!Input.eof())
	{
		Input >> TempX;
		Input >> TempY;
		Input >> TempW;
		Input >> TempH;

		CreateTile(TempX,TempY,TempW,TempH);
	}
	return true;
}