#include"LoadLevel.h"
#include"DoTiles.h"
#include<iostream>
#include<fstream>

inline bool operator==(const SDL_Rect& One, const SDL_Rect& Two)
{
	return One.x == Two.x && One.y == Two.y && One.w == Two.w && One.h == Two.h;
}

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
		
		if (LevelVector.empty() || !(LevelVector.back().WorldX == TempX && LevelVector.back().WorldY == TempY
			&& LevelVector.back().Width == TempW && LevelVector.back().Height == TempH))
		{
			CreateTile(TempX, TempY, TempW, TempH);
		}

		else DebugWindow("Purge");
	}
	return true;
}