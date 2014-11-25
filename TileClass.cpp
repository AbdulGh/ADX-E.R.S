#include "TileClass.h"

int SWidth = 1920;
int SHeight = 1080;

bool Between (int Min, int Value, int Max) {return Value >= Min && Value <= Max;}

bool Tile::IsOnScreen(int CameraX, int CameraY)
{
	return !(WorldX > CameraX + SWidth || WorldX + Width < CameraX || WorldY > CameraY + SHeight || WorldY + Height < CameraY);
}

SDL_Rect Tile::ScreenRect(int CameraX, int CameraY)
{
	int TempX = WorldX;
	int TempY = WorldY;
	int TempHeight = Height;
	int TempWidth = Width;

	if (TempX < CameraX)
	{
		TempWidth = TempX + TempWidth - CameraX;
		TempX = CameraX;
	}

	if (TempX + TempWidth > CameraX + SWidth)
	{
		TempWidth = CameraX + SWidth - TempX;
	}
	if (TempY < CameraY)
	{
		TempHeight = TempY + TempHeight - CameraY;
		TempY = CameraY;
	}

	if (TempY + TempHeight > CameraY + SHeight)
	{
		TempHeight = CameraY + SHeight - TempY;
	}

	SDL_Rect ReturnThis;
	ReturnThis.x = TempX - CameraX;
	ReturnThis.y = TempY - CameraY;
	ReturnThis.w = TempWidth;
	ReturnThis.h = TempHeight;
	return ReturnThis;
}