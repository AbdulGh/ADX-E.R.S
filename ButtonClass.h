#ifndef BUTTONCLASS_H
#define BUTTONCLASS_H
#include"Declarations.h"
#include"DebugWindow.h"

class Button
{
public:
	SDL_Rect ButtonRect;
	SDL_Surface *Normal;
	SDL_Surface *NotNormal;

	int ID;
	int Offset;
	 
	Button();
	bool IsMouseOver(int Mx, int My);
};

#endif