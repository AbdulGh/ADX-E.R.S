#include "TileClass.h"
#define ITopLeftx x
#define ITopRightx x+w
#define IBottomLeftx x
#define IBottomRightx x+w
#define ITopLefty y
#define ITopRighty y
#define IBottomLefty y+h
#define IBottomRighty y+h

#define TTopLeftx WorldX
#define TTopRightx WorldX+Width
#define TBottomLeftx WorldX
#define TBottomRightx WorldX+Width
#define TTopLefty WorldY
#define TTopRighty WorldY
#define TBottomLefty WorldY+Height
#define TBottomRighty WorldY+Height

int ScreenWidth = 1366;
int ScreenHeight = 768;

bool Tile::IsOnScreen(int CameraX, int CameraY)
{
	if (TTopRightx >= CameraX && TTopRightx <= CameraX + ScreenWidth &&  TTopRighty >= CameraY && TTopRighty <= CameraY + ScreenHeight) return true;
	else if (TTopLeftx >= CameraX && TTopLeftx <= CameraX + ScreenWidth &&  TTopLefty >= CameraY && TTopLefty <= CameraY + ScreenHeight) return true;
	else if (TBottomLeftx >= CameraX && TBottomLeftx <= CameraX + ScreenWidth &&  TBottomLefty >= CameraY && TBottomLefty <= CameraY + ScreenHeight) return true;
	else if (TBottomRightx >= CameraX && TBottomRightx <= CameraX + ScreenWidth &&  TBottomRighty >= CameraY && TBottomRighty <= CameraY + ScreenHeight) return true;

	else if (CameraX >= TTopLeftx && CameraX <= TTopRightx && CameraY >= TTopLefty && CameraY <= TBottomLefty) return true;
	else if (CameraX + ScreenWidth >= TTopLeftx && CameraX + ScreenWidth <= TTopRightx && CameraY >= TTopLefty && CameraY <= TBottomLefty) return true;
	else if (CameraX >= TTopLeftx && CameraX <= TTopRightx && CameraY + ScreenHeight >= TTopLefty && CameraY + ScreenHeight <= TBottomLefty) return true; 
	else if (CameraX + ScreenWidth >= TTopLeftx && CameraX + ScreenWidth <= TTopRightx && CameraY + ScreenHeight >= TTopLefty && CameraY + ScreenHeight <= TBottomLefty) return true;
	return false;
}

SDL_Rect Tile::ScreenRect(int CameraX, int CameraY)
{
	int TempX = WorldX;
	int TempY = WorldY;
	int TempWidth = Width;
	int TempHeight = Height;
	if (TempX <= CameraX) {TempWidth = TempX + TempWidth - CameraX; TempX = CameraX;}
	else if (TempX + TempWidth >= ScreenWidth + CameraX) TempWidth = ScreenWidth + CameraX - TempX; //and guns in this games
	if (TempY <= CameraY) { TempHeight = TempY + TempHeight - CameraY; TempY = CameraY;}
	else if (TempY + TempHeight >= ScreenHeight + CameraY) TempHeight = ScreenHeight + CameraY - TempY;
	SDL_Rect ReturnThis;
	ReturnThis.x = TempX - CameraX;
	ReturnThis.y = TempY - CameraY;
	ReturnThis.h = TempHeight;
	ReturnThis.w = TempWidth;
	return ReturnThis;
}