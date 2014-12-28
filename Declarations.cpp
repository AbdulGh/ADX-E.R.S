#include"Declarations.h"
#include"OpenDebugWindow.h"
#include"DoProjectiles.h"

SDL_Event event;

int ScreenWidth = 1920;
int ScreenHeight = 1080;

bool Damaged = false;
bool Invincible = false;
bool Boss = false;
bool Shake = 0;
bool Laser = false;

float LaserY = 0;

int XChange = 0;
int YChange = 0;
int LevelWidth = 2000;
int LevelHeight = 2000;
int Frame = 0;
int Frametime = 0;
int DamageDealt = 0;
int Enemies = 0;
int Temp1 = 200;
int Temp2 = 0;
int Temp3 = 0;
int Dur = 0;
int Mag = 0;

Uint8 MouseStates;

int Ammo[WEAPONS] = {1,0,0,0,0,0};

Uint32 LevelColour = 0xFF0000;

std::vector<Tile> LevelVector;
std::vector<Projectile> ProjectileVector;
std::vector<SDL_Rect> RectVector;
std::vector <int> SpawnVector;

std::stringstream SpareStream;

Viewport Camera;

SDL_Surface *Screen = NULL;
SDL_Surface *Serious = NULL;
SDL_Surface *Gunman = NULL;
SDL_Surface *Message = NULL;
SDL_Surface *Message2 = NULL;
SDL_Surface *Message3 = NULL;
SDL_Surface *PlayerNormal = NULL;
SDL_Surface *CursorSheet = NULL;
SDL_Surface *TeleportSheet = NULL;
SDL_Surface *Suicide = NULL;
SDL_Surface *Shotgun = NULL;
SDL_Surface *Ship = NULL;
SDL_Surface *ShipProjectile = NULL;
SDL_Surface *Worm = NULL;
SDL_Surface *Invader = NULL;
SDL_Surface *Rocket = NULL;
SDL_Surface *MachineGun = NULL;
SDL_Surface *Health = NULL;
SDL_Surface *Win = NULL;
SDL_Surface *Fail = NULL;
SDL_Surface *DoorGuard = NULL;
SDL_Surface *Spawner = NULL;
SDL_Surface *Flamethrower = NULL;
SDL_Surface *LaserPickup = NULL;
SDL_Surface *Frog = NULL;
SDL_Surface *Silo = NULL;
SDL_Surface *VertLaser = NULL;
SDL_Surface *Biggie = NULL;
SDL_Surface *Warden = NULL;
SDL_Surface *RIP = NULL;
SDL_Surface *Grenade = NULL;

SDL_Colour White = {255,255,255};
SDL_Colour Red = {255,0,0};
SDL_Colour Green = {0,255,0};
SDL_Colour Blue = {0,0,255};

TTF_Font *Start = NULL;
TTF_Font *StartBig = NULL;
TTF_Font *Sys = NULL;
TTF_Font *SysMid = NULL;
TTF_Font *SysSmall = NULL;
TTF_Font *Small = NULL;
TTF_Font *SmallSmall = NULL;

SDL_Rect CursorClips[2];
SDL_Rect TeleportClips[2];
SDL_Rect WormFrames[2];
SDL_Rect AmmoFrames[2];
SDL_Rect ShipFrames[3];
SDL_Rect InvaderFrames[2];

Mix_Chunk *ShotgunFire = NULL;
Mix_Chunk *ShotgunPump = NULL;
Mix_Chunk *MachineGunFire = NULL;
Mix_Chunk *Pistol = NULL;
Mix_Chunk *Empty = NULL;
Mix_Chunk *SmashDeath = NULL;

Mix_Music *SmashSong = NULL;

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

	WormFrames[0].x = 0;
	WormFrames[0].y = 0;
	WormFrames[0].w = 50;
	WormFrames[0].h = 50;

	WormFrames[1].x = 50;
	WormFrames[1].y = 0;
	WormFrames[1].w = 50;
	WormFrames[1].h = 50;

	AmmoFrames[0].x = 0;
	AmmoFrames[0].y = 0;
	AmmoFrames[0].w = 25;
	AmmoFrames[0].h = 25;

	AmmoFrames[1].x = 25;
	AmmoFrames[1].y = 0;
	AmmoFrames[1].w = 25;
	AmmoFrames[1].h = 25;

	ShipFrames[0].x = 0;
	ShipFrames[0].y = 0;
	ShipFrames[0].w = 50;
	ShipFrames[0].h = 20;

	ShipFrames[1].x = 50;
	ShipFrames[1].y = 0;
	ShipFrames[1].w = 50;
	ShipFrames[1].h = 20;

	ShipFrames[2].x = 100;
	ShipFrames[2].y = 0;
	ShipFrames[2].w = 50;
	ShipFrames[2].h = 20;

	InvaderFrames[0].x = 0;
	InvaderFrames[0].y = 0;
	InvaderFrames[0].w = 50;
	InvaderFrames[0].h = 50;

	InvaderFrames[1].x = 50;
	InvaderFrames[1].y = 0;
	InvaderFrames[1].w = 50;
	InvaderFrames[1].h = 50;
}

void DoMouse(int * ex, int * why)
{
	SDL_PumpEvents();
	MouseStates = SDL_GetMouseState(ex,why);

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
	Screen = SDL_SetVideoMode(ScreenWidth,ScreenHeight,32,SDL_SWSURFACE|SDL_FULLSCREEN);
	if (Screen == NULL) return false;
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) return false;
	Mix_AllocateChannels(16);
    SDL_WM_SetCaption("it worked", NULL);
    SDL_EnableUNICODE(SDL_ENABLE);
    return true;

	XChange = 0;
	YChange = 0;
}

void ApplySurface( int x, int y, SDL_Surface* Source, SDL_Surface* Destination, SDL_Rect* Clip)
{
	if (Source != NULL)
	{
		SDL_Rect offset;
		offset.x = x;
		offset.y = y;
		SDL_BlitSurface(Source, Clip, Destination, &offset);
	}
}

bool Load()
{
	Start = TTF_OpenFont("Resources/Fonts/Start.ttf",40);
	Small = TTF_OpenFont("Resources/Fonts/Pixelmix.ttf",14);
	SmallSmall = TTF_OpenFont("Resources/Fonts/Pixelmix.ttf",12);
	StartBig = TTF_OpenFont("Resources/Fonts/Start.ttf",128);
	Sys = TTF_OpenFont("Resources/Fonts/Sys.ttf",160);
	SysMid = TTF_OpenFont("Resources/Fonts/Sys.ttf", 64);
	SysSmall = TTF_OpenFont("Resources/Fonts/Sys.ttf",32);
	if (Start == NULL || Sys == NULL || Small == NULL) return false;

	ShotgunPump = Mix_LoadWAV("Resources/Sounds/Pump.wav");
	ShotgunFire = Mix_LoadWAV("Resources/Sounds/Shotgun.wav");
	MachineGunFire = Mix_LoadWAV("Resources/Sounds/MachineGun.wav");
	Pistol = Mix_LoadWAV("Resources/Sounds/Pistol.wav");
	Empty = Mix_LoadWAV("Resources/Sounds/Empty.ogg");
	SmashDeath = Mix_LoadWAV("Resources/Sounds/SmashDeath.wav");

	SmashSong = Mix_LoadMUS("Resources/Sounds/Smash.ogg");

	PlayerNormal = LoadImage("Resources/Images/Player.png");
	Spawner = LoadImage("Resources/Images/Spawner.png");
	Serious = LoadImage("Resources/Images/Serious.png");
	CursorSheet = LoadImage("Resources/Images/Cursor.png");
	TeleportSheet = LoadImage("Resources/Images/Teleport.png");
	Suicide = LoadImage("Resources/Images/Suicide.png");
	Gunman = LoadImage("Resources/Images/Gunman.png");
	Shotgun = LoadImage("Resources/Images/Shotgun.png");
	Ship = LoadImage("Resources/Images/Ship.png");
	Win = LoadImage("Resources/Images/Win.png");
	Fail = LoadImage("Resources/Images/Fail.png");
	Invader = LoadImage("Resources/Images/Invader.png");
	Health = LoadImage("Resources/Images/Health.png");
	Worm = LoadImage("Resources/Images/Worm.png");
	ShipProjectile = LoadImage("Resources/Images/EnemyProjectile.png");
	MachineGun = LoadImage("Resources/Images/MachineGun.png");
	DoorGuard = LoadImage("Resources/Images/DoorGuard.png");
	Flamethrower = LoadImage("Resources/Images/Flamethrower.png");
	Frog = LoadImage("Resources/Images/Frog.png");
	LaserPickup = LoadImage("Resources/Images/Laser.png");
	Silo = LoadImage("Resources/Images/Missiles.png");
	VertLaser = LoadImage("Resources/Images/VertLaser.png");
	Rocket = LoadImage("Resources/Images/Rocket.png");
	Biggie = LoadImage("Resources/Images/Biggie.png");
	Warden = LoadImage("Resources/Images/Warden.png");
	RIP = LoadImage("Resources/Images/RIP.png");
	Grenade = LoadImage("Resources/Images/Grenade.png");

	if (PlayerNormal == NULL || CursorSheet == NULL) return false;
	return true;
}

bool InBetween (int Min, int Value, int Max) {return Value >= Min && Value <= Max;}

bool IsIntersecting(SDL_Rect Small, SDL_Rect Big)
{
	return !(Small.x > Big.x + Big.w || Small.x + Small.w < Big.x || Small.y > Big.y + Big.h || Small.y + Small.h < Big.y);
}

void ClearScreen()
{
	SDL_FillRect(Screen,NULL,0x000000);
}
