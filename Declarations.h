#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "OpenDebugWindow.h"
#include "ButtonClass.h"
#include "TileClass.h"
#include "MapCreator.h"
#include "CameraClass.h"
#include "DoProjectiles.h"
#include <fstream>

extern SDL_Event event;

extern const int ScreenWidth;
extern const int ScreenHeight;

extern SDL_Surface *Screen;
extern SDL_Surface *Message;
extern SDL_Surface *PlayerNormal;
extern SDL_Surface *CursorSheet;
extern SDL_Surface *TeleportSheet;

extern SDL_Colour White;
extern SDL_Colour Green;
extern SDL_Colour Red;
extern SDL_Colour Blue;

extern SDL_Rect CursorClips[2];

extern int XChange;
extern int YChange;
extern int LevelWidth;
extern int LevelHeight;
extern int Frame;
extern int Frametime;

extern Uint32 LevelColour;

extern std::vector<Tile> LevelVector;
extern std::vector<SDL_Rect> RectVector;
extern std::vector<Projectile> ProjectileVector;

extern std::stringstream SpareStream;

extern Viewport Camera;

extern TTF_Font *Start;
extern TTF_Font *StartBig;
extern TTF_Font *Sys;
extern TTF_Font *SysSmall;

extern enum Gamestate {GAME,OPTIONS,QUIT};
extern Gamestate State;

SDL_Surface *LoadImage( std::string filename );
bool IsIntersecting(SDL_Rect Rect1, SDL_Rect Rect2);
bool Init();
void ApplySurface( int x, int y, SDL_Surface* Source, SDL_Surface* Destination, SDL_Rect* Clip = NULL );
bool Load();
void ClearScreen();
void SetClips();
void DoMouse(int * ex, int * why);

#endif