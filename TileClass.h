#ifndef TILECLASS_H
#define TILECLASS_H
#include<SDL.h>
#include<SDL_main.h>

class Tile
{
public:
	int WorldX;
	int WorldY;
	int Width;
	int Height;

	bool IsIntersecting(int x, int y, int w, int h);
	bool IsOnScreen(int CameraX, int CameraY);

	bool operator==(const Tile& rhs);

	SDL_Rect ScreenRect(int CameraX, int CameraY);
};

void SetTileScreen(int Width, int Height);

#endif