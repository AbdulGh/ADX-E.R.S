#include<Windows.h>
#include"DebugWindow.h"

#define LINES 6

std::vector <std::string> DebugLog;

void DebugWindow(std::string Argument)
{
	DebugLog.push_back(Argument);
	if (DebugLog.size() > LINES) DebugLog.erase(DebugLog.begin());
}

void PrintDebugWindow(TTF_Font *Font, SDL_Color Colour, SDL_Surface *Screen)
{
	SDL_Surface *Line = NULL;

	SDL_Rect Offset;
	Offset.x = 15;
	Offset.y = 1;
	for (int i = 0; i < DebugLog.size(); i++)
	{
		SDL_FreeSurface(Line);
		Line = TTF_RenderText_Solid(Font, DebugLog.at(i).c_str(), Colour);
		SDL_BlitSurface(Line, NULL, Screen, &Offset);
		Offset.y += 20;
	}
}

void DumpDebugWindowTostderr()
{
	for (int i = 0; i < DebugLog.size(); i++)
	{
		fprintf(stderr, DebugLog.at(i).c_str());
		fprintf(stderr, "\n");
	}
}