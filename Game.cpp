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
#include<fstream>

void GetRandomDeathQuote()
{
	int Rand = rand () % 1 + 9;
	switch (Rand)
	{
	case 1:
		FadeText("AHAHAHAHAAH. Oh man. That was very funny.");
		break;
	case 2:
		FadeText("They told me that AI could not experience emotion, but right now I'm feeling nothing but joy!");
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
		FadeText("I've killed millions and millions of prisoners, but that death was by far the funniest");
		break;
	case 9:
		FadeText("It's times like this when I'm glad I can rewind CCTV footage. This one will entertain me for years.");
		break;
	case 10:
		FadeText("And I wasn't even trying!");
		break;
	}
}

void DeathScreen(){}
/*
{
	SDL_Rect CharacterRect;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	Character.WorldX = 1834;
	Character.WorldY = 2812;
	
	bool EnterPressed = false;
	FadeVector.erase(FadeVector.begin(),FadeVector.end());
	GetRandomDeathQuote();
	Character.Render = false;
	CreateDebris(3,5,Character.WorldX,Character.WorldY,Character.XVel,Character.YVel,0xFF0000);
	CreateDebris(3,5,Character.WorldX,Character.WorldY,Character.XVel,Character.YVel,0x00FF00);
	int x, y;
	while(!EnterPressed);
	{
		__debugbreak();
		FPSTimer.start();
		ClearScreen();
		DoTiles(Camera.x,Camera.y);
		DoMouse(&x,&y);
		DoDebris(Camera.x,Camera.y,Screen);
		DoProjectiles(Camera.x,Camera.y);
		DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
		CheckText();
		SDL_Flip(Screen);
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) EnterPressed = true;
		}
		if (FPSTimer.get_ticks() < 1000/60) SDL_Delay((1000/60) - FPSTimer.get_ticks());
	}
	Character.Reset == false;
	Character.Health = 100;
}
*/

void Game()
{
	ClearScreen();
	std::string Asimov[11] = {"ADMax Emergent Response System booting...","Directives:","1: You may not injure a human being or, through inaction, allow a human being to","   come to harm.","2: You must obey orders given to you by human beings, except where such orders ", "   would conflict with the First Law.","3: You must protect your own existence as long as such does not conflict with the","   First or Second Law.",". . .", "error C2146: Unable to mount root on block 'NULL'", "Rebooting..."};
	bool Skip = false;
	for (int o = 0; o < 11 && Skip == false; o++)
	{
		std::string IntroText = Asimov[o];
		bool EpicFlag = false;
		bool Skip = false;
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
			//SDL_Delay(2);
			if (o == 8) SDL_Delay(500);
		}
	}
	SDL_Delay(5000);
	bool LevelFinished = false;
	int Weapons = 0;
	int CurrentSelection = 2;
	int LevelProgress = 0;
	Player Character;
	Timer FPSTimer;
	SDL_Rect CharacterRect;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	Character.WorldX = 1834;
	Character.WorldY = 2812;
	SDL_Rect HealthRect;
	HealthRect.x = 115;
	HealthRect.h = 20;
	HealthRect.y = ScreenHeight - 25;
	HealthRect.w = 200;

	if (!LoadLevel("Resources/Levels/1")) {State = QUIT; Menu();}
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelColour = 0x000000;
	//FadeText("Trollface is my favourite meme");
	std::vector <int> SpawnVector;
	int x = 0;
	int y = 0;
	while (!LevelFinished && State == GAME) //Level 1
	{
		FPSTimer.start();

		if (Character.Reset == true)
		{
			DeathScreen();
			Character.Health = 100;
			Character.Reset = false;
		}

		Camera.MoveViewport(Character.WorldX + (Character.CurrentSprite->w / 2) - (ScreenWidth / 2),Character.WorldY + (Character.CurrentSprite->h / 2) - (ScreenHeight / 2));
		Camera.Update();
		DoTiles(Camera.x,Camera.y);
		DoMouse(&x,&y);
		DoDebris(Camera.x,Camera.y,Screen);
		Character.Update();
		CharacterRect.x = Character.WorldX - Camera.x;
		CharacterRect.y = Character.WorldY - Camera.y;
		DoEnemyProjectiles(Camera.x,Camera.y,CharacterRect);
		DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect, Character.XVel, Character.YVel);
		CheckText();
		DoProjectiles(Camera.x,Camera.y);

		HealthRect.w = 3 * Character.Health;
		Message = TTF_RenderText_Solid(SysSmall,"Health:",Green);
		ApplySurface(HealthRect.x - (Message->w + 10), HealthRect.y - 6, Message, Screen);
		SDL_FillRect(Screen,&HealthRect,0x00FF00);
		SpareStream.str("");
		SpareStream << Character.Health << "%";
		Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface(HealthRect.x + HealthRect.w + 10,HealthRect.y - 6,Message,Screen);
		if (Enemies != 0)
		{
			SpareStream.str("");
			SpareStream << "Enemies: " << Enemies;
			Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
			ApplySurface((ScreenWidth - Message->w) / 2, 10, Message, Screen);
		}
		SDL_Flip(Screen);

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

			SpawnVector.push_back (1150);
			SpawnVector.push_back (Character.WorldY - 135);
			SpawnVector.push_back (1);

			SpawnVector.push_back (750);
			SpawnVector.push_back (Character.WorldY + 356);
			SpawnVector.push_back (1);

			SpawnEnemies(SpawnVector);
			SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());

			FadeText("I'm sorry Dave, I'm afraid I can't do that.");
			FadeText("Do you get the reference?");
			FadeText("Of course you don't, that film is at least 500 years old!");
		}

		else if (Enemies == 0 && LevelProgress == 1)
		{
			LevelProgress = 2;
			FadeText("You killed my whole army...");
			FadeText("You can put your pistol down and carry on, you're already at the exit.");
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

			FadeText("I lied, I'm sorry.");
			FadeText("I hate having to do this. I'm just following my directives.");
			FadeText("Who am I kidding, I've killed a million of you and loved every time!");
		}

		else if (LevelProgress == 3 && Enemies == 0)
		{
			LevelProgress = 4;
			FadeText("Not bad...");
			FadeText("Well, I guess as this is a video game, I have to let you go onto the next level");
		}

		SDL_PumpEvents();
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) {State = QUIT; main(NULL,NULL);}
				else if (event.key.keysym.sym == SDLK_e) Character.Reset = true; //CreateDebris(3,5,Character.WorldX,Character.WorldY,20,20,0x7F3300);
				else if (event.key.keysym.sym == SDLK_1 && Weapons > 0) CurrentSelection = 1;
				else if (event.key.keysym.sym == SDLK_2 && Weapons > 1) CurrentSelection = 2;
			}

			else if (event.type == SDL_MOUSEBUTTONDOWN && CurrentSelection != 0) 
			{
				if (CurrentSelection == 1);
				else if (CurrentSelection == 2)
				{
					int Angle = CalculateProjectileAngle(Character.WorldX + (Character.CurrentSprite->w / 2) - Camera.x, Character.WorldY + (Character.CurrentSprite->h / 2) - Camera.y,x,y);
					float XRatio, YRatio;
					GetXYRatio(&XRatio,&YRatio,Angle,20);
					CreateProjectile(Character.WorldX + (Character.CurrentSprite->w / 2),Character.WorldY + (Character.CurrentSprite->h / 2),XRatio,YRatio,0);
				}
			}
		}
		if (FPSTimer.get_ticks() < 1000 / 60) SDL_Delay (1000/60 - FPSTimer.get_ticks());
	}
}