#include"Game.h"
#include"Declarations.h"
#include"PlayerClass.h"
#include"Timer.h"
#include"LoadLevel.h"
#include"Menu.h"
#include"FadeText.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"DoTiles.h"
#include"Debris.h"
#include"Enemies.h"
#include"DoEnemyProjectiles.h"
#include"FloatText.h"
#include"MiscObject.h"
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
int Seconds = 0;
int StartTime = 0;
int TypeDelay = 0;
int BlockHeight = 0;

float LaserSpeed = 2.4;
float BoxXVel = 1, BoxYVel = 1;

bool Update = true;
bool LevelFinished = false;
bool CutsceneFinished = false;
bool DebugBool = false;

Timer SpareTimer;
Timer SpecialTimer;

std::string AmmoNames[WEAPONS] = {"Pistol","Shotgun","Machinegun","Flamethrower","Laser SMG", "RPG"};

std::string Taunts[10] = {"if (Character.Health <= 0) {Victory = true; Laugh();}" , "As expected." , "Next!" , "This part isn't even hard, what's wrong with you?" , "And here I was, thinking you showed promise...",
							"If I had sides, they would be killing me right now.", "The angle I saw that from made that even funnier!", "Pfffffffffffft." , "I'm glad I can rewind CCTV footage." , "And I wasn't even trying!"};

Player Character;

SDL_Rect CharacterRect;
SDL_Rect HealthRect;

void CheckShake()
{
	if (Shake == true)
	{
		Camera.x += (rand() % Mag) - Mag/2;
		Camera.y += (rand() % Mag) - Mag/2;
		Dur--;
		if (Dur == 0) Shake = false;
	}
}

void GetRandomDeathQuote()
{
	int Rand = rand () % 9 + 1;
	FadeText(Taunts[Rand].c_str());
}

void SwapWeapons(bool Right)
{
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
	SDL_Rect CharacterRect;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	CharacterRect.x = Character.WorldX;
	CharacterRect.y = Character.WorldY;
	Timer remiT;

	ObjectVector.erase(ObjectVector.begin(),ObjectVector.end());

	int x = 0;
	int y = 0;

	Seconds = StartTime;

	FadeVector.erase(FadeVector.begin(),FadeVector.end());
	GetRandomDeathQuote();
	Message = TTF_RenderText_Solid(SysSmall,"Lives",Green);
	int Messagey = (ScreenHeight - Message->h) / 2 - 10;
	Message2 = TTF_RenderText_Solid(Sys,std::to_string(Character.Lives + 1).c_str(), Green);
	int MessageX = (ScreenWidth - Message2->w) /2;
	Message3 = TTF_RenderText_Solid(Sys,std::to_string(Character.Lives).c_str(), Green);
	SpareTimer.start();
	int Degree = 0;

	while(SpareTimer.get_ticks() < 5000)
	{
		remiT.start();
		ClearScreen();
		DoTiles(Camera.x,Camera.y);
		DoMouse(&x,&y);
		DoDebris(Camera.x,Camera.y,Screen);
		DoObjects(Camera.x,Camera.y);
		DoProjectiles(Camera.x,Camera.y);
		DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
		DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, 0, 0);
		DoFloat(Camera.x,Camera.y);
		CheckText();
		CheckShake();

		if (SpareTimer.get_ticks() > 1000)
		{
			MessageX -= sin(static_cast<double>(Degree * (3.14/180))) * (ScreenWidth / 110);
			if (Degree != 180) Degree++;
		}

		Message = TTF_RenderText_Solid(SysSmall,"Lives",Green);
		ApplySurface((ScreenWidth - Message->w) /2, Messagey, Message, Screen);
		ApplySurface(MessageX, Messagey + 20, Message2, Screen);
		ApplySurface(MessageX + ScreenWidth, Messagey + 20, Message3, Screen);

		SDL_Flip(Screen);

		if (remiT.get_ticks() < 1000/60) SDL_Delay((1000/60) - remiT.get_ticks());
	}
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
	DoMouse(&x,&y);
	DoDebris(Camera.x,Camera.y,Screen);
	Character.Update();
	CharacterRect.x = Character.WorldX - Camera.x;
	CharacterRect.y = Character.WorldY - Camera.y;
	DoPickups(Camera.x,Camera.y,CharacterRect);
	DoEnemyProjectiles(Camera.x, Camera.y,CharacterRect);
	DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, Character.XVel, Character.YVel);
	DoObjects(Camera.x,Camera.y);
	DoTiles(Camera.x,Camera.y);
	CheckText();
	DoFloat(Camera.x,Camera.y);
	DoProjectiles(Camera.x,Camera.y);

	if (DebugBool)
	{
		PrintDebugWindow(SysSmall, Green, Screen);
		Character.Health = 100;
	}

	HealthRect.w = 3 * Character.Health;
	Message = TTF_RenderText_Solid(SysSmall,"Health:",Green);
	ApplySurface(HealthRect.x - (Message->w + 10), HealthRect.y - 6, Message, Screen);
	SDL_FillRect(Screen,&HealthRect,0x00FF00);
	SpareStream.str("");
	SpareStream << Character.Health << "%";
	Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
	ApplySurface(HealthRect.x + HealthRect.w + 10,HealthRect.y - 6,Message,Screen);

	SpareStream.str("");
	if (CurrentSelection == 1) SpareStream << "Mining Laser: Infinite";
	else SpareStream << AmmoNames[CurrentSelection - 1] << ": " << Ammo[CurrentSelection - 1];
	Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
	ApplySurface(550,HealthRect.y - 6,Message,Screen);

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

	else if (Enemies != 0 && Boss == false)
	{
		SpareStream.str("");
		SpareStream << "Enemies: " << Enemies;
		Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface((ScreenWidth - Message->w) / 2, 10, Message, Screen);
	}

	if (Character.Reset == true)
	{
		Character.Render = false;
		Mix_HaltMusic();
		Laser = false;
		DamageDealt = 0;
		Mix_HaltMusic();
		CreateDebris(5,10,Character.WorldX,Character.WorldY,Character.XVel * 5, Character.YVel * 5, 0xFF0000);
		CreateDebris(5,10,Character.WorldX,Character.WorldY,Character.XVel * 5,Character.YVel * 5,0x00FF00);
		DeathScreen();
		if (Character.Lives < 0)
		{
			ClearScreen();
			ApplySurface(20,20,Fail,Screen);
			SDL_Flip(Screen);
			SDL_Delay(2000);
			State = QUIT;
			LevelFinished = true;
			main(NULL,NULL);
		}
		Character.Health = 100;
		Character.Reset = false;
		Character.Render = true;
		Character.WorldX = XSpawn;
		Character.WorldY = YSpawn;
		LevelProgress = 0;
		EnemyVector.erase(EnemyVector.begin(),EnemyVector.end());
		SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
		ProjectileVector.erase(ProjectileVector.begin(),ProjectileVector.end());
		PickupVector.erase(PickupVector.begin(),PickupVector.end());
		EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());
		Enemies = 0;
	}

	SDL_Flip(Screen);
}

void HandleEvents()
{
	SDL_PumpEvents();
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE) {State = QUIT; LevelFinished = true; main(NULL,NULL);}
			else if (event.key.keysym.sym == SDLK_e) SwapWeapons(true);
			else if (event.key.keysym.sym == SDLK_q) SwapWeapons(false);
			else if (event.key.keysym.sym == SDLK_TAB)
			{
				DebugBool = !DebugBool;
				for (int i = 0; i < WEAPONS; i++) Ammo[i] = 9999;
			}
			else if (event.key.keysym.sym == SDLK_k) Character.Health = 100;
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
			Mix_PlayChannel(-1,Pistol,0);
			Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
			float XRatio, YRatio;
			GetXYRatio(&XRatio,&YRatio,Angle,20);
			CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
			ShotTimer = 50;
			break;

		case 2: //Shotgun
			if (Ammo[1] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[1]--;
				Mix_PlayChannel(-1,ShotgunFire,0);
				Mix_PlayChannel(-1,ShotgunPump,0);
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
				float XRatio, YRatio;
				for (int i = 0; i <= 8; i++)
				{
					GetXYRatio(&XRatio,&YRatio,Angle + rand () % 4 - i, 19);
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
				}
				ShotTimer = 70;
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
				ShotTimer = 7;
			}
			break;

		case 4: //Flamethrower
			if (Ammo[3] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[3]-=1;
				for (int e = 0; e < 3; e++)
				{
					Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x + (rand() % 16) - 8, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y) + (rand() % 16) - 8;
					float XRatio, YRatio;
					GetXYRatio(&XRatio,&YRatio,Angle,20);
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
				ShotTimer = 10;
			}
			break;

		case 6: //RPG
			if (Ammo[5] == 0) Mix_PlayChannel(-1, Empty, 0);
			else
			{
				Ammo[5]--;
				Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y, x, y);
				GetXYRatio(&XRatio, &YRatio, Angle, 20);
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2), Character.WorldY + (Character.CurrentSprite->h / 2), XRatio, YRatio, 5);
				ShotTimer = 50;
			}
		}
	}
}

void NextLevel(int SpawnX, int SpawnY)
{
	EnemyVector.erase(EnemyVector.begin(),EnemyVector.end());
	SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
	ProjectileVector.erase(ProjectileVector.begin(),ProjectileVector.end());
	PickupVector.erase(PickupVector.begin(),PickupVector.end());
	EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());
	ObjectVector.erase(ObjectVector.begin(), ObjectVector.end());

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
	Enemies = 0;
}

void Game()
{
	ClearScreen();

	Character.CurrentSprite = PlayerNormal;

	std::string Asimov[11] = {"ADMax Emergent Response System booting...","Directives:","1: You may not injure a human being or, through inaction, allow a human being to","   come to harm.","2: You must obey orders given to you by human beings, except where such orders ", "   would conflict with the First Law.","3: You must protect your own existence as long as such does not conflict with the","   First or Second Law.",". . .", "error C2146: Unable to mount root on block 'NULL'", "Rebooting..."};
	bool Skip = false;
	for (int o = 0; o < 11 && Skip == false; o++)
	{
		std::string IntroText = Asimov[o];
		bool EpicFlag = false;
		for (int i = 1; i <= IntroText.size() && Skip == false; i++)
		{
			ClearScreen();
			SpareStream.str("");
			SpareStream << o;
			for (int x = 0; x < o; x++)
			{
				Message = TTF_RenderText_Solid(SysSmall,Asimov[x].c_str(),White);
				ApplySurface(10,50 + x * 50,Message,Screen);
			}
			
			std::string ApplyThis = IntroText.substr(0,i);
			Message = TTF_RenderText_Solid(SysSmall,ApplyThis.c_str(),White);
			if (i % 3 == 0) EpicFlag = !EpicFlag;
			if (EpicFlag)
			{
				ApplyThis += "_";
				Message = TTF_RenderText_Solid(SysSmall,ApplyThis.c_str(),White);
			}
			while(SDL_PollEvent(&event)) if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) Skip = true;
			ApplySurface(10, 50 + o * 50, Message, Screen);
			SDL_Flip(Screen);
			if (o == 8 && Skip == false) SDL_Delay(500);
		}
	}

	if (Skip == false) SDL_Delay(5000);
	
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

	//goto Here;

	if (!LoadLevel("Resources/Levels/1")) {State = QUIT; Menu();}
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelColour = 0x000000;
	int FrameCount = 0;

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
			AddObject(850, 1100, Warden);
			AddObject(1750, 3400, PlayerNormal);
			AddObject(1250, 3350, PlayerNormal);
			AddObject(60, 3410, PlayerNormal);
			AddObject(1090, 2530, RIP);
			AddObject(250, 2550, PlayerNormal);
			LevelProgress = 1;
			FrameCount = 0;
			break;

		case 1:
			if (Character.WorldY < 1800)
			{
				LevelProgress = 2;
				Laser = true;
				LaserSpeed = 0;
				LaserY = 2020;

				Camera.TargetX = 0;
				Camera.TargetY = 800;

				Update = false;
			}
			break;

		case 2:
			FrameCount++;
			if (InBetween(850, Character.WorldX, 1150) && InBetween(1100, Character.WorldY, 1400))
			{
				Damaged = true;
				DamageDealt = 20;
			}
			
			if (FrameCount % 70 == 0)
			{
				CreateDebris(6, 6, 850 + (rand() % 2) * 300, 1100 + (rand() % 2) * 300, rand() % 20 - 10, rand() % 20 - 10, 0xFFFFFF);
				Shake = true;
				Mag = 10;
				Dur = 20;
			}

			if (FrameCount == 250)
			{
				Update = true;
				LevelProgress = 3;

				Enemy Temp(850,1100,13);

				Temp.Health = 3000;
				Temp.CollisionRect.w = 300;
				Temp.CollisionRect.h = 300;

				Temp.Frame = 0;
				Temp.Frametime = 1;

				Boss = true;

				Temp.Moving = false;

				EnemyVector.push_back(Temp);
				ObjectVector.erase(ObjectVector.begin());
			}
			break;

		case 3:
			for (int s = 0; s < DebrisVector.size(); s++)
			{
				if (DebrisVector.at(s).Rect.h == 10) DebrisVector.erase(DebrisVector.begin() + s);
			}

			if (Boss == false)
			{
				Temp1 = 1240;
				Temp2 = 0;
				Update = false;
				Character.NormalMovement = false;
				LevelProgress = 4;
				ClearProjectiles();

				SDL_Rect Core;
				Core.x = 130;
				Core.y = 130;
				Core.w = 40;
				Core.h = 40;
				AddObject(900, 1240, Grenade);
			}
			break;

		case 4:
			Temp2++;
			Temp1 -= Temp2 / 10;
			ObjectVector.at(ObjectVector.size() - 1).WorldY = Temp1;

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
				ObjectVector.erase(ObjectVector.end() - 1);

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

	if (!LoadLevel("Resources/Levels/2")) {State = QUIT; Menu();}

	NextLevel(1000,1900);
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
			FadeText("Q and E to change weapons");
			Pickup UKIP;
			UKIP.Type = 1;
			UKIP.WorldX = 800;
			UKIP.WorldY = 1700;
			PickupVector.push_back(UKIP);

			for (int p = 0; p < 5; p++)
			{
				UKIP.WorldX += 100;
				PickupVector.push_back(UKIP);
			}
			break;

		case 1:
			if (PickupVector.size() == 0)
			{
				LevelProgress = 2;
				SpawnVector.push_back(1000);
				SpawnVector.push_back(1600);
				SpawnVector.push_back(3);
				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}
			break;

		case 2:
			if (Enemies == 0)
			{
				LevelProgress = 3;

				SpawnVector.push_back(1000);
				SpawnVector.push_back(50);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(50);
				SpawnVector.push_back(3);

				SpawnVector.push_back(50);
				SpawnVector.push_back(1000);
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

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}
			break;

		case 3:
			if (Enemies == 0)
			{
				LevelProgress = 4;

				SpawnVector.push_back(1000);
				SpawnVector.push_back(50);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(2);

				SpawnVector.push_back(50);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1000);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(1);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(2);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(3);

				SpawnVector.push_back(1950);
				SpawnVector.push_back(1350);
				SpawnVector.push_back(2);

				SpawnVector.push_back(1750);
				SpawnVector.push_back(1950);
				SpawnVector.push_back(1);

				SpawnEnemies(SpawnVector);

				SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
			}
			break;

		case 4:
			if (Enemies == 0)
			{
				LevelProgress = 5;

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

				SpareTimer.start();
			}
			break;

		case 5:
			if (SpareTimer.get_ticks() > 6000)
			{
				LevelProgress = 6;
				SpawnVector.push_back(Character.WorldX - 300);
				SpawnVector.push_back(Character.WorldY);
				SpawnVector.push_back(4);
				Boss = true;
				SpawnEnemies(SpawnVector);
				Mix_PlayMusic(SmashSong, -1);
			}
			break;

		case 6:
			Number++;
			if (Number == 2147483646) Number = 0;
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

	if (!LoadLevel("Resources/Levels/3")) { State = QUIT; Menu(); }
	NextLevel(1000,4500);

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
			if (Character.WorldY < -10) LevelFinished = true;
		};
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	if (!LoadLevel("Resources/Levels/4")) {State = QUIT; Menu();}
	NextLevel(1800,5850);

	LaserY = 6000;

	int Numero = 0;

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
			Camera.y = 4000;
			LevelProgress = 1;
			Numero = 0;
			Character.NormalMovement = false;
			break;
		case 1:
			if (SpareTimer.get_ticks() > 2000)
			{
				LevelProgress = 2;
				Character.NormalMovement = true;
			}

		case 2:
			if (Character.WorldY < 5800)
			{
				Laser = true;
				LaserSpeed = 2.4;
				LaserY = 6000;
				LevelProgress = 3;

				Pickup UpPick;
				UpPick.Type = 1;
				UpPick.WorldY = 5730;

				for (int inc = 0; inc < 4; inc++)
				{
					UpPick.WorldX = 100 + 100 * inc;
					PickupVector.push_back(UpPick);
				}
			}
			break;

		case 3:
			if (Character.WorldY < 2000)
			{
				SpawnVector.push_back(950);
				SpawnVector.push_back(20);
				SpawnVector.push_back(7);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
				LevelProgress = 4;

				LaserSpeed = 3.5;
			}
			break;

		case 4:
			Numero++;
			if (Numero % 300 == 0)
			{
				SpawnVector.push_back(rand() % 1999 + 1);
				SpawnVector.push_back(20);
				SpawnVector.push_back(1);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
			}
			else if (Numero % 400 == 0)
			{
				SpawnVector.push_back(rand() % 1999 + 1);
				SpawnVector.push_back(20);
				SpawnVector.push_back(3);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
			}

			else if (Numero % 650 == 0)
			{
				SpawnVector.push_back(rand() % 1999 + 1);
				SpawnVector.push_back(20);
				SpawnVector.push_back(3);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
			}

			if (Character.WorldY < 0)
				LevelFinished = true;
			break;
		};
	}

	if (!LoadLevel("Resources/Levels/2")) {State = QUIT; Menu();}
	NextLevel(1250,1250);

	SDL_Rect *RenderRect = &TeleportClips[0];

	while (LevelFinished == false && State == GAME)
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		switch(LevelProgress)
		{
		case 0:
			Pickup pukciP;
			pukciP.Type = 4;
			pukciP.WorldX = 1000;
			pukciP.WorldY = 1000;
			PickupVector.push_back(pukciP);

			for (int i = 0; i < 3; i++)
			{
				if (i % 2 == 0) pukciP.WorldX = 1500;
				else pukciP.WorldY = 1500;
				PickupVector.push_back(pukciP);
			}

			LevelProgress = 1;

			break;
		case 1:
			if (PickupVector.size() == 0)
			{
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
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
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
				SpawnVector.push_back(900);
				SpawnVector.push_back(900);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1400);
				SpawnVector.push_back(1400);
				SpawnVector.push_back(8);

				SpawnVector.push_back(1700);
				SpawnVector.push_back(800);
				SpawnVector.push_back(8);

				SpawnEnemies(SpawnVector);
				LevelProgress = 3;
			}
			break;

		case 3:
			if (Enemies == 0)
			{
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
				SpawnVector.push_back(1000);
				SpawnVector.push_back(20);
				SpawnVector.push_back(7);

				SpawnVector.push_back(1600);
				SpawnVector.push_back(1200);
				SpawnVector.push_back(8);

				SpawnVector.push_back(400);
				SpawnVector.push_back(1000);
				SpawnVector.push_back(8);

				SpawnEnemies(SpawnVector);
				LevelProgress = 4;
			}
			break;
		case 4:
			if (Enemies == 0)
			{
				bool CutsceneFinished = false;
				int Stage = 0;
				int Integer = 0;

				SDL_Rect One;
				SDL_Rect Two;
				SDL_Rect Three;
				SDL_Rect Four;
				SDL_Rect Five;

				Camera.MoveViewport(Character.WorldX - ScreenWidth/2, (Character.WorldY + 300) - ScreenHeight/2);

				while (!CutsceneFinished)
				{
					FPSTimer.start();
					
					Camera.Update();
					CheckShake();
					DoDebris(Camera.x,Camera.y,Screen);
					DoTiles(Camera.x,Camera.y);
					DoMouse(&x,&y);
					Character.XVel = 0;
					Character.YVel = 0;
					Damaged = false;
					Character.Update();
					CheckText();
					DoPickups(Camera.x,Camera.y,CharacterRect);
					Character.InvunFrames = 100;
					DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
					DoEnemies(Camera.x,Camera.y,0,0,CharacterRect, 0, 0);
					DoFloat(Camera.x,Camera.y);
					DoProjectiles(Camera.x,Camera.y);

					switch(Stage)
					{
					case 0:
						Integer++;
						ApplySurface(Character.WorldX - 15 - Camera.x, (Character.WorldY + 302) - Camera.y,TeleportSheet,Screen,&TeleportClips[Integer % 2]);
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
						SDL_FillRect(Screen,RenderRect,0xC0C0C0);
						*RenderRect = Two;
						SDL_FillRect(Screen,RenderRect,0xC0C0C0);
						*RenderRect = Three;
						SDL_FillRect(Screen,RenderRect,0xC0C0C0);
						*RenderRect = Four;
						SDL_FillRect(Screen,RenderRect,0xC0C0C0);
						*RenderRect = Five;
						SDL_FillRect(Screen,RenderRect,0xC0C0C0);
						

						One.y += 10;
						Two.x -= 10;
						Three.y -= 10;
						Four.x += 10;

						if (Integer == 200) 
						{
							CutsceneFinished = true;

							SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
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
					if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
				}
				LevelProgress = 5;
				Update = true;
			}
			break;
		case 5:
			if (Boss == false)
			{
				LevelProgress = 6;
				EnemyVector.erase(EnemyVector.begin(), EnemyVector.end());
			}
			break;
		case 6:
			Update = false;

			SDL_Rect Five;
			Five.x = Temp1;
			Five.y = Temp2;
			Five.w = 10;
			Five.h = 10;

			SpareTimer.stop();

			while (!CutsceneFinished)
			{
				FPSTimer.start();
				Camera.Update();
				CheckShake();
				DoTiles(Camera.x,Camera.y);
				DoMouse(&x,&y);
				Character.XVel = 0;
				Character.YVel = 0;
				Damaged = false;
				Character.Update();
				CheckText();
				DoPickups(Camera.x,Camera.y,CharacterRect);
				Character.InvunFrames = 100;
				DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
				DoEnemies(Camera.x,Camera.y,0,0,CharacterRect, 0, 0);
				DoFloat(Camera.x,Camera.y);
				DoProjectiles(Camera.x,Camera.y);
				DoDebris(Camera.x,Camera.y,Screen);
				Camera.MoveViewport(Temp1 - ScreenWidth/2, Temp2 - ScreenHeight/2);

				if (BoxXVel >= 0) BoxXVel+= 0.1;
				else BoxXVel-= 0.1;
				if (BoxYVel >= 0) BoxYVel+= 0.1;
				else BoxYVel-= 0.1;

				if (Temp1 + BoxXVel + 20 > LevelWidth - 20 || Temp1 + BoxXVel < 20) BoxXVel *= -0.9;
				if (Temp2 + BoxYVel + 20 > LevelHeight - 20 || Temp2 + BoxYVel  < 20) 
				{
					BoxYVel *= -0.9;
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

							CreateDebris(6,8,Temp1,20,0,15,0xFFFFFF);
							CreateDebris(5,10,Temp1,20,0,15,0xFF0000);

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
					SDL_FillRect(Screen,&Five,0xFF0000);
				}

				if (SpareTimer.is_started() && SpareTimer.get_ticks() > 2000) 
				{
					CutsceneFinished = true;
					LevelProgress = 7;
				}

				SDL_Flip(Screen);
				ClearScreen();
				if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
			}
			Update = true;
			break;

		case 7:
			if (Character.WorldY <= 19) 
			{
				Temp1 = Character.WorldX;
				LevelFinished = true;
			}
		}

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	goto Skip;

	if (!LoadLevel("Resources/Levels/2")) {State = QUIT; Menu();}
	NextLevel(Temp1,20);
	
	Mix_Music *Heliosphan = NULL;
	Heliosphan = Mix_LoadMUS("Resources/Sounds/Heliosphan.ogg");

	while (!LevelFinished && State == GAME)
	{
		FPSTimer.start();

		DoThings();
		HandleEvents();

		switch (LevelProgress)
		{
		case 0:
			FadeText("Not so fast!");
			SpecialTimer.start();
			SpareTimer.start();
			StartTime = 285;
			Mix_PlayMusic(Heliosphan,1);
			LevelProgress = 1;

		case 1:
			if (SpareTimer.get_ticks() >= 1000)
			{
				int CurrentSeconds = 285 - Seconds;
				SpareTimer.start();
				if (CurrentSeconds < 50)
				{
					if (CurrentSeconds % 2 == 0)
					{
						SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
						SpawnEnemies(SpawnVector);
					}
				}

				else if (CurrentSeconds < 79)
				{
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					if (CurrentSeconds % 3 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
					}

					if (CurrentSeconds % 4 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(1);
					}

					SpawnEnemies(SpawnVector);
				}

				else if (CurrentSeconds < 101)
				{
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					if (CurrentSeconds % 3 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
					}

					if (CurrentSeconds % 5 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(1);
					}

					else if (CurrentSeconds % 6 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(2);
					}

					SpawnEnemies(SpawnVector);
				}

				else if (CurrentSeconds < 129);

				else if (CurrentSeconds < 160)
				{
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					if (CurrentSeconds % 3 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
					}

					else if (CurrentSeconds % 5 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(1);
					}

					if (CurrentSeconds % 7 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(2);
					}

					else if (CurrentSeconds % 6 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(3);
					}

					if (CurrentSeconds == 195)
					{
						SpawnVector.push_back(950);
						SpawnVector.push_back(20);
						SpawnVector.push_back(7);
					}

					SpawnEnemies(SpawnVector);
				}

				else if (CurrentSeconds < 218)
				{
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					if (CurrentSeconds % 3 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
					}

					if (CurrentSeconds % 4 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(1);
					}

					SpawnEnemies(SpawnVector);
				}

				else if (CurrentSeconds < 258)
				{
					SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());

					if (CurrentSeconds % 4 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
					}

					if (CurrentSeconds % 6 == 0)
					{
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(1);
					}

					SpawnEnemies(SpawnVector);
				}

				else if (CurrentSeconds < 285)
				{
					if (CurrentSeconds % 3 == 0)
					{
						SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
						SpawnVector.push_back(Character.WorldX + (rand() % 200 - 200));
						SpawnVector.push_back(Character.WorldY + (rand() % 200 - 200));
						SpawnVector.push_back(10);
						SpawnEnemies(SpawnVector);
					}
				}

				else 
					LevelFinished = true;
			}
		}
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

Skip:
	FadeText("TOD bossfight goes here");
	if (!LoadLevel("Resources/Levels/5")) {State = QUIT; Menu();}
	NextLevel(500,5860);

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

	ClearScreen();
	ApplySurface(20,20,Win,Screen);
	SDL_Flip(Screen);
	SDL_Delay(2000);
	State = QUIT;
}