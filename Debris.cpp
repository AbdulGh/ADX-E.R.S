#include"Debris.h"
#include<time.h>

std::vector<Debris> DebrisVector;

void CreateDebris(int Size, int Number, float x, float y, int XVel, int YVel, Uint32 Colour)
{
	for (int i = 0; i < Number; i++)
	{
		Debris PushThis;

		PushThis.Rect.x = x;
		PushThis.Rect.y = y;
		PushThis.Rect.w = Size;
		PushThis.Rect.h = Size;

		PushThis.Colour = Colour;

		XVel += rand() % 20 - 10;
		YVel += rand() % 20 - 10;

		PushThis.XVel = XVel;
		PushThis.YVel = YVel;
		PushThis.Time = 0;

		DebrisVector.push_back(PushThis);
	}
}

#define CURRENTDEBRIS DebrisVector.at(i)
void DoDebris(int CameraX, int CameraY, SDL_Surface *Screen)
{
	if (DebrisVector.size() != 0)
	{
		for (int i = 0; i < DebrisVector.size(); i++)
		{
			CURRENTDEBRIS.Rect.x += CURRENTDEBRIS.XVel;
			CURRENTDEBRIS.Rect.y += CURRENTDEBRIS.YVel;
			SDL_Rect FillThis;
			FillThis.x = CURRENTDEBRIS.Rect.x - CameraX;
			FillThis.y = CURRENTDEBRIS.Rect.y - CameraY;
			FillThis.h =  CURRENTDEBRIS.Rect.h;
			FillThis.w = CURRENTDEBRIS.Rect.w;
			CURRENTDEBRIS.XVel /= 1.2;
			CURRENTDEBRIS.YVel /= 1.2;
			if (CURRENTDEBRIS.XVel < 1 && CURRENTDEBRIS.XVel > -1) CURRENTDEBRIS.XVel = 0;
			if (CURRENTDEBRIS.YVel < 1 && CURRENTDEBRIS.YVel > -1) CURRENTDEBRIS.YVel = 0;
			SDL_FillRect(Screen,&FillThis,CURRENTDEBRIS.Colour);
			CURRENTDEBRIS.Time++;
			if (CURRENTDEBRIS.Time == 60) DebrisVector.erase(DebrisVector.begin() + i, DebrisVector.begin() + i + 1);
		}
	}
}