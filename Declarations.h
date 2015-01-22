#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#define WEAPONS 6

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
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
#include "SDL_rotozoom.h"

extern SDL_Event event;

extern int ScreenWidth;
extern int ScreenHeight;

extern SDL_Surface *Screen;
extern SDL_Surface *Gunman;
extern SDL_Surface *Message;
extern SDL_Surface *Rocket;
extern SDL_Surface *Message2;
extern SDL_Surface *Message3;
extern SDL_Surface *PlayerNormal;
extern SDL_Surface *CursorSheet;
extern SDL_Surface *TeleportSheet;
extern SDL_Surface *Suicide;
extern SDL_Surface *Shotgun;
extern SDL_Surface *Serious;
extern SDL_Surface *Ship;
extern SDL_Surface *ShipProjectile;
extern SDL_Surface *Worm;
extern SDL_Surface *Invader;
extern SDL_Surface *MachineGun;
extern SDL_Surface *Health;
extern SDL_Surface *Win;
extern SDL_Surface *Fail;
extern SDL_Surface *DoorGuard;
extern SDL_Surface *Spawner;
extern SDL_Surface *Flamethrower;
extern SDL_Surface *Frog;
extern SDL_Surface *LaserPickup;
extern SDL_Surface *Silo;
extern SDL_Surface *VertLaser;
extern SDL_Surface *Biggie;
extern SDL_Surface *Warden;
extern SDL_Surface *RIP;
extern SDL_Surface *Grenade;

extern SDL_Colour White;
extern SDL_Colour Green;
extern SDL_Colour Red;
extern SDL_Colour Blue;

extern SDL_Rect CursorClips[2];
extern SDL_Rect TeleportClips[2];
extern SDL_Rect WormFrames[2];
extern SDL_Rect AmmoFrames[2];
extern SDL_Rect ShipFrames[3];
extern SDL_Rect InvaderFrames[2];

extern bool Damaged;
extern bool Invincible;
extern bool Boss;
extern bool Shake;
extern bool Laser;
extern bool SpawnFlowers;

extern float LaserY;
extern float AngleOffset;

extern int XChange;
extern int YChange;
extern int LevelWidth;
extern int LevelHeight;
extern int Frame;
extern int Frametime;
extern int DamageDealt;
extern int Enemies;
extern int Temp1;
extern int Temp2;
extern int Temp3;
extern int Dur;
extern int Mag;
extern int OrbitX;
extern int OrbitY;

extern Uint8 MouseStates;

extern int Ammo[WEAPONS];

extern Uint32 LevelColour;

extern std::vector<Tile> LevelVector;
extern std::vector<SDL_Rect> RectVector;
extern std::vector<Projectile> ProjectileVector;
extern std::vector <int> SpawnVector;

extern std::stringstream SpareStream;

extern Viewport Camera;

extern TTF_Font *Start;
extern TTF_Font *StartBig;
extern TTF_Font *Sys;
extern TTF_Font *SysMid;
extern TTF_Font *SysSmall;
extern TTF_Font *Small;
extern TTF_Font *SmallSmall;

extern Mix_Music *BossTheme;

extern Mix_Chunk *Metal;
extern Mix_Chunk *ShotgunFire;
extern Mix_Chunk *ShotgunPump;
extern Mix_Chunk *MachineGunFire;
extern Mix_Chunk *Pistol;
extern Mix_Chunk *Empty;
extern Mix_Chunk *SmashDeath;
extern Mix_Chunk *Impact;

extern enum Gamestate {GAME,OPTIONS,QUIT};
extern Gamestate State;

SDL_Surface *LoadImage( std::string filename );
bool IsIntersecting(SDL_Rect Rect1, SDL_Rect Rect2);
bool Init();
bool InBetween (int Min, int Value, int Max);
void ApplySurface( int x, int y, SDL_Surface* Source, SDL_Surface* Destination, SDL_Rect* Clip = NULL );
bool Load();
void ClearScreen();
void SetClips();
void DoMouse(int * ex, int * why);

#endif