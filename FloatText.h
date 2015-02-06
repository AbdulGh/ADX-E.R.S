#ifndef FLOATTEXT_H
#define FLOATTEXT_H

#include"Declarations.h"

class FloatText
{
public:
	float x;
	float y;
	int Alpha;
	SDL_Color Colour;
	std::string String;
	int Time;
	int Type;

	bool IsNotActive() const;
};

extern std::vector <FloatText> FloatVector;

void FloatSomeText(int WorldX, int WorldY, std::string String, SDL_Color Colour, int Type = 1);
void DoFloat(float CameraX, float CameraY);
#endif