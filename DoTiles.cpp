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

	DebugWindow("New tile created:");
	SpareStream.str("");
	SpareStream << "Y: " << ex << " Y: " << y << " W: " << w << " H: " << h;
	DebugWindow(SpareStream.str().c_str());
}

void CreateTile(SDL_Rect Rect)
{
	Tile PushToVectorPls;
	PushToVectorPls.WorldX = Rect.x;
	PushToVectorPls.WorldY = Rect.y;
	PushToVectorPls.Width = Rect.w;
	PushToVectorPls.Height = Rect.h;
	LevelVector.push_back(PushToVectorPls);
}

void DoTiles(int XCamera, int YCamera)
{
	RectVector.erase(RectVector.begin(), RectVector.end());
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