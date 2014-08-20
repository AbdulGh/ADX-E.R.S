#include"DoTiles.h"
#include"TileClass.h"
#define CURRENTTILE LevelVector.at(i)
#define CURRENTRECT RectVector.at(i)
	
void CreateTile(int ex, int y, int w, int h)
{
	Tile PushToVectorPls;
	PushToVectorPls.WorldX = ex;
	PushToVectorPls.WorldY = y;
	PushToVectorPls.Width = w;
	PushToVectorPls.Height = h;
	LevelVector.push_back(PushToVectorPls);
}

void DoTiles(int XCamera, int YCamera)
{
	RectVector.erase(RectVector.begin(), RectVector.end());
	ClearScreen();
	SDL_FillRect(Screen,NULL,LevelColour);
	if (LevelVector.size() != 0)
	{
		for (int i = 0; i < LevelVector.size(); i++)
		{
			if (!CURRENTTILE.IsOnScreen(XCamera, YCamera)) continue;
			RectVector.push_back(CURRENTTILE.ScreenRect(XCamera,YCamera));
		}
	}

	for (int i = 0; i < RectVector.size(); i++)
	{
		SDL_FillRect(Screen,&CURRENTRECT,0xFFFFFF);
	}
}