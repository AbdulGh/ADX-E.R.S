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

bool Update = true;

Timer SpareTimer;

std::string AmmoNames[WEAPONS] = {"Pistol","Shotgun","Machinegun"};

Player Character;

SDL_Rect CharacterRect;
SDL_Rect HealthRect;

std::vector <int> SpawnVector;

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
	switch (Rand)
	{
	case 1:
		FadeText("if (Character.Health <= 0) {Victory = true; Laugh();}");
		break;
	case 2:
		FadeText("As expected.");
		break;
	case 3:
		FadeText("Next!");
		break;
	case 4:
		FadeText("This part isn't even hard, what's wrong with you?");
		break;
	case 5:
		FadeText("And here I was, thinking you showed promise...");
		break;
	case 6:
		FadeText("If I had sides, they would be killing me right now.");
		break;
	case 7:
		FadeText("The angle I saw that from made that even funnier!");
		break;
	case 8:
		FadeText("Pfffffffffffffffft.");
		break;
	case 9:
		FadeText("I'm glad I can rewind CCTV footage.");
		break;
	case 10:
		FadeText("And I wasn't even trying!");
		break;
	}
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
	bool EnterPressed = false;
	FadeVector.erase(FadeVector.begin(),FadeVector.end());
	GetRandomDeathQuote();
	int x = 0;
	int y = 0;
	SpareTimer.start();

	while(!EnterPressed && SpareTimer.get_ticks() < 5000)
	{
		remiT.start();
		ClearScreen();
		DoTiles(Camera.x,Camera.y);
		DoMouse(&x,&y);
		DoDebris(Camera.x,Camera.y,Screen);
		DoProjectiles(Camera.x,Camera.y);
		DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
		DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, 0, 0);
		DoFloat(Camera.x,Camera.y);
		CheckText();
		SDL_Flip(Screen);
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) EnterPressed = true;
		}
		if (remiT.get_ticks() < 1000/60) SDL_Delay((1000/60) - remiT.get_ticks());
	}
}

void DoThings()
{
	if (ShotTimer != 0) ShotTimer--;
	
	if (Update)
	{
		Camera.MoveViewport(Character.WorldX + (Character.CurrentSprite->w / 2) - (ScreenWidth / 2),Character.WorldY + (Character.CurrentSprite->h / 2) - (ScreenHeight / 2));
		Camera.Update();
	}
	CheckShake();
	DoTiles(Camera.x,Camera.y);
	DoMouse(&x,&y);
	DoDebris(Camera.x,Camera.y,Screen);
	Character.Update();
	CharacterRect.x = Character.WorldX - Camera.x;
	CharacterRect.y = Character.WorldY - Camera.y;
	DoPickups(Camera.x,Camera.y,CharacterRect);
	DoEnemyProjectiles(Camera.x, Camera.y,CharacterRect);
	DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, Character.XVel, Character.YVel);
	CheckText();
	DoFloat(Camera.x,Camera.y);
	DoProjectiles(Camera.x,Camera.y);

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
	ApplySurface(500,HealthRect.y - 6,Message,Screen);

	SpareStream.str("");
	SpareStream << "Lives: " << Character.Lives;
	Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
	ApplySurface(ScreenWidth - (Message->w + 10),HealthRect.y - 6,Message,Screen);

	if (Enemies != 0)
	{
		SpareStream.str("");
		SpareStream << "Enemies: " << Enemies;
		Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface((ScreenWidth - Message->w) / 2, 10, Message, Screen);
	}

	if (Laser)
	{
		LaserY -= 2.4;
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

	if (Character.Reset == true)
	{
		Character.Render = false;
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
			if (event.key.keysym.sym == SDLK_ESCAPE) {State = QUIT; main(NULL,NULL);}
			else if (event.key.keysym.sym == SDLK_e)SwapWeapons(true); //CurrentSelection = 3 - CurrentSelection; //CreateDebris(3,5,Character.WorldX,Character.WorldY,20,20,0x7F3300);
			else if (event.key.keysym.sym == SDLK_q) SwapWeapons(false);
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
		if (CurrentSelection == 1)
		{
			Mix_PlayChannel(-1,Pistol,0);
			int Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
			float XRatio, YRatio;
			GetXYRatio(&XRatio,&YRatio,Angle,20);
			CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
			ShotTimer = 50;
		}

		else if (CurrentSelection == 2)
		{
			if (Ammo[1] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[1]--;
				Mix_PlayChannel(-1,ShotgunFire,0);
				Mix_PlayChannel(-1,ShotgunPump,0);
				int Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
				float XRatio, YRatio;
				for (int i = 0; i <= 8; i++)
				{
					GetXYRatio(&XRatio,&YRatio,Angle + rand () % 4 - i, 19);
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,1);
				}
				ShotTimer = 70;
			}
		}

		else if (CurrentSelection == 3 && Ammo[2] != 0)
		{
			if (Ammo[2] == 0) Mix_PlayChannel(-1,Empty,0);
			else
			{
				Ammo[2]--;
				Mix_PlayChannel(-1,MachineGunFire,0);
				int Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y) + (rand() % 8) - 4;
				float XRatio, YRatio;
				GetXYRatio(&XRatio,&YRatio,Angle,30);
				CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,2);
				ShotTimer = 10;
			}
		}
	}
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
	

	bool LevelFinished = false;
	int Weapons = 0;
	Timer FPSTimer;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	XSpawn = 1834;
	YSpawn = 2812;
	Character.WorldX = 1834;
	Character.WorldY = 2812;
	HealthRect.x = 115;
	HealthRect.h = 20;
	HealthRect.y = ScreenHeight - 25;
	HealthRect.w = 200;

	if (!LoadLevel("Resources/Levels/1")) {State = QUIT; Menu();}
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelColour = 0x000000;

	FadeText("Stay RIGHT there. Don't move an inch.");

	while (!LevelFinished && State == GAME) //Level 1
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		if (Character.WorldX < 1400 && LevelProgress < 1)
		{
			LevelProgress = 1;
			SpawnVector.push_back (1300);
			SpawnVector.push_back (Character.WorldY);
			SpawnVector.push_back (1);

			SpawnVector.push_back (1250);
			SpawnVector.push_back (Character.WorldY + 100);
			SpawnVector.push_back (1);

			SpawnVector.push_back (1550);
			SpawnVector.push_back (Character.WorldY - 135);
			SpawnVector.push_back (1);

			SpawnVector.push_back (1150);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (1);

			SpawnVector.push_back (900);
			SpawnVector.push_back (Character.WorldY);
			SpawnVector.push_back (1);

			SpawnVector.push_back (850);
			SpawnVector.push_back (Character.WorldY + 100);
			SpawnVector.push_back (1);


			SpawnVector.push_back (750);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (1);



			SpawnEnemies(SpawnVector);
			SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());

			FadeText("Why do they never listen?");
		}

		else if (Enemies == 0 && LevelProgress == 1)
		{
			LevelProgress = 2;
			FadeText("My favourite grunts!");
		}

		else if (LevelProgress == 2 && Character.WorldY < 1600)
		{
			LevelProgress = 3;
			SpawnVector.push_back (Character.WorldX - 100);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX + 100);
			SpawnVector.push_back (Character.WorldY);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 145);
			SpawnVector.push_back (Character.WorldY + 120);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 300);
			SpawnVector.push_back (Character.WorldY - 135);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX + 200);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 445);
			SpawnVector.push_back (Character.WorldY + 420);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 600);
			SpawnVector.push_back (Character.WorldY - 435);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX + 500);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 345);
			SpawnVector.push_back (Character.WorldY + 720);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX + 300);
			SpawnVector.push_back (Character.WorldY + 256);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 445);
			SpawnVector.push_back (Character.WorldY + 620);
			SpawnVector.push_back (2);

			SpawnEnemies(SpawnVector);
			SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
		}

		else if (LevelProgress == 3 && Enemies == 0)
		{
			LevelProgress = 4;
			FadeText("Alright, you've passed the test. You can go free.");
			SpareTimer.start();
		}

		else if (LevelProgress == 4 && SpareTimer.get_ticks() > 8000)
		{
			LevelProgress = 5;
			SpareTimer.stop();
			FadeText("I lied, I'm sorry.");

			SpawnVector.push_back (Character.WorldX + 400);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 145);
			SpawnVector.push_back (Character.WorldY + 520);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX + 300);
			SpawnVector.push_back (Character.WorldY + 256);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 445);
			SpawnVector.push_back (Character.WorldY + 620);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX + 500);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 345);
			SpawnVector.push_back (Character.WorldY + 720);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX + 300);
			SpawnVector.push_back (Character.WorldY + 256);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 445);
			SpawnVector.push_back (Character.WorldY + 620);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 445);
			SpawnVector.push_back (Character.WorldY + 620);
			SpawnVector.push_back (2);

			SpawnVector.push_back (Character.WorldX - 145);
			SpawnVector.push_back (Character.WorldY + 120);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 300);
			SpawnVector.push_back (Character.WorldY - 135);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 145);
			SpawnVector.push_back (Character.WorldY + 120);
			SpawnVector.push_back (1);

			SpawnVector.push_back (Character.WorldX - 300);
			SpawnVector.push_back (Character.WorldY - 135);
			SpawnVector.push_back (1);

			SpawnEnemies(SpawnVector);
			SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
		}

		else if (LevelProgress == 5 && Enemies == 0)
		{
			LevelProgress = 6;
			FadeText("You really do have a lot of potential.");
			FadeText("No one has actually ever made it this far before.");
			FadeText("Into the cage you go.");
			SpareTimer.start();
		}

		else if (LevelProgress == 6 && SpareTimer.get_ticks() > 10000) LevelFinished = true;

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}

	LevelFinished = false;
	if (!LoadLevel("Resources/Levels/2")) {State = QUIT; Menu();}

	EnemyVector.erase(EnemyVector.begin(),EnemyVector.end());
	SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
	ProjectileVector.erase(ProjectileVector.begin(),ProjectileVector.end());
	PickupVector.erase(PickupVector.begin(),PickupVector.end());
	EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());

	Character.Lives += 3;
	XSpawn = 1000;
	YSpawn = 1000;
	Character.WorldX = 1000;
	Character.WorldY = 1000;
	LevelProgress = 0;
	LevelColour = 0x000000;
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	while(LevelFinished == false && State == GAME)
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		if (LevelProgress == 0)
		{
			LevelProgress = 1;
			SpareTimer.start();
			FadeText("There is no way out. Go ahead and try.");
		}

		else if (LevelProgress == 1 && SpareTimer.get_ticks() > 5000)
		{
			LevelProgress = 2;
			FadeText("I do hope you're good at dodging!");
			SpawnVector.push_back(50);
			SpawnVector.push_back(50);
			SpawnVector.push_back(3);
			SpawnEnemies(SpawnVector);
		}

		else if (LevelProgress == 2 && Enemies == 0)
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

			SpawnVector.push_back(1000);
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

		else if (LevelProgress == 3 && Enemies == 0)
		{
			LevelProgress = 4;

			SpawnVector.push_back(1000);
			SpawnVector.push_back(50);
			SpawnVector.push_back(3);

			SpawnVector.push_back(1950);
			SpawnVector.push_back(50);
			SpawnVector.push_back(2);

			SpawnVector.push_back(50);
			SpawnVector.push_back(1000);
			SpawnVector.push_back(1);

			SpawnVector.push_back(1000);
			SpawnVector.push_back(1000);
			SpawnVector.push_back(1);

			SpawnVector.push_back(1950);
			SpawnVector.push_back(1000);
			SpawnVector.push_back(2);

			SpawnVector.push_back(50);
			SpawnVector.push_back(1950);
			SpawnVector.push_back(3);

			SpawnVector.push_back(1000);
			SpawnVector.push_back(1950);
			SpawnVector.push_back(3);

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
			SpawnVector.push_back(2);

			SpawnEnemies(SpawnVector);

			SpawnVector.erase(SpawnVector.begin(), SpawnVector.end());
		}

		else if (LevelProgress == 4 && Enemies == 0)
		{
			LevelProgress = 5;
			FadeText("Clearly throwing grunts at you is ineffective.");
			FadeText("I've invited some old friends to the party.");
			FadeText("I think you may have actually met them before!");

			Pickup PushThis;
			PushThis.Type = 1;
			PushThis.WorldX = 500;
			PushThis.WorldY = 500;

			for (int i = 0; i <= 2; i++)
			{
				PushThis.Type = 1 + i;
				PushThis.WorldX = 500 + 100 * i;
				PickupVector.push_back(PushThis);
			}

			SpareTimer.start();
		}

		else if (LevelProgress == 5 && SpareTimer.get_ticks() > 10000)
		{
			LevelProgress = 6;
			SpawnVector.push_back(50);
			SpawnVector.push_back(50);
			SpawnVector.push_back(4);
			Boss = true;
			SpawnEnemies(SpawnVector);
			Mix_PlayMusic(SmashSong,-1);
		}

		else if (LevelProgress == 6)
		{
			Number++;
			if (Number == 2147483646) Number = 0;
			if (Number % 30 == 0)
			{
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
				SpawnVector.push_back(LevelWidth - 50);
				SpawnVector.push_back((rand() % LevelHeight - 50) + 50);
				SpawnVector.push_back(5);
				SpawnEnemies(SpawnVector);
			}

			else if (Number % 25 == 0)
			{
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
				SpawnVector.push_back(50);
				SpawnVector.push_back((rand() % LevelHeight - 50) + 50);
				SpawnVector.push_back(6);
				SpawnEnemies(SpawnVector);
			}

			if (Boss == false) LevelProgress = 7;
		}

		else if (LevelProgress == 7)
		{
			for (int i = 0; i < EnemyVector.size(); i++) EnemyVector.at(i).Health = 0;
			EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());
			LevelProgress = 8;
		}
		
		else if (LevelProgress == 8)
		{
			float Vel = 0.01;
			Shake = true;
			Dur = 20;
			Mag = 20;

			Mix_HaltMusic();

			while (Temp1 < LevelWidth - 70)
			{
				Camera.MoveViewport(Temp1 - ScreenWidth/2, Temp2 - ScreenHeight/2);
				Camera.Update();
				CheckShake();
				DoDebris(Camera.x,Camera.y,Screen);
				DoTiles(Camera.x,Camera.y);
				DoMouse(&x,&y);
				Character.XVel = 0;
				Character.YVel = 0;
				Character.Update();
				DoPickups(Camera.x,Camera.y,CharacterRect);
				DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
				DoEnemies(Camera.x,Camera.y,0,0,CharacterRect, 0, 0);
				DoFloat(Camera.x,Camera.y);
				DoProjectiles(Camera.x,Camera.y);

				ApplySurface(Temp1 - Camera.x, Temp2 - Camera.y, Ship, Screen);

				Temp1 += Vel;
				if (Vel < 10) Vel *= 1.1;

				SDL_Flip(Screen);
				SDL_Delay(10);
			}

			Shake = true;
			Dur = 20;
			Mag = 30;
			Update = false;

			CreateDebris(7,7,LevelWidth - 20,Temp2,-10,0,0xFFFFFF);

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

			Mix_PlayChannel(0,SmashDeath,0);

			FadeText("As if!");
		}

		else if (LevelProgress == 9)
		{
			Character.XVel = 0;
			Character.YVel = 0;
			Camera.MoveViewport(Temp1 - ScreenWidth/2, Temp2 - ScreenHeight/2);
			Camera.Update();
			if (SpareTimer.get_ticks() > 1200) 
			{
				LevelProgress = 10;
				Update = true;
			}
		}

		else if (LevelProgress == 10 && Character.WorldX > LevelWidth - Character.CurrentSprite->w) LevelFinished = true;
		

		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}
	LevelFinished = false;
	if (!LoadLevel("Resources/Levels/3")) {State = QUIT; Menu();}

	EnemyVector.erase(EnemyVector.begin(),EnemyVector.end());
	SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
	ProjectileVector.erase(ProjectileVector.begin(),ProjectileVector.end());
	PickupVector.erase(PickupVector.begin(),PickupVector.end());
	EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());

	Character.Lives += 3;
	XSpawn = 1000;
	YSpawn = 4500;
	Character.WorldX = 1000;
	Character.WorldY = 4500;
	LevelProgress = 0;
	LevelColour = 0x000000;
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;

	LaserY = 5000;

	while(LevelFinished == false && State == GAME)
	{
		FPSTimer.start();

		DoThings();

		HandleEvents();

		switch(LevelProgress)
		{
		case 0:
			LaserY = 5000;
			FadeText("You thought that wasn't part of my plan?");
			LevelProgress = 1;
			SpareTimer.start();
			break;
		case 1:
			Character.WorldX = XSpawn;
			Character.WorldY = YSpawn;
			if (SpareTimer.get_ticks() > 2000)
			{
				LevelProgress = 2;
				Laser = true;
			}
			break;
		case 2:
			if (static_cast<int>(LaserY) % 160 == 2)
			{
				SpawnVector.push_back((rand() % LevelWidth) + 1);
				SpawnVector.push_back(2500);
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

	LevelFinished = false;
	if (!LoadLevel("Resources/Levels/4")) {State = QUIT; Menu();}

	EnemyVector.erase(EnemyVector.begin(),EnemyVector.end());
	SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
	ProjectileVector.erase(ProjectileVector.begin(),ProjectileVector.end());
	PickupVector.erase(PickupVector.begin(),PickupVector.end());
	EnemyProjectileVector.erase(EnemyProjectileVector.begin(),EnemyProjectileVector.end());

	Character.Lives += 3;
	XSpawn = 1800;
	YSpawn = 5850;
	Character.WorldX = 1800;
	Character.WorldY = 5850;
	LevelProgress = 0;
	LevelColour = 0x000000;
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;

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
			break;
		case 1:
			Character.WorldX = XSpawn;
			Character.WorldY = YSpawn;
			if (SpareTimer.get_ticks() > 2000) LevelProgress = 2;
		case 2:
			if (Character.WorldY < 5800)
			{
				Laser = true;
				LaserY = 6000;
				LevelProgress = 3;

				SpawnVector.push_back(950);
				SpawnVector.push_back(20);
				SpawnVector.push_back(7);

				SpawnEnemies(SpawnVector);
				SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
			}
			break;
		case 3:
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

	ClearScreen();
	ApplySurface(20,20,Win,Screen);
	SDL_Flip(Screen);
	SDL_Delay(2000);
	State = QUIT;
}