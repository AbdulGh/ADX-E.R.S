#include"Declarations.h"
#include"OpenDebugWindow.h"
#include"DoProjectiles.h"

SDL_Event event;

const int ScreenWidth = 1366;
const int ScreenHeight = 768;

bool Damaged = false;
bool Invincible = false;

int XChange = 0;
int YChange = 0;
int LevelWidth = 2000;
int LevelHeight = 2000;
int Frame = 0;
int Frametime = 0;
int DamageDealt = 0;
int Enemies = 0;

Uint32 LevelColour = 0xFF0000;

std::vector<Tile> LevelVector;
std::vector<Projectile> ProjectileVector;
std::vector<SDL_Rect> RectVector;

std::stringstream SpareStream;

Viewport Camera;

SDL_Surface *Screen = NULL;
SDL_Surface *Gunman = NULL;
SDL_Surface *Message = NULL;
SDL_Surface *PlayerNormal = NULL;
SDL_Surface *CursorSheet = NULL;
SDL_Surface *TeleportSheet = NULL;
SDL_Surface *Suicide = NULL;

SDL_Colour White = {255,255,255};
SDL_Colour Red = {255,0,0};
SDL_Colour Green = {0,255,0};
SDL_Colour Blue = {0,0,255};

TTF_Font *Start = NULL;
TTF_Font *StartBig = NULL;
TTF_Font *Sys = NULL;
TTF_Font *SysSmall = NULL;

SDL_Rect CursorClips[2];
SDL_Rect TeleportClips[2];

void SetClips()
{
	CursorClips[0].x = 0;
	CursorClips[0].y = 0;
	CursorClips[0].w = 21;
	CursorClips[0].h = 21;

	CursorClips[1].x = 0;
	CursorClips[1].y = 21;
	CursorClips[1].w = 21;
	CursorClips[1].h = 21;

	TeleportClips[0].x = 0;
	TeleportClips[0].y = 0;
	TeleportClips[0].w = 40;
	TeleportClips[0].h = 40;

	TeleportClips[1].x = 0;
	TeleportClips[1].y = 40;
	TeleportClips[1].w = 40;
	TeleportClips[1].h = 40;
}

void DoMouse(int * ex, int * why)
{
	SDL_PumpEvents();
	SDL_GetMouseState(ex,why);

	Frametime++;
	if (Frametime > 30) 
	{
		Frame = 1 - Frame;
		Frametime = 0;
	}
	ApplySurface(*ex-10,*why-10,CursorSheet,Screen,&CursorClips[Frame]);
}

Gamestate State;

SDL_Surface *LoadImage( std::string filename )
{
    OpenDebugWindow(filename);
	SDL_Surface* LoadedImage = NULL;
    SDL_Surface* OptimizedImage = NULL;
    LoadedImage = IMG_Load( filename.c_str() );
    if( LoadedImage != NULL )
    {
        OptimizedImage = SDL_DisplayFormatAlpha( LoadedImage );
        SDL_FreeSurface( LoadedImage );
    }
	else
	{
		OpenDebugWindow("Failed to load last file");
		OpenDebugWindow("Open");
	}

    return OptimizedImage;
}

bool Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
	if(TTF_Init() == -1) return false;
	Screen = SDL_SetVideoMode(ScreenWidth,ScreenHeight,32,SDL_SWSURFACE/*|SDL_FULLSCREEN*/);
	if (Screen == NULL) return false;
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) return false;
    SDL_WM_SetCaption("it worked", NULL);
    SDL_EnableUNICODE(SDL_ENABLE);
    return true;

	XChange = 0;
	YChange = 0;
}
void ApplySurface( int x, int y, SDL_Surface* Source, SDL_Surface* Destination, SDL_Rect* Clip)
{
    SDL_Rect offset;
	offset.x = x + XChange;
    offset.y = y + YChange;
    SDL_BlitSurface( Source, Clip, Destination, &offset );
}
bool Load()
{
	Start = TTF_OpenFont("Resources/Fonts/Start.ttf",40);
	StartBig = TTF_OpenFont("Resources/Fonts/Start.ttf",128);
	Sys = TTF_OpenFont("Resources/Fonts/Sys.ttf",160);
	SysSmall = TTF_OpenFont("Resources/Fonts/Sys.ttf",32);
	if (Start == NULL || Sys == NULL) return false;

	PlayerNormal = LoadImage("Resources/Images/Player.png");
	CursorSheet = LoadImage("Resources/Images/Cursor.png");
	TeleportSheet = LoadImage("Resources/Images/Teleport.png");
	Suicide = LoadImage("Resources/Images/Suicide.png");
	Gunman = LoadImage("Resources/Images/Gunman.png");
	if (PlayerNormal == NULL || CursorSheet == NULL) return false;
	return true;
}

bool IsIntersecting(SDL_Rect Rect1, SDL_Rect Rect2)
{
	if (Rect2.x > Rect1.x && Rect2.x < Rect1.x + Rect1.w)
	{
		if (Rect2.y > Rect1.y && Rect2.y < Rect1.y + Rect1.h) return true;
		else if (Rect2.y + Rect2.h > Rect1.y && Rect2.y + Rect2.h < Rect1.y + Rect1.h) return true;
	}
	else if (Rect2.x + Rect2.w > Rect1.x && Rect2.x + Rect2.w < Rect1.x + Rect1.w)
	{
		if (Rect2.y > Rect1.y && Rect2.y < Rect1.y + Rect1.h) return true;
		else if (Rect2.y + Rect2.h > Rect1.y && Rect2.y + Rect2.h < Rect1.y + Rect1.h) return true;
	}
	return false;
}

void ClearScreen()
{
	SDL_FillRect(Screen,NULL,0x000000);
}
