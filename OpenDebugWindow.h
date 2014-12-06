#ifndef OPENDEBUGWINDOW_H
#define OPENDEBUGWINDOW_H

#include<vector>
#include<string>
#include<SDL.h>
#include<SDL_ttf.h>

extern std::vector <std::string> DebugLog;

extern void OpenDebugWindow(extern std::string Argument);
void PrintDebugWindow(TTF_Font *Font, SDL_Color Colour, SDL_Surface *Screen);

#endif