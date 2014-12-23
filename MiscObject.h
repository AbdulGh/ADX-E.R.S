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

	MiscObject(int,int, SDL_Surface* , int, int, int);
	~MiscObject();
};

extern std::vector<MiscObject> ObjectVector;

void AddObject(int,int, SDL_Surface* , int, int, int);
void DoObjects(float CameraX, float CameraY);
#endif