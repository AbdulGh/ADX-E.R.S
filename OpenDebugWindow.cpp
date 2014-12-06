#include<Windows.h>
#include"OpenDebugWindow.h"

#define LINES 6

std::vector <std::string> DebugLog;

void OpenDebugWindow(std::string Argument)
{
	DebugLog.push_back(Argument);
	if (DebugLog.size() > LINES) DebugLog.erase(DebugLog.begin());
}

void PrintDebugWindow(TTF_Font *Font, SDL_Color Colour, SDL_Surface *Screen)
{
	SDL_Surface *Line;

	SDL_Rect Offset;
	Offset.x = 15;
	Offset.y = 1;
	for (int i = 0; i < DebugLog.size(); i++)
	{
		Line = TTF_RenderText_Solid(Font, DebugLog.at(i).c_str(), Colour);
		SDL_BlitSurface(Line, NULL, Screen, &Offset);
		Offset.y += 20;
	}
}