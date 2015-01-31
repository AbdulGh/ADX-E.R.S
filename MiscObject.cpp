#include "MiscObject.h"

MiscObject::MiscObject(int One, int Two, SDL_Surface Three, int Four, int Five, int Six, SDL_Rect Seven)
{
	WorldX = One;
	WorldY = Two;
	Surface = Three;
	XVel = Four;
	YVel = Five;
	Time = Six;
	Rekt = Seven;
}

MiscObject::~MiscObject()
{
	//SDL_FreeSurface(&Surface);
}

std::vector<MiscObject> ObjectVector;

void AddObject(int X, int Y, SDL_Surface Image, int XSpeed, int YSpeed, int Frames, SDL_Rect Clip)
{
	if (Clip.w == 0)
	{
		Clip.w = Image.w;
		Clip.h = Image.h;
	}
	
	MiscObject PushThis(X, Y, Image, XSpeed, YSpeed, Frames, Clip);
	ObjectVector.push_back(PushThis);
}

#define CURRENTOBJECT ObjectVector.at(i)
void DoObjects(float CameraX, float CameraY)
{
	for (int i = 0; i < ObjectVector.size(); i++)
	{
		if (abs(CURRENTOBJECT.XVel) > 0.3)
		{
			CURRENTOBJECT.WorldX += CURRENTOBJECT.XVel;
			CURRENTOBJECT.XVel /= 1.1;
		}

		if (abs(CURRENTOBJECT.YVel) > 0.3)
		{
			CURRENTOBJECT.WorldY += CURRENTOBJECT.YVel;
			CURRENTOBJECT.YVel /= 1.1;
		}

		ApplySurface(CURRENTOBJECT.WorldX - CameraX, CURRENTOBJECT.WorldY - CameraY, &CURRENTOBJECT.Surface, Screen, &CURRENTOBJECT.Rekt);

		if (CURRENTOBJECT.Time != -1)
		{
			CURRENTOBJECT.Time--;
			if (CURRENTOBJECT.Time == 0) ObjectVector.erase(ObjectVector.begin() + i);
		}
	}
}
