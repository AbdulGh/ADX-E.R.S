#ifndef MISCOBJECT_H
#define MISCOBJECT_H
#include "Declarations.h"

class MiscObject
{
public:
	float WorldX;
	float WorldY;
	float XVel;
	float YVel;
	int Time;

	SDL_Surface *Surface;
	SDL_Rect Rekt;

	MiscObject(int,int, SDL_Surface* , int, int, int, SDL_Rect);
	bool IsNotActive() const;
};

extern std::vector<MiscObject> ObjectVector;

void AddObject(int, int, SDL_Surface*, int = 0, int = 0, int = -1, SDL_Rect = { 0, 0, 0, 0 });
void DoObjects(float CameraX, float CameraY);
#endif