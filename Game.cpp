#include"Game.h"
#include"Declarations.h"
#include"PlayerClass.h"
#include"Timer.h"
#include"LoadLevel.h"
#include"Menu.h"
#include"FadeText.h"
#include"DoAsteroids.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"DoTiles.h"
#include"Debris.h"
#include"Enemies.h"
#include"DoEnemyProjectiles.h"
#include"FloatText.h"
#include"MiscObject.h"
#include"Terminal.h"
#include<string>
#include<string.h>
#include<fstream>

int CurrentSelection = 1;
int ShotTimer = 0;
int XSpawn = 0;
int YSpawn = 0;
int LevelProgress = 0;
int x = 0;
int y = 0;
int Number = 0;
int TopBounces = 1;
int TypeDelay = 0;
int BlockHeight = 0;
int ScreenAlpha = 0;
int ScreenDirection = 8;
int LoopingChannel = -50;

float LaserSpeed = 2.4;
float BoxXVel = 1, BoxYVel = 1;
float Zoom = 1;
float Direction = -0.02;

bool Update = true;
bool LevelFinished = false;
bool CutsceneFinished = false;
bool DebugBool = false;
bool Ded = false;
Timer SpareTimer;

std::string AmmoNames[WEAPONS] = {"Pistol","Shotgun","Machinegun","Flamethrower","Laser SMG", "RPG", "Automatic Laser Shotgun", "Grenade Machinegun", "Minigun"};

Player Character;

SDL_Rect CharacterRect;
SDL_Rect HealthRect;

SDL_Surface *ScreenShot;

void CheckShake()
{
	if (ScreenShake && Shake == true)
	{
		Camera.x += (rand() % Mag) - Mag/2;
		Camera.y += (rand() % Mag) - Mag/2;
		Dur--;
		if (Dur == 0) Shake = false;
	}
}

void CheckScreenFade()
{
	if (ScreenAlpha > 0)
	{
		SDL_SetAlpha(Fader, SDL_SRCALPHA, ScreenAlpha);
		ScreenAlpha += ScreenDirection;
		ApplySurface(0, 0, Fader, Screen);

		if (ScreenAlpha >= 250) ScreenDirection *= -1;
		else if (ScreenAlpha <= 0)
		{
			ScreenAlpha = 0;
			ScreenDirection = 1;
		}
	}
}

void SwapWeapons(bool Right)
{
	if (LoopingChannel != -50) Mix_HaltChannel(LoopingChannel);

	if (Right)
	{
		do 
		{
			CurrentSelection++;
			if (CurrentSelection > WEAPONS) CurrentSelection = 1;
		} while (Ammo[CurrentSelection - 1] == 0);
	}
	else
	{
		do  
		{
			CurrentSelection--;
			if (CurrentSelection < 1) CurrentSelection = WEAPONS;
		} while (Ammo[CurrentSelection - 1] == 0);
	}
}

void DeathScreen()
{
	SpareTimer.start();
	Mix_HaltMusic();
	if (LoopingChannel != -50) Mix_HaltChannel(LoopingChannel);
	while (SpareTimer.get_ticks() < 5000)
	{
		ApplySurface(0, 0, ScreenShot, Screen);
		DoDebris(Camera.x, Camera.y, Screen);
		
		ApplyTextCentered("Lives:", SysSmall, Green, NULL, NULL, 0, -100);

		SpareStream.str("");
		SpareStream << Character.Lives;

		SDL_Surface *Zoomed = rotozoomSurface(TTF_RenderText_Solid(StartBig, SpareStream.str().c_str(), Green), 0, Zoom, 0);
		ApplySurface((ScreenWidth - Zoomed->w) / 2, (ScreenHeight - Zoomed->h) / 2, Zoomed, Screen);
		SDL_FreeSurface(Zoomed);

		if (Zoom <= 0)
		{
			Direction *= -1;
			Character.Lives--;
			Zoom = Direction;
			SDL_Delay(500);
		}

		else if (Zoom <= 1) Zoom += Direction;

		SDL_Flip(Screen);
		SDL_Delay(20);
	}
	Zoom = 1;
	Direction = -0.02;

}

void DoThings()
{
	ClearScreen();
	SDL_FillRect(Screen,NULL,LevelColour);

	if (ShotTimer != 0) ShotTimer--;
	
	if (Update)
	{
		Camera.MoveViewport(Character.WorldX + (Character.CurrentSprite->w / 2) - (ScreenWidth / 2),Character.WorldY + (Character.CurrentSprite->h / 2) - (ScreenHeight / 2));
	}
	Camera.Update();

	CheckShake();
	DoDebris(Camera.x,Camera.y,Screen);
	Character.Update();
	CharacterRect.x = Character.WorldX - Camera.x;
	CharacterRect.y = Character.WorldY - Camera.y;

	DoPickups(Camera.x,Camera.y,CharacterRect);
	DoEnemyProjectiles(Camera.x, Camera.y,CharacterRect);
	DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, Character.XVel, Character.YVel);
	DoAsteroids(Camera.x, Camera.y, CharacterRect);
	DoObjects(Camera.x,Camera.y);
	DoTiles(Camera.x,Camera.y);
	DoFloat(Camera.x,Camera.y);
	DoProjectiles(Camera.x,Camera.y);

	if (DebugBool)
	{
		PrintDebugWindow(SysSmall, Green, Screen);
		Character.Health = 100;
	}

	HealthRect.w = 3 * Character.Health;
	if (Character.Health != 0) SDL_FillRect(Screen, &HealthRect, 0x00FF00);
	ApplyText(10, ScreenHeight - 30, "Health:", SysSmall, Green);
	SpareStream.str("");
	SpareStream << Character.Health << "%";
	ApplyText(HealthRect.x + HealthRect.w + 10, ScreenHeight - 30, SpareStream.str(), SysSmall, Green);

	SpareStream.str("");
	if (CurrentSelection == 1) SpareStream << "Mining Laser: Infinite";
	else SpareStream << AmmoNames[CurrentSelection - 1] << ": " << Ammo[CurrentSelection - 1];
	ApplyText(550, ScreenHeight - 30, SpareStream.str(), SysSmall, Green);

	if (DeathRect.w != 0)
	{
		SDL_Rect LaserRect;

		LaserRect.w = 3;
		LaserRect.h = ScreenHeight;
		LaserRect.y = 0;

		LaserRect.x = DeathRect.x - Camera.x - 4;
		SDL_FillRect(Screen, &LaserRect, 0xFF0000);
		LaserRect.x = DeathRect.x + DeathRect.w - Camera.x;
		SDL_FillRect(Screen, &LaserRect, 0xFF0000);

		LaserRect.h = 3;
		LaserRect.w = ScreenWidth;
		LaserRect.x = 0;

		LaserRect.y = DeathRect.y - Camera.y - 4;
		SDL_FillRect(Screen, &LaserRect, 0xFF0000);
		LaserRect.y = DeathRect.y + DeathRect.h - Camera.y;
		SDL_FillRect(Screen, &LaserRect, 0xFF0000);

		if (Character.WorldX < DeathRect.x || Character.WorldX + CharacterRect.w > DeathRect.x + DeathRect.w || Character.WorldY < DeathRect.y || Character.WorldY + CharacterRect.h > DeathRect.y + DeathRect.h)
		{
			Invincible = false;
			Damaged = true;
			DamageDealt = 1000;
		}
	}

	if (BlockHeight != 0)
	{
		SDL_Rect LaserBlock;
		LaserBlock.x = 0;
		LaserBlock.y = BlockHeight - Camera.y;
		LaserBlock.w = ScreenWidth;
		LaserBlock.h = 4;
		SDL_FillRect(Screen, &LaserBlock, 0xFF0000);

		if (Character.WorldY < BlockHeight)
		{
			Damaged = true;
			DamageDealt = 1000;
		}
	}

	if (Laser)
	{
		LaserY -= LaserSpeed;
		if (LaserY < Camera.y + ScreenHeight)
		{
			SDL_Rect LaserRect;
			LaserRect.x = 0;
			LaserRect.y = LaserY - Camera.y;
			LaserRect.w = ScreenWidth;
			LaserRect.h = 4;
			SDL_FillRect(Screen,&LaserRect,0xFF0000);
		}

		if (Character.WorldY + Character.CurrentSprite->h > LaserY)
		{
			Invincible = false;
			Character.InvunFrames = 0;
			Damaged = true;
			DamageDealt = 1000;
		}
	}

	if (Boss)
	{
		int Width = 0;
		ApplyText(15, 5, BossName.c_str(), SysSmall, Green, &Width);

		BossHealthRect.x = Width + 25;
		BossHealthRect.y = 5;
		BossHealthRect.h = 20;
		BossHealthRect.w = BossHealth * Multiplier;

		SDL_FillRect(Screen, &BossHealthRect, 0x00FF00);
	}

	else if (Enemies != 0)
	{
		SpareStream.str("");
		SpareStream << "Enemies: " << Enemies;
		ApplyTextCentered(SpareStream.str(), SysSmall, Green, NULL, NULL, 0, -(ScreenHeight / 2) + 30);
	}

	if (Character.Reset == true)
	{
		SDL_Delay(500);
		if (LoopingChannel != -50) Mix_HaltChannel(LoopingChannel);
		Character.Render = false;
		Boss = false;
		DeathRect.x = 0;
		Laser = false;
		DamageDealt = 0;
		CreateDebris(5,10,Character.WorldX,Character.WorldY,Character.XVel * 5, Character.YVel * 5, 0xFF0000);
		CreateDebris(5,10,Character.WorldX,Character.WorldY,Character.XVel * 5,Character.YVel * 5,0xFFFFFF);
		SpareTimer.start();

		if (Character.Lives <= 0)
		{
			ClearScreen();
			Terminal("Resources/Text/Dead");
			SDL_Delay(2000);
			QuitGame(false);
			exit(0);
		}

		else
		{
			ScreenShot = SDL_ConvertSurface(Screen, Screen->format, 0);
			DeathScreen();
			SDL_FreeSurface(ScreenShot);
		}

		DeathRect.w = 0;
		Character.Health = 100;
		Character.Reset = false;
		Character.Render = true;
		Character.WorldX = XSpawn;
		Character.WorldY = YSpawn;
		LevelProgress = 0;
		EnemyVector.clear();
		SpawnVector.clear();
		ProjectileVector.clear();
		PickupVector.clear();
		EnemyProjectileVector.clear();
		Enemies = 0;
	}
	DoMouse(&x, &y);
	CheckScreenFade();
	SDL_Flip(Screen);
}

void HandleEvents()
{
	SDL_PumpEvents();
	while(SDL_PollEvent(&event) && Character.NormalMovement)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE) 
			{
				SDL_SetAlpha(Fader, SDL_SRCALPHA, 126);
				ApplySurface(0, 0, Fader, Screen);
				ApplyTextCentered("Quit? Y/N", Start, White);
				SDL_Flip(Screen);
				bool Done = false;
				while (!Done)
				{
					while (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN)
					{
						if (event.key.keysym.sym == SDLK_y) QuitGame();
						else if (event.key.keysym.sym == SDLK_n || event.key.keysym.sym == SDLK_ESCAPE) Done = true;
					}
				}
			}

			else if (event.key.keysym.sym == SDLK_e && !(MouseStates & SDL_BUTTON(SDL_BUTTON_LEFT))) SwapWeapons(true);
			else if (event.key.keysym.sym == SDLK_q && !(MouseStates & SDL_BUTTON(SDL_BUTTON_LEFT))) SwapWeapons(false);

			else if (event.key.keysym.sym == SDLK_TAB)
			{
				DebugBool = !DebugBool;
				DumpDebugWindowTostderr();
				for (int i = 0; i < WEAPONS; i++) Ammo[i] = 9999;
			}
			else if (event.key.keysym.sym == SDLK_k)
			{
				for (int u = 0; u < EnemyVector.size(); u++)
				{
					EnemyVector.at(u).Health -= 999;
				}
			}
		}

		else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && CurrentSelection == 1)
		{
			Mix_PlayChannel(-1,Pistol,0);
			int Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
			float XRatio, YRatio;
			GetXYRatio(&XRatio,&YRatio,Angle,20);
			CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
		}
	}

	if (ShotTimer == 0 && MouseStates & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		int Angle;
		switch(CurrentSelection)
		{
		case 1: //Pistol
			ShotTimer = 16;
			Mix_PlayChannel(-1,Pistol,0);
			Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
			float XRatio, YRatio;
			GetXYRatio(&XRatio,&YRatio,Angle,20);
			CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
			break;

		case 2: //Shotgun
			if (Ammo[1] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[1]--;
				
				SpareStream.str("");
				SpareStream << "Resources/Sounds/Weapons/Shotgun" << rand() % 2 + 1 << ".wav";
				ShotgunFire = Mix_LoadWAV(SpareStream.str().c_str());
				Mix_PlayChannel(-1, ShotgunFire, 0);

				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
				float XRatio, YRatio;
				for (int i = -3; i <= 3; i += 1)
				{
					GetXYRatio(&XRatio,&YRatio,Angle + i, 19);
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
				}
				ShotTimer = 65;
			}
			break;

		case 3: //Machinegun
			if (Ammo[2] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[2]--;
				Mix_PlayChannel(-1,MachineGunFire,0);
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y) + (rand() % 4) - 2;
				float XRatio, YRatio;
				GetXYRatio(&XRatio,&YRatio,Angle,30);
				XRatio += Character.XVel;
				YRatio += Character.YVel;
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,2);
				ShotTimer = 6;
			}
			break;

		case 4: //Flamethrower
			if (Ammo[3] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[3]-=1;
				if (LoopingChannel == -50) LoopingChannel = Mix_FadeInChannel(-1, FlamethrowerSFX, -1, 1000);
				for (int e = 0; e < 3; e++)
				{
					Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x + (rand() % 16) - 8, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y) + (rand() % 16) - 8;
					float XRatio, YRatio;
					GetXYRatio(&XRatio,&YRatio,Angle, rand() % 10 + 25);
					XRatio += Character.XVel/2;
					YRatio += Character.YVel/2;
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,3);
				}
				ShotTimer = 1;
			}
			break;

		case 5: //Laser SMG
			if (Ammo[4] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Mix_PlayChannel(-1,Pistol,0);
				Ammo[4] -= 1;
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x + (rand() % 8) - 4, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y) + (rand() % 8) - 4;
				float XRatio, YRatio;
				GetXYRatio(&XRatio,&YRatio,Angle,20);
				for (int i = 0; i <= 10; i++)
				{
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2) + i * (-XRatio / 20), Character.WorldY + (Character.CurrentSprite->h / 2) + i * (-YRatio / 20), XRatio, YRatio, 4);
				}
				ShotTimer = 7;
			}
			break;

		case 6: //RPG
			if (Ammo[5] == 0) Mix_PlayChannel(-1, Empty, 0);
			else
			{
				Ammo[5]--;
				Mix_PlayChannel(-1, RocketSFX, 0);
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y, x, y);
				GetXYRatio(&XRatio, &YRatio, Angle, 20);
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2), Character.WorldY + (Character.CurrentSprite->h / 2), XRatio, YRatio, 5);
				ShotTimer = 50;
			}
			break;

		case 7: //Laser shotgun
			if (Ammo[6] == 0) Mix_PlayChannel(-1, Empty, 0);
			else
			{
				Mix_PlayChannel(-1, Pistol, 0);
				Ammo[6] -= 1;
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y, x, y) - 3;
				for (int p = 0; p <= 4; p++)
				{
					Angle += 2;
					float XRatio, YRatio;
					GetXYRatio(&XRatio, &YRatio, Angle, 20);
					for (int i = 0; i <= 10; i++)
					{
						CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2) + i * (-XRatio / 20), Character.WorldY + (Character.CurrentSprite->h / 2) + i * (-YRatio / 20), XRatio, YRatio, 4);
					}
					ShotTimer = 12;
				}
			}
			break;

		case 8: //Grenade machinegun
			if (Ammo[7] == 0) Mix_PlayChannel(-1, Empty, 0);
			else
			{
				Ammo[7]--;
				Mix_PlayChannel(-1, Pistol, 0);
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y, x, y);
				GetXYRatio(&XRatio, &YRatio, Angle, 30);
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2), Character.WorldY + (Character.CurrentSprite->h / 2), XRatio, YRatio, 6);
				ShotTimer = 20;
			}
			break;

		case 9: //Minigun
			if (Ammo[8] == 0) Mix_PlayChannel(-1, Empty, 0);
			else
			{
				if (LoopingChannel == -50) LoopingChannel = Mix_PlayChannel(-1, MinigunSFX, -1);
				Ammo[8]--;
				Shake = true;
				Mag = 6;
				Dur = 2;
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y, x, y) + (rand() % 6 - 3);
				GetXYRatio(&XRatio, &YRatio, Angle, 20);
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2), Character.WorldY + (Character.CurrentSprite->h / 2), XRatio, YRatio, 1);
				ShotTimer = 1;
			}
			break;
		}
	}

	else if (LoopingChannel != -50 && !(MouseStates & SDL_BUTTON(SDL_BUTTON_LEFT)))
	{
		if (CurrentSelection == 9) 
		{
			Mix_PlayChannel(-1, MinigunSpindown, 0);
			Mix_HaltChannel(LoopingChannel);
		}

		else Mix_FadeOutChannel(LoopingChannel, 200);

		LoopingChannel = -50;
	}
}

void NextLevel(int SpawnX, int SpawnY, std::string Level, const char *Text = NULL, bool Fade = true)
{
	if (Fade)
	{
		ScreenAlpha = 1;
		
		while (ScreenAlpha < 250)
		{
			DoThings();
			SDL_Delay(8);
		}
	}

	EnemyVector.clear();
	SpawnVector.clear();
	ProjectileVector.clear();
	PickupVector.clear();
	EnemyProjectileVector.clear();
	ObjectVector.clear();

	if (!LoadLevel(Level))
	{
		DebugWindow("Couldn't load level!");
		DumpDebugWindowTostderr();
		State = QUIT;
	}

	Character.Lives += 2;
	Character.Health = 100;
	XSpawn = SpawnX;
	YSpawn = SpawnY;
	Character.WorldX = SpawnX;
	Character.WorldY = SpawnY;
	LevelProgress = 0;
	LevelColour = 0x000000;
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelFinished = false;
	Laser = false;
	Boss = false;
	Enemies = 0;

	if (Text != NULL) Terminal(Text);
}

void Game()
{
	ClearScreen();

	Character.CurrentSprite = PlayerNormal;

	DeathRect.x = 0;
	DeathRect.y = 0;
	DeathRect.w = 0;
	DeathRect.h = 300;

	int Weapons = 0;
	Timer FPSTimer;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	XSpawn = 1850;
	YSpawn = 3400;
	Character.WorldX = 1850;
	Character.WorldY = 3400;
	HealthRect.x = 115;
	HealthRect.h = 20;
	HealthRect.y = ScreenHeight - 25;
	HealthRect.w = 200;

	Terminal("Resources/Text/Booting");

	if (!LoadLevel("Resources/Levels/1")) {State = QUIT; Menu();}
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelColour = 0x000000;
	int FrameCount = 0;

	goto Jump; //Only used for debugging purposes I promise

	Camera.x = 0;
	Camera.y = 2000;

	while (!LevelFinished && State == GAME) //Level 1
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		switch (LevelProgress)
		{
		case 0:
			AddObject(850, 1000, Warden);
			AddObject(1750, 3400, PlayerNormal);
			AddObject(1250, 3350, PlayerNormal);
			AddObject(60, 3410, PlayerNormal);
			AddObject(1090, 2530, RIP);
			AddObject(250, 2550, PlayerNormal);
			BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Beat1.ogg");
			Camera.LevelHeight = LevelHeight;
			LevelProgress = 1;
			FrameCount = 0;
			break;

		case 1:
			if (Character.WorldY < 1650)
			{
				LevelProgress = 2;
				Laser = true;
				LaserSpeed = 0;
				LaserY = 2010;

				Camera.TargetX = 0;
				Camera.TargetY = 700;

				Update = false;
				Camera.LevelHeight = 2020;
				Character.NormalMovement = false;
			}
			break;

		case 2:
			FrameCount++;
			
			if (FrameCount % 70 == 0)
			{
				CreateDebris(6, 6, 850 + (rand() % 2) * 300, 1000 + (rand() % 2) * 300, rand() % 20 - 10, rand() % 20 - 10, 0xFFFFFF);
				Shake = true;
				Mag = 10;
				Dur = 20;
				Mix_PlayChannel(-1, Impact, 0);
			}

			if (FrameCount == 270)
			{
				Update = true;
				LevelProgress = 3;

				Enemy Temp(850,1000,13);

				Temp.Health = 5500;
				Temp.CollisionRect.w = 300;
				Temp.CollisionRect.h = 300;
				Temp.Frame = 0;
				Temp.Frametime = 1;
				AngleOffset = 0;

				Temp.Moving = false;
				Character.NormalMovement = true;

				Mix_PlayMusic(BossTheme, -1);
				BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Beat2.ogg");

				Boss = true;
				BossName = "W.A.R DEN:";
				Multiplier = (float)(ScreenWidth-50)/Temp.Health;
				EnemyVector.push_back(Temp);
				ObjectVector.erase(ObjectVector.begin());
			}
			break;

		case 3:
			if (Boss == false)
			{
				Temp1 = 1030;
				Temp2 = 0;
				Update = false;
				Character.NormalMovement = false;
				LevelProgress = 4;
				ClearProjectiles();
				Mix_HaltMusic();

				EnemyProjectile WallBreaker(4);
				WallBreaker.WorldX = 980;
				WallBreaker.WorldY = 1030;
				WallBreaker.CollisionRect.w = 40;
				WallBreaker.CollisionRect.h = 40;
				WallBreaker.XVel = 0;
				WallBreaker.YVel = -10;
				EnemyProjectileVector.push_back(WallBreaker);
			}
			break;

		case 4:
			Temp1 -= 10;
			Camera.MoveViewport(980 - ScreenWidth / 2, Temp1 - ScreenHeight / 2);

			if (Temp1 <= 20)
			{
				SpareTimer.start();
				Character.NormalMovement = false;
				LevelProgress = 5;

				LevelVector.erase(LevelVector.begin() + 1);
				Tile NewWall;
				NewWall.WorldX = 0;
				NewWall.WorldY = 0;
				NewWall.Width = 900;
				NewWall.Height = 20;
				LevelVector.push_back(NewWall);

				NewWall.WorldX = 1100;
				LevelVector.push_back(NewWall);

				CreateDebris(8, 5, 950, 10, 0, 10, 0xFFFFFFF);
				CreateDebris(8, 5, 1050, 10, 0, 10, 0xFFFFFFF);

				Shake = true;
				Mag = 30;
				Dur = 40;
			}
			break;
		case 5:
			if (SpareTimer.get_ticks() > 3000)
			{
				Update = true;
				Character.NormalMovement = true;
			}
			if (Character.WorldY < 0) LevelFinished = true;
			break;
		}

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Smash.ogg");
	NextLevel(1000, 1950, "Resources/Levels/2");
	float Vel = 0.01;

	while(LevelFinished == false && State == GAME)
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		switch (LevelProgress)
		{
		case 0:
			LevelProgress = 1;
			Pickup PICK;
			PICK.Type = 1;
			PICK.WorldX = 800;
			PICK.WorldY = 1700;
			PickupVector.push_back(PICK);
			AddObject(900, 1600, TTF_RenderText_Solid(SysSmall, "Q & E to swap weapons", White));

			for (int p = 0; p < 5; p++)
			{
				PICK.WorldX += 100;
				PickupVector.push_back(PICK);
			}
			break;

		case 1:
			if (PickupVector.size() == 0)
			{
				LevelProgress = 2;
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				ObjectVector.clear();

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(2);

				SpawnVector.push_back(2000);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(2);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(2000);
				SpawnVector.push_back(2);

				SpawnVector.push_back(2000);
				SpawnVector.push_back(2000);
				SpawnVector.push_back(2);
				SpawnEnemies(SpawnVector);
			}
			break;

		case 2:
			if (Enemies == 0)
			{
				LevelProgress = 3;
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(1000);
				SpawnVector.push_back(50);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(50);
				SpawnVector.push_back(1);

				SpawnVector.push_back(50);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(1);

				SpawnVector.push_back(50);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1150);
				SpawnVector.push_back(2000);
				SpawnVector.push_back(1);

				SpawnEnemies(SpawnVector);
			}
			break;

		case 3:
			if (Enemies == 0)
			{
				LevelProgress = 4;
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

				SpawnVector.push_back(1000);
				SpawnVector.push_back(50);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(3);

				SpawnVector.push_back(50);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}
			break;

		case 4:
			if (Enemies == 0)
			{
				LevelProgress = 6; //NOT a typo

				Pickup PushThis;
				PushThis.Type = 1;
				PushThis.WorldX = Character.WorldX - 100;
				PushThis.WorldY = Character.WorldY - 100;
				PushThis.Type = 1;
				PickupVector.push_back(PushThis);

				for (int i = 0; i <= 2; i++)
				{
					PushThis.Type = 1 + i;
					PushThis.WorldX += 100;
					PickupVector.push_back(PushThis);
				}

				SpawnVector.push_back(Character.WorldX - 300);
				SpawnVector.push_back(Character.WorldY);
				SpawnVector.push_back(4);
				SpawnEnemies(SpawnVector);
				Mix_PlayMusic(BossTheme, -1);

				Boss = true;
				BossName = "S.I.S:";
				BossHealth = 1500;
				Multiplier = (float)(ScreenWidth - 50) / BossHealth;
			}
			break;

		case 6:
			Number++;
			if (Number % 30 == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(LevelWidth - 50);
				SpawnVector.push_back((rand() % LevelHeight - 50) + 50);
				SpawnVector.push_back(5);
				SpawnEnemies(SpawnVector);
			}

			else if (Number % 25 == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(50);
				SpawnVector.push_back((rand() % LevelHeight - 50) + 50);
				SpawnVector.push_back(6);
				SpawnEnemies(SpawnVector);
			}

			if (Boss == false) LevelProgress = 7;
			break;

		case 7:
			for (int i = 0; i < EnemyVector.size(); i++) EnemyVector.at(i).Health = 0;
			EnemyProjectileVector.erase(EnemyProjectileVector.begin(), EnemyProjectileVector.end());
			LevelProgress = 8;
			break;

		case 8:
			Shake = true;
			Dur = 20;
			Mag = 20;

			Mix_HaltMusic();

			while (Temp1 < LevelWidth - 70) //Re-write this please Abdul
			{
				Camera.MoveViewport(Temp1 - ScreenWidth / 2, Temp2 - ScreenHeight / 2);
				Camera.Update();
				CheckShake();
				DoDebris(Camera.x, Camera.y, Screen);
				DoTiles(Camera.x, Camera.y);
				DoMouse(&x, &y);
				Character.XVel = 0;
				Character.YVel = 0;
				Character.Update();
				DoPickups(Camera.x, Camera.y, CharacterRect);
				DoEnemyProjectiles(Camera.x, Camera.y, CharacterRect);
				DoEnemies(Camera.x, Camera.y, 0, 0, CharacterRect, 0, 0);
				DoFloat(Camera.x, Camera.y);
				DoProjectiles(Camera.x, Camera.y);

				ApplySurface(Temp1 - Camera.x, Temp2 - Camera.y, Ship, Screen);

				Temp1 += Vel;
				if (Vel < 10) Vel *= 1.1;

				SDL_Flip(Screen);
				ClearScreen();
				SDL_Delay(10);
			}

			Shake = true;
			Dur = 20;
			Mag = 30;
			Update = false;

			CreateDebris(7, 7, LevelWidth - 20, Temp2, -10, 0, 0xFFFFFF);

			LevelVector.erase(LevelVector.begin() + 2);

			Tile PushThis;
			PushThis.Height = Temp2 - 50;
			PushThis.Width = 20;
			PushThis.WorldX = LevelWidth - 20;
			PushThis.WorldY = 0;

			LevelVector.push_back(PushThis);

			PushThis.Height = LevelHeight - (Temp2 + 50);
			PushThis.WorldY = Temp2 + 50;

			LevelVector.push_back(PushThis);

			SpareTimer.start();

			LevelProgress = 9;

			Mix_PlayChannel(0, SmashDeath, 0);
			Character.NormalMovement = false;
			break;

		case 9:
			Camera.MoveViewport(Temp1 - ScreenWidth / 2, Temp2 - ScreenHeight / 2);
			Camera.Update();
			if (SpareTimer.get_ticks() > 1200)
			{
				LevelProgress = 10;
				Update = true;
				Character.NormalMovement = true;
			}
			break;

		case 10:
			if (Character.WorldX > LevelWidth - Character.CurrentSprite->w) LevelFinished = true;
		}
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	NextLevel(1000, 4500, "Resources/Levels/3", "Resources/Text/Attempt1");

	while(LevelFinished == false && State == GAME)
	{
		FPSTimer.start();
		DoThings();

		HandleEvents();

		switch(LevelProgress)
		{
		case 0:
			LaserY = 5000;
			LaserSpeed = 2.4;
			LevelProgress = 1;
			SpareTimer.start();
			Character.NormalMovement = false;
			break;

		case 1:
			if (SpareTimer.get_ticks() > 2000)
			{
				LevelProgress = 2;
				Character.NormalMovement = true;
				Laser = true;
			}
			break;

		case 2:
			if (static_cast<int>(LaserY) % 160 == 2)
			{
				SpawnVector.push_back((rand() % LevelWidth) + 1);
				SpawnVector.push_back(2400);
				SpawnVector.push_back(1);
				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}

			if (Character.WorldY < 2500) LevelProgress = 3;
			break;
		case 3:
			if (static_cast<int>(LaserY) % 230 == 2)
			{
				SpawnVector.push_back((rand() % LevelWidth) + 1);
				SpawnVector.push_back(300);
				SpawnVector.push_back(1);
				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}

			if (static_cast<int>(LaserY) % 250 == 4)
			{
				SpawnVector.push_back((rand() % LevelWidth) + 1);
				SpawnVector.push_back(300);
				SpawnVector.push_back(3);
				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}

			if (Character.WorldY < 0) LevelFinished = true;
		};
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	NextLevel(1000, 9950, "Resources/Levels/4");

	LaserY = 6000;

	int Numero = 0;

	EnemyProjectile Big(4);
	Big.CollisionRect.w = 60;
	Big.CollisionRect.h = 60;
	Big.XVel = -7;
	Big.YVel = 0;
	Big.WorldX = 1919;
	Big.WorldY = 0;
	Big.Damage = 100;

	EnemyProjectile Big2(4);
	Big2.CollisionRect.w = 60;
	Big2.CollisionRect.h = 60;
	Big2.XVel = 0;
	Big2.YVel = 7;
	Big2.WorldX = 0;
	Big2.WorldY = 30;
	Big2.Damage = 100;

	EnemyProjectile Small(3);
	Small.CollisionRect.w = 30;
	Small.CollisionRect.h = 30;
	Small.XVel = 7;
	Small.YVel = 0;
	Small.WorldX = 21;
	Small.WorldY = 0;
	Small.Damage = 30;

	EnemyProjectile Small2(3);
	Small2.CollisionRect.w = 30;
	Small2.CollisionRect.h = 30;
	Small2.XVel = 0;
	Small2.YVel = -11;
	Small2.WorldX = 21;
	Small2.WorldY = LevelHeight - 40;
	Small2.Damage = 30;

	EnemyProjectile Sine(9);
	Sine.CollisionRect.w = 20;
	Sine.CollisionRect.h = 20;
	Sine.WorldX = 0;
	Sine.WorldY = 30;
	Sine.Spare1 = 0;
	Sine.Spare2 = 30;
	Sine.YVel = 9;
	Sine.Frame = 180;
	Sine.Frametime = 0;
	Sine.Damage = 30;
	Sine.Wave = true;

	while(LevelFinished == false && State == GAME)
	{
		FPSTimer.start();
		DoThings();
		HandleEvents();
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());

		switch (LevelProgress)
		{
		case 0:
			SpareTimer.start();
			Laser = false;
			Camera.y = 8000;
			LevelProgress = 1;
			Numero = 0;
			Character.NormalMovement = false;
			break;
		case 1:
			if (SpareTimer.get_ticks() > 500)
			{
				LevelProgress = 2;
				Character.NormalMovement = true;
			}

		case 2:
			if (Character.WorldY < 9800)
			{
				Laser = true;
				LaserSpeed = 1.5;
				LaserY = 10000;
				LevelProgress = 3;

				Pickup UpPick;
				UpPick.Type = 1;
				UpPick.WorldY = 9600;

				for (int inc = 0; inc < 5; inc++)
				{
					UpPick.WorldX = 800 + 100 * inc;
					PickupVector.push_back(UpPick);
				}
			}
			break;

		case 3:
			Numero++;

			if (Numero % 5 == 0)
			{
				Small.WorldY = rand() % 6900 + 3021;
				EnemyProjectileVector.push_back(Small);
			}

			if (Numero % 10 == 0)
			{
				Big.WorldY = rand() % 6900 + 3021;
				EnemyProjectileVector.push_back(Big);
			}

			if (Numero % 20 == 0)
			{
				Sine.WorldX = rand() % (LevelWidth - 60) + 20;
				Sine.Spare1 = Sine.WorldX;
				EnemyProjectileVector.push_back(Sine);

				Small2.WorldX = Sine.WorldX;
				EnemyProjectileVector.push_back(Small2);
			}

			if (Numero % 30 == 0)
			{
				Big2.WorldX = rand() % (LevelWidth - 140) + 20;
				EnemyProjectileVector.push_back(Big2);
			}

			if (Character.WorldY < 0) LevelFinished = true;

			break;
		};
	}

	Pickup pukciP;
	pukciP.Type = 4;

	NextLevel(1100, 1100, "Resources/Levels/2", "Resources/Text/Attempt2");

	SDL_Rect *RenderRect = &TeleportClips[0];

	while (LevelFinished == false && State == GAME)
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		switch (LevelProgress)
		{
		case 0:
			pukciP.WorldX = 800;
			pukciP.WorldY = 800;
			PickupVector.push_back(pukciP);
			BossTheme = Mix_LoadMUS("Resources/Sounds/Music/One.ogg");

			for (int i = 0; i < 3; i++)
			{
				if (i % 2 == 0) pukciP.WorldX = 2000 - pukciP.WorldX;
				pukciP.WorldY = 2000 - pukciP.WorldY;
				PickupVector.push_back(pukciP);
			}

			LevelProgress = 1;

			break;
		case 1:
			if (PickupVector.size() == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(1000);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(8);
				SpawnEnemies(SpawnVector);
				LevelProgress = 2;
			}
			break;
		case 2:
			if (Enemies == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(300);
				SpawnVector.push_back(300);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(300);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(300);
				SpawnVector.push_back(8);

				SpawnVector.push_back(300);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(8);
				SpawnEnemies(SpawnVector);
				LevelProgress = 3;
			}
			break;

		case 3:
			if (Enemies == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(300);
				SpawnVector.push_back(300);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(300);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(300);
				SpawnVector.push_back(1);

				SpawnVector.push_back(300);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(1);
				SpawnEnemies(SpawnVector);

				LevelProgress = 4;
			}
			break;

		case 4:
		{
			if (Enemies == 0)
			{
				if (LoopingChannel != -50) Mix_HaltChannel(LoopingChannel);
				CutsceneFinished = false;
				int Stage = 0;
				int Integer = 0;

				SDL_Rect One;
				SDL_Rect Two;
				SDL_Rect Three;
				SDL_Rect Four;
				SDL_Rect Five;

				Camera.MoveViewport(Character.WorldX - ScreenWidth / 2, (Character.WorldY + 300) - ScreenHeight / 2);
				Character.NormalMovement = false;

				while (!CutsceneFinished)
				{
					FPSTimer.start();

					Camera.Update();
					CheckShake();
					DoDebris(Camera.x, Camera.y, Screen);
					DoTiles(Camera.x, Camera.y);
					DoMouse(&x, &y);
					Damaged = false;
					Character.Update();
					//CheckText();
					DoPickups(Camera.x, Camera.y, CharacterRect);
					Character.InvunFrames = 100;
					DoEnemyProjectiles(Camera.x, Camera.y, CharacterRect);
					DoEnemies(Camera.x, Camera.y, 0, 0, CharacterRect, 0, 0);
					DoFloat(Camera.x, Camera.y);
					DoProjectiles(Camera.x, Camera.y);

					switch (Stage)
					{
					case 0:
						Integer++;
						ApplySurface(Character.WorldX - 15 - Camera.x, (Character.WorldY + 302) - Camera.y, TeleportSheet, Screen, &TeleportClips[Integer % 2]);
						if (Integer == 61) Stage = 1;
						break;
					case 1:
						Update = false;

						Integer = 0;

						Five.x = Character.WorldX - Camera.x;
						Five.y = (Character.WorldY + 300) - Camera.y;
						Five.w = 10;
						Five.h = 10;

						One.x = Five.x - 15;
						One.y = Five.y - 2120;
						One.w = 40;
						One.h = 100;

						Two.x = Five.x + 2030;
						Two.y = Five.y - 15;
						Two.w = 100;
						Two.h = 40;

						Three.x = One.x;
						Three.y = Five.y + 2030;
						Three.w = 40;
						Three.h = 100;

						Four.x = Five.x - 2120;
						Four.y = Two.y;
						Four.w = 100;
						Four.h = 40;

						Stage = 2;
					case 2:
						Integer++;

						*RenderRect = One;
						SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
						*RenderRect = Two;
						SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
						*RenderRect = Three;
						SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
						*RenderRect = Four;
						SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
						*RenderRect = Five;
						SDL_FillRect(Screen, RenderRect, 0xC0C0C0);


						One.y += 10;
						Two.x -= 10;
						Three.y -= 10;
						Four.x += 10;

						if (Integer == 200)
						{
							CutsceneFinished = true;

							SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
							SpawnVector.push_back(Character.WorldX - 15);
							SpawnVector.push_back(Character.WorldY + 180);
							SpawnVector.push_back(9);
							SpawnEnemies(SpawnVector);
							EnemyVector.at(EnemyVector.size() - 1).Frame = 3;
							EnemyVector.at(EnemyVector.size() - 1).Frametime = 15;

							Shake = true;
							Dur = 45;
							Mag = 20;
						}
						break;
					}
					SDL_Flip(Screen);
					ClearScreen();
					if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
				}
				LevelProgress = 5;
				Character.NormalMovement = true;
				Update = true;

				BossName = "M.A.R.S:";
				BossHealth = 14000;
				Multiplier = (float)(ScreenWidth - 50) / BossHealth;
				Boss = true;

				Mix_PlayMusic(BossTheme, -1);

				pukciP.WorldX = Character.WorldX - 200;
				pukciP.WorldY = Character.WorldY;
				PickupVector.push_back(pukciP);

				for (int i = 0; i < 5; i++)
				{
					pukciP.WorldX += 100;
					PickupVector.push_back(pukciP);
				}
			}
			break;
		}

		case 5:
		{
			if (Boss == false)
			{
				LevelProgress = 6;
				EnemyVector.erase(EnemyVector.begin(), EnemyVector.end());
			}
			break;
		}

		case 6:
		{
			Update = false;

			SDL_Rect Five;
			Five.x = Temp1;
			Five.y = Temp2;
			Five.w = 10;
			Five.h = 10;

			SpareTimer.stop();
			CutsceneFinished = false;
			Mix_HaltMusic();
			if (LoopingChannel != -50) Mix_HaltChannel(LoopingChannel);

			while (!CutsceneFinished)
			{
				FPSTimer.start();
				Camera.Update();
				CheckShake();
				DoTiles(Camera.x, Camera.y);
				DoMouse(&x, &y);
				Character.XVel = 0;
				Character.YVel = 0;
				Damaged = false;
				Character.Update();
				DoPickups(Camera.x, Camera.y, CharacterRect);
				Character.InvunFrames = 100;
				DoEnemyProjectiles(Camera.x, Camera.y, CharacterRect);
				DoEnemies(Camera.x, Camera.y, 0, 0, CharacterRect, 0, 0);
				DoFloat(Camera.x, Camera.y);
				DoProjectiles(Camera.x, Camera.y);
				DoDebris(Camera.x, Camera.y, Screen);
				Camera.MoveViewport(Temp1 - ScreenWidth / 2, Temp2 - ScreenHeight / 2);

				if (BoxXVel >= 0) BoxXVel += 0.1;
				else BoxXVel -= 0.1;
				if (BoxYVel >= 0) BoxYVel += 0.1;
				else BoxYVel -= 0.1;

				if (Temp1 + BoxXVel + 20 > LevelWidth - 20 || Temp1 + BoxXVel < 20)
				{
					BoxXVel *= -0.9;
					Shake = true;
					Mag = 20;
					Dur = 30;
				}

				if (Temp2 + BoxYVel + 20 > LevelHeight - 20 || Temp2 + BoxYVel < 20)
				{
					BoxYVel *= -0.9;
					Shake = true;
					Mag = 20;
					Dur = 30;

					if (BoxYVel >= 0)
					{
						TopBounces--;
						if (TopBounces == -1)
						{
							LevelVector.erase(LevelVector.begin() + 1);
							Tile TempTile;
							TempTile.Height = 20;
							TempTile.WorldX = 0;
							TempTile.WorldY = 0;
							TempTile.Width = Temp1 - 50;
							LevelVector.push_back(TempTile);

							TempTile.WorldX = Temp1 + 50;
							TempTile.Width = LevelWidth - (Temp1 + 50);
							LevelVector.push_back(TempTile);

							CreateDebris(6, 8, Temp1, 20, 0, 15, 0xFFFFFF);
							CreateDebris(5, 10, Temp1, 20, 0, 15, 0xFF0000);

							Shake = true;
							Mag = 30;
							Dur = 50;

							SpareTimer.start();
						}
					}
				}

				if (TopBounces >= 0)
				{
					Temp1 += BoxXVel;
					Temp2 += BoxYVel;
					Five.x = Temp1 - Camera.x;
					Five.y = Temp2 - Camera.y;
					Five.w = 20;
					Five.h = 20;
					SDL_FillRect(Screen, &Five, 0xFF0000);
				}

				if (SpareTimer.is_started() && SpareTimer.get_ticks() > 2000)
				{
					CutsceneFinished = true;
					LevelProgress = 7;
				}

				SDL_Flip(Screen);
				ClearScreen();
				if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
			}
			Update = true;
			break;
		}

		case 7:
		{
			if (Character.WorldY <= 50 && InBetween(Temp1 - 100, Character.WorldX, Temp1 + 100))
			{
				Temp1 = Character.WorldX;
				LevelFinished = true;
			}
			}
		}

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	LoadLevel("Resources/Levels/2");
	Character.Lives += 3;
	Character.Health = 100;
	XSpawn = Temp1;
	YSpawn = 30;
	LevelProgress = 0;
	LevelColour = 0x000000;
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelFinished = false;
	Laser = false;
	Boss = false;
	Enemies = 0;


	while (!LevelFinished && State == GAME)
	{
		FPSTimer.start();
		DoThings();
		HandleEvents();

		switch (LevelProgress)
		{
		case 0:
			Pickup PICK;
			PICK.Type = 4;
			PICK.WorldX = Temp1 - 200;
			PICK.WorldY = 200;
			PickupVector.push_back(PICK);

			for (int p = 0; p < 5; p++)
			{
				PICK.WorldX += 100;
				PickupVector.push_back(PICK);
			}

			SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

			BossName = "Enemies:";
			BossHealth = 90;
			Multiplier = (float)(ScreenWidth - 50) / 90;
			Boss = true;
			Temp2 = 0;
			LevelProgress = 1;

		case 1:
			Temp2++;
			if (Temp2 % 40 == 0 && Enemies < 100)
			{
				BossHealth--;

				int RandomX = rand() % 2000;
				int RandomY = rand() % 2000;

				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				
				SpawnVector.push_back(RandomX);
				SpawnVector.push_back(RandomY);
				SpawnVector.push_back(10);

				if (BossHealth < 60)
				{
					SpawnVector.push_back(RandomX + rand() % 1000 - 500);
					SpawnVector.push_back(RandomY + rand() % 1000 - 500);
					SpawnVector.push_back(1);
				}

				if (BossHealth < 15)
				{
					SpawnVector.push_back(RandomX + rand() % 400 - 200);
					SpawnVector.push_back(RandomY + rand() % 400 - 200);
					SpawnVector.push_back(3);
				}

				if (BossHealth < 5)
				{
					SpawnVector.push_back(RandomX + rand() % 400 - 200);
					SpawnVector.push_back(RandomY + rand() % 400 - 200);
					SpawnVector.push_back(2);
				}

				if (BossHealth < 0)
				{
					Boss = false;
					LevelProgress = 2;
				}

				SpawnEnemies(SpawnVector);
			}
			break;

		case 2:
			if (Enemies == 0)
			{
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
				SpawnVector.push_back(950);
				SpawnVector.push_back(950);
				SpawnVector.push_back(14);
				SpawnEnemies(SpawnVector);
				LevelProgress = 3;
			}
			break;

		case 3:
			if (Enemies == 0) LevelFinished = true;
		}

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	NextLevel(500, 5860, "Resources/Levels/5");

	while(!LevelFinished && State == GAME)
	{
		FPSTimer.start();

		DoThings();
		HandleEvents();

		EnemyProjectile BetterRun(8);
		BetterRun.CollisionRect.w = 4;
		BetterRun.CollisionRect.h = 4;
		BetterRun.Friction = true;
		BetterRun.Damage = 10;

		switch (LevelProgress)
		{
			case 0:
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

				SpawnVector.push_back(400);
				SpawnVector.push_back(4420);
				SpawnVector.push_back(11);

				SpawnVector.push_back(800);
				SpawnVector.push_back(4420);
				SpawnVector.push_back(11);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(4420);
				SpawnVector.push_back(11);

				SpawnVector.push_back(1200);
				SpawnVector.push_back(4420);
				SpawnVector.push_back(11);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(4420);
				SpawnVector.push_back(11);

				Camera.y = LevelHeight - ScreenHeight;

				SpawnEnemies(SpawnVector);
				Temp1 = 30;
				Temp2 = 5824;

				Pickup PushThis;
				PushThis.Type = 1;
				PushThis.WorldX = 200;
				PushThis.WorldY = 4500;
				PickupVector.push_back(PushThis);

				for (int i = 1; PushThis.Type != 6; i++)
				{
					PushThis.WorldX += 70;
					PickupVector.push_back(PushThis);
					if (i % 3 == 0) PushThis.Type++;
				}

				LevelProgress = 1;

			case 1:
				if (Temp1 < Character.WorldX) Temp1 += 5;
				else Temp1 -= 5;

				Temp2 = Character.WorldY - (static_cast<int>(Character.WorldY) % 200) + 22;

				if (Temp2 != Temp3)
				{
					if (Character.WorldX < 1000) Temp1 = 0;
					else Temp1 = 1980;
				}

				Temp3 = Temp2;

				BetterRun.WorldX = Temp1 + rand() % 12 - 6;
				BetterRun.WorldY = Temp2;
				BetterRun.XVel = rand() % 6 - 3;
				BetterRun.YVel = 10 + BetterRun.XVel;
				EnemyProjectileVector.push_back(BetterRun);
				BetterRun.WorldX += rand() % 4 - 2;
				BetterRun.XVel = rand() % 6 - 3;
				BetterRun.YVel = 10 + BetterRun.XVel;
				EnemyProjectileVector.push_back(BetterRun);
				if (Character.WorldY < 4600) LevelProgress = 2;
				break;

			case 2:
				if (Character.WorldY < 4376)
				{
					Laser = true;
					LaserSpeed = 0;
					LaserY = 4400;
					EnemyVector.erase(EnemyVector.begin(), EnemyVector.end());

					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
					SpawnVector.push_back(900);
					SpawnVector.push_back(3800);
					SpawnVector.push_back(12);
					SpawnEnemies(SpawnVector);
					BlockHeight = 3000;
					LevelProgress = 3;
				}
				break;

			case 3:
				if (Enemies == 0)
				{
					LaserSpeed = 1.9;
					LevelProgress = 4;

					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					SpawnVector.push_back(400);
					SpawnVector.push_back(2020);
					SpawnVector.push_back(11);

					SpawnVector.push_back(800);
					SpawnVector.push_back(2020);
					SpawnVector.push_back(11);

					SpawnVector.push_back(1200);
					SpawnVector.push_back(2020);
					SpawnVector.push_back(11);

					SpawnVector.push_back(1600);
					SpawnVector.push_back(2020);
					SpawnVector.push_back(11);

					SpawnEnemies(SpawnVector);
					Pickup PushThis;
					PushThis.WorldY = 2050;

					for (int v = 0; v < 3; v++)
					{
						PushThis.WorldX = 1200;
						PushThis.Type = 1;
						for (int c = 0; c <= 5; c++)
						{
							PickupVector.push_back(PushThis);
							PushThis.WorldX += 70;
							PushThis.Type += 1;
						}
						PushThis.WorldY += 70;
					}
					BlockHeight = 0;
				}
				break;

			case 4:
				if (Character.WorldY < 1900)
				{
					LaserY = 1950;
					LaserSpeed = 0;
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
					SpawnVector.push_back(500);
					SpawnVector.push_back(1100);
					SpawnVector.push_back(12);
					SpawnVector.push_back(1500);
					SpawnVector.push_back(1100);
					SpawnVector.push_back(12);
					SpawnEnemies(SpawnVector);

					LevelProgress = 5;
				}
				break;

			case 5:
				BlockHeight = 200;

				if (Enemies == 0)
				{
					LevelProgress = 6;
					LaserSpeed = 1.5;
					BlockHeight = 0;
				}
				break;

			case 6:
				if (Character.WorldX > 2000) LevelFinished = true;
				break;
			}
			if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
		}

		NextLevel(1000, 1000, "Resources/Levels/2");

		while (!LevelFinished && State == GAME)
		{
			FPSTimer.start();

			DoThings();
			HandleEvents();

			switch (LevelProgress)
			{
			case 0:
				SpawnVector.clear();
				SpawnVector.push_back(30);
				SpawnVector.push_back(30);
				SpawnVector.push_back(3);
				SpawnEnemies(SpawnVector);
				LevelProgress = 1;

			case 1:
				if (Enemies == 0)
				{
					if (Character.WorldX - 400 < 20) Temp1 = 20;
					else Temp1 = Character.WorldX - 400;

					if (Character.WorldY - 400 < 20) Temp2 = 20;
					else Temp2 = Character.WorldY - 400;
				
					Pickup PICK;
					PICK.Type = 4;
					PICK.WorldX = Temp1;
					PICK.WorldY = Temp2;

					for (int j = 0; j <= 4; j++)
					{
						for (int p = 0; p < 5; p++)
						{
							PICK.WorldX += 100;
							PickupVector.push_back(PICK);
						}
						PICK.WorldX = Temp1;
						if (PICK.Type == 4) PICK.Type = 6;
						else PICK.Type++;
						PICK.WorldY += 100;
					}

					BossName = "Turrets:";
					BossHealth = 30;
					Multiplier = (float)(ScreenWidth - 50) / 30;
					Boss = true;
					Temp2 = 0;
					LevelProgress = 2;
				}
				break;

			case 2:
				Temp2++;
				if (Temp2 == 200 && BossHealth > 0)
				{
					BossHealth--;
					Temp2 = 0;
					SpawnVector.clear();
					SpawnVector.push_back(rand() % 1900 + 20);
					SpawnVector.push_back(rand() % 1900 + 20);
					SpawnVector.push_back(15);

					if (BossHealth < 15)
					{
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(15);
					}

					if (BossHealth < 10)
					{
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(1);
					}

					if (BossHealth < 5)
					{
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(rand() % 1900 + 20);
						SpawnVector.push_back(2);
					}

					SpawnEnemies(SpawnVector);
					SpawnVector.clear();
				}

				else if (BossHealth <= 0)
				{
					LevelProgress = 3;
					Boss = false;
				}
				break;

			case 3:
				if (Enemies == 0)
				{
					CreateAsteroid((1 - (int)Character.WorldX / 1000) * 1000, (1 - (int)Character.WorldY % 1000) * 1000, 400, 5, 5);
					LevelProgress = 4;
				}
				break;

			case 4:
				if (AsteroidVector.size() == 0) LevelFinished = true;
				break;
			};
			if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
		}

	Jump:

		NextLevel(1850, 3400, "Resources/Levels/1", "Resources/Text/Antimov");

		while (!LevelFinished && State == GAME)
		{
			FPSTimer.start();
			DoThings();
			HandleEvents();

			switch (LevelProgress)
			{

				case 0:
				{
					ObjectVector.clear();
					for (int p = 1; p <= WEAPONS - 1; p++) Ammo[p] = 0;
					AddObject(850, 1000, Warden);
					AddObject(1750, 3400, PlayerNormal);
					AddObject(1250, 3350, PlayerNormal);
					AddObject(60, 3410, PlayerNormal);
					AddObject(1090, 2530, RIP);
					AddObject(250, 2550, PlayerNormal);
					Camera.LevelHeight = LevelHeight;
					LevelProgress = 1;
					FrameCount = 0;
					break;
				}

				case 1:
				{
					if (Character.WorldY < 1750)
					{
						LevelProgress = 2;
						Laser = true;
						LaserSpeed = 0;
						LaserY = 2010;

						Camera.TargetX = 0;
						Camera.TargetY = 700;

						Update = false;
						Camera.LevelHeight = 2020;
						Character.NormalMovement = false;
					}
					break;
				}

				case 2:
				{
					FrameCount++;

					if (FrameCount % 70 == 0)
					{
						CreateDebris(6, 6, 850 + (rand() % 2) * 300, 1000 + (rand() % 2) * 300, rand() % 20 - 10, rand() % 20 - 10, 0xFFFFFF);
						Shake = true;
						Mag = 10;
						Dur = 20;
						Mix_PlayChannel(-1, Impact, 0);
					}

					if (FrameCount == 270)
					{
						Update = true;
						LevelProgress = 3;

						Enemy Temp(850, 1000, 16);

						Temp.Health = 3000;
						Temp.CollisionRect.w = 300;
						Temp.CollisionRect.h = 300;
						Temp.Frame = 0;
						Temp.Frametime = 1;
						AngleOffset = 0;

						DeathRect.w = 300;
						DeathRect.h = 300;
						DeathRect.x = Character.WorldX - 150;
						DeathRect.y = Character.WorldY - 150;

						Temp.Moving = false;
						Character.NormalMovement = true;

						Boss = true;
						BossName = "W.A.R DEN:";
						Multiplier = (float)(ScreenWidth - 50) / Temp.Health;
						EnemyVector.push_back(Temp);
						ObjectVector.erase(ObjectVector.begin());
					}
					break;
				}

				case 3:
				{
					if (!Boss)
					{
						LevelProgress = 4;
						EnemyProjectileVector.clear();

						Mix_HaltMusic();
						BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Smash.ogg");
						Mix_PlayMusic(BossTheme, -1);

						DeathRect.w = 0;

						Pickup PICK;
						PICK.Type = 1;
						PICK.WorldX = Temp1 - 100;
						PICK.WorldY = Temp2 - 100;
						PickupVector.push_back(PICK);

						for (int q = 0; q < 2; q++)
						{
							for (int p = 0; p < 5; p++)
							{
								PICK.WorldX += 100;
								PickupVector.push_back(PICK);
							}
							PICK.WorldY++;
						}

						SpawnVector.clear();
						SpawnVector.push_back(Temp1);
						SpawnVector.push_back(Temp2);
						SpawnVector.push_back(17);
						SpawnEnemies(SpawnVector);

						Boss = true;
						Number = 0;
					}
					break;
				}

				case 4:
				{
					Number++;
					if (Number % 30 == 0)
					{
						SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
						SpawnVector.push_back(50);
						SpawnVector.push_back((rand() % Camera.LevelHeight - 50) + 50);
						SpawnVector.push_back(6);
						SpawnEnemies(SpawnVector);
					}

					else if (Number % 40 == 0)
					{
						SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
						SpawnVector.push_back(LevelWidth - 50);
						SpawnVector.push_back((rand() % Camera.LevelHeight - 50) + 50);
						SpawnVector.push_back(5);
						SpawnEnemies(SpawnVector);
					}

					if (Boss == false) LevelProgress = 5;
					break;
				}

				case 5:
				{
					for (int i = 0; i < EnemyVector.size(); i++)
					{
						EnemyVector.at(i).Health = 0;
					}

					CutsceneFinished = false;
					Mix_HaltMusic();
					BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Four.ogg");
					Update = false;
					int Stage = 0;
					int Integer = 0;

					SDL_Rect *RenderRect = &TeleportClips[0];
					SDL_Rect One;
					SDL_Rect Two;
					SDL_Rect Three;
					SDL_Rect Four;
					SDL_Rect Five;

					Camera.MoveViewport(1000 - ScreenWidth / 2, 1300 - ScreenHeight / 2);

					while (!CutsceneFinished)
					{
						FPSTimer.start();
						DoThings();
						HandleEvents();

						switch (Stage)
						{

							case 0:
							{
								Integer++;
								ApplySurface(985 - Camera.x, 1302 - Camera.y, TeleportSheet, Screen, &TeleportClips[Integer % 2]);
								if (Integer == 61) Stage = 1;
								break;
							}

							case 1:
							{
								Update = false;

								Integer = 0;

								Five.x = 1000 - Camera.x;
								Five.y = 1300 - Camera.y;
								Five.w = 10;
								Five.h = 10;

								One.x = Five.x - 15;
								One.y = Five.y - 2120;
								One.w = 40;
								One.h = 100;

								Two.x = Five.x + 2030;
								Two.y = Five.y - 15;
								Two.w = 100;
								Two.h = 40;

								Three.x = One.x;
								Three.y = Five.y + 2030;
								Three.w = 40;
								Three.h = 100;

								Four.x = Five.x - 2120;
								Four.y = Two.y;
								Four.w = 100;
								Four.h = 40;

								Stage = 2;
							}

							case 2:
							{
								Integer++;

								*RenderRect = One;
								SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
								*RenderRect = Two;
								SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
								*RenderRect = Three;
								SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
								*RenderRect = Four;
								SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
								*RenderRect = Five;
								SDL_FillRect(Screen, RenderRect, 0xC0C0C0);


								One.y += 10;
								Two.x -= 10;
								Three.y -= 10;
								Four.x += 10;

								if (Integer == 200)
								{
									CutsceneFinished = true;

									Enemy RUSHMARS(985, 1180, 18);
									RUSHMARS.Health = 3000;
									RUSHMARS.CollisionRect.w = 40;
									RUSHMARS.CollisionRect.h = 100;
									EnemyVector.push_back(RUSHMARS);

									Mix_PlayMusic(BossTheme, -1);

									Shake = true;
									Dur = 45;
									Mag = 20;
									Update = true;
									Character.NormalMovement = true;
								}
							}
							break;
						}

					SDL_Flip(Screen);
					ClearScreen();
					if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
				}

				LevelProgress = 6;

				BossName = "M.A.R.S:";
				BossHealth = 3000;
				Multiplier = (float)(ScreenWidth - 50) / BossHealth;
				Boss = true;

				pukciP.WorldX = Character.WorldX - 200;
				pukciP.WorldY = Character.WorldY;
				pukciP.Type = 3;
				PickupVector.push_back(pukciP);

				for (int i = 0; i < 5; i++)
				{
					pukciP.WorldX += 100;
					PickupVector.push_back(pukciP);
				}

				pukciP.WorldY += 100;
				pukciP.Type = 4;
				pukciP.WorldX = Character.WorldX - 200;

				for (int i = 0; i < 5; i++)
				{
					pukciP.WorldX += 100;
					PickupVector.push_back(pukciP);
				}
				break;
			}

			case 6:
			{
				if (!Boss)
				{
					LevelFinished = true;
					Temp1 = Character.WorldX;
					Temp2 = Character.WorldY;
					Mix_HaltMusic();
				}
				break;
			}
		}
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
	}

	OrbitX = 1000;
	OrbitY = 20;
	NextLevel(Temp1, Temp2, "Resources/Levels/1", NULL, false);
	Camera.LevelHeight = 2020;

	while (!LevelFinished && State == GAME)
	{
		FPSTimer.start();
		DoThings();
		HandleEvents();

		switch (LevelProgress)
		{
			case 0:
			{
				LevelProgress = 1;
				LaserY = 2010;
				LaserSpeed = 0;
				Laser = true;

				pukciP.WorldX = Character.WorldX - 200;
				pukciP.WorldY = Character.WorldY;
				pukciP.Type = 6;

				for (int u = 0; u <= 3; u++)
				{
					for (int i = 0; i < 5; i++)
					{
						pukciP.WorldX += 100;
						PickupVector.push_back(pukciP);
					}
					pukciP.WorldX = Character.WorldX - 200;
					pukciP.WorldY += 100;
				}

				Boss = true;
				BossName = "ADXERS:";
				Multiplier = (float)(ScreenWidth - 50) / 6000;

				SpawnVector.clear();
				SpawnVector.push_back(500);
				SpawnVector.push_back(500);
				SpawnVector.push_back(19);
				SpawnVector.push_back(1500);
				SpawnVector.push_back(500);
				SpawnVector.push_back(20);
				SpawnEnemies(SpawnVector);


				break;
			}

			case 1:
			{
				Boss = true;
				BossHealth = 0;
				for (int i = 0; i < EnemyVector.size(); i++) BossHealth += EnemyVector.at(i).Health;
				if (BossHealth <= 0)
				{
					LevelProgress = 2;
					DeathRect.w = 600;
					DeathRect.h = 600;

					if (Character.WorldX - 300 < 20) DeathRect.x = 20;
					else DeathRect.x = Character.WorldX - 300;
					if (Character.WorldY - 300 < 20) DeathRect.y = 20;
					else DeathRect.y = Character.WorldY - 300;

					DeathRect.x = DeathRect.x - DeathRect.x % 5;
					DeathRect.y = DeathRect.y - DeathRect.y % 5;
					
					BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Wan.ogg");
					Mix_PlayMusic(BossTheme, -1);

					SpareTimer.start();
				}
				break;
			}

			case 2:
			{
				if (SpareTimer.get_ticks() > 3000)
				{
					if (DeathRect.x < 850) DeathRect.x += 5;
					else if (DeathRect.x != 850) DeathRect.x -= 5;
					if (DeathRect.y < 400) DeathRect.y += 5;
					else if (DeathRect.y != 400) DeathRect.y -= 5;

					if (DeathRect.w > 400) 
					{
						DeathRect.w--;
						DeathRect.h--;
					}

				}

				if (DeathRect.x == 850 && DeathRect.y == 400 && DeathRect.w == 400) 
				{
					SpawnVector.clear();
					SpawnVector.push_back(985);
					SpawnVector.push_back(-110);
					SpawnVector.push_back(21);
					SpawnEnemies(SpawnVector);

					pukciP.WorldX = DeathRect.x;
					pukciP.WorldY = DeathRect.y;
					pukciP.Type = 9;

					for (int u = 0; u <= 3; u++)
					{
						for (int i = 0; i < 5; i++)
						{
							pukciP.WorldX += 100;
							PickupVector.push_back(pukciP);
						}
						pukciP.WorldX = DeathRect.x;
						pukciP.WorldY += 100;
					}

					pukciP.WorldX = DeathRect.x;
					pukciP.WorldY = DeathRect.y;
					pukciP.Type = 1;

					for (int u = 0; u <= 3; u++)
					{
						for (int i = 0; i < 5; i++)
						{
							pukciP.WorldX += 100;
							PickupVector.push_back(pukciP);
						}
						pukciP.WorldX = DeathRect.x;
						pukciP.WorldY += 100;
						pukciP.Type++;
					}

					Boss = true;
					Multiplier = (float)(ScreenWidth - 50) / 4000;
					LevelProgress = 3;
				}
				break;
			}

			case 3:
			{
				if (BossStage == 6)
				{
					LaserSpeed = 0;
					Laser = true;
					LaserY = 900;
				}

				if (Boss == false) LevelProgress = 4;
				break;
			}

			case 4:
			{
				CutsceneFinished = false;
				int FPS = 60;
				Character.NormalMovement = false;
				Update = false;
				SDL_Rect Rect = TeleportClips[0];
				Number = 0;
				Mix_HaltMusic();

				while (!CutsceneFinished)
				{
					FPSTimer.start();
					DoThings();
					HandleEvents();
					Number++;

					Camera.MoveViewport(Temp1 - ScreenWidth/2 + 30, Temp2 - ScreenHeight/2 + 30);

					if (Number < 250)
					{
						if (rand() < 2000) CreateExplosion(Temp1 - 10 + rand() % 70, Temp2 - 10 + rand() % 70);

						Rect.w = 60;
						Rect.h = 60;
						Rect.x = Temp1 - Camera.x;
						Rect.y = Temp2 - Camera.y;
						SDL_FillRect(Screen, &Rect, 0xFFFFFF);

						int XInc = (Temp1 - 830) / 16;
						int YInc = (Temp2 - 20) / 16;
						int X = 850;
						int Y = 20;

						for (int u = 0; u < 16; u++)
						{
							X += XInc;
							Y += YInc;
							Rect.x = X - Camera.x;
							Rect.y = Y - Camera.y;
							Rect.w = 20;
							Rect.h = 20;
							SDL_FillRect(Screen, &Rect, 0x262626);
						}

						SDL_Flip(Screen);
					}

					else if (Number == 250) 
					{
						CreateDebris(16,5,Temp1 + 30, Temp2 + 30, 0,0, 0xFFFFFF);
						CreateExplosion(Temp1 + 30, Temp2 + 30);
						FPS = 15;
					}

					else if (Number == 295) FPS = 60;

					else if (Number == 500) ScreenAlpha = 1;

					else if (ScreenAlpha == 249)
					{
						CutsceneFinished = true;
						LevelFinished = true;
					}
					if (FPSTimer.get_ticks() < 1000 / FPS) SDL_Delay(1000 / FPS - FPSTimer.get_ticks());
				}
			}
		}
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay(1000 / 60 - FPSTimer.get_ticks());
	}

	ClearScreen();
	Terminal("Resources/Text/End");
	SDL_Flip(Screen);
	SDL_Delay(2000);
	QuitGame(true);
}