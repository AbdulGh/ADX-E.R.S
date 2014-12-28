#include "MiscObject.h"

MiscObject::MiscObject(int One, int Two, SDL_Surface Three, int Four, int Five, int Six )
{
	WorldX = One;
	WorldY = Two;
	Surface = Three;
	XVel = Four;
	YVel = Five;
	Time = Six;
}

MiscObject::~MiscObject()
{
	//SDL_FreeSurface(&Surface);
}

std::vector<MiscObject> ObjectVector;

void AddObject(int X, int Y, SDL_Surface *Image, int XSpeed, int YSpeed, int Frames, SDL_Rect Clip)
{
	if (Clip.w != 0)
	{
		//SDL_Surface PleaseWork = *SDL_CreateRGBSurface(Image->flags, Clip.w, Clip.h, Image->format->BitsPerPixel, Image->format->Rmask, Image->format->Gmask,
		//										Image->format->Bmask, Image->format->Amask);
		SDL_Surface PleaseWork = *Image;
		ApplySurface(0, 0, Image, &PleaseWork, &Clip);
		__debugbreak();

		/*PleaseWork.w = Clip->w;
		PleaseWork.h = Clip->h;*/
		MiscObject PushThis(X, Y, PleaseWork, XSpeed, YSpeed, Frames);
		ClearScreen();
		ApplySurface(400, 0, Warden, Screen);
		ApplySurface(0, 0, Image, Screen);
		ApplySurface(800, 0, Image, Screen, &Clip);
		ApplySurface(1200, 0, &PleaseWork, Screen);
		SDL_FillRect(Screen, &Clip, 0xFF0000);
		SDL_Flip(Screen);
		SDL_Delay(3000);
		ObjectVector.push_back(PushThis);
	}

	else
	{
		MiscObject PushThis(X, Y, *Image, XSpeed, YSpeed, Frames);
		ObjectVector.push_back(PushThis);
	}
}

#define CURRENTOBJECT ObjectVector.at(i)
void DoObjects(float CameraX, float CameraY)
{
	for (int i = 0; i < ObjectVector.size(); i++)
	{
		//__debugbreak();
		if (CURRENTOBJECT.Time != -1)
		{
			CURRENTOBJECT.Time--;
			if (CURRENTOBJECT.Time == 0) CURRENTOBJECT.~MiscObject();
		}

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

		ApplySurface(CURRENTOBJECT.WorldX - CameraX, CURRENTOBJECT.WorldY - CameraY, &CURRENTOBJECT.Surface, Screen);
	}
}
