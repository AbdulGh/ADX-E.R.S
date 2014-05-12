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
	SDL_Rect HealthRect;
	HealthRect.x = 200;
	HealthRect.h = 20;
	HealthRect.y = ScreenHeight - 30;
	HealthRect.w = 200;

	Player Character;
	SDL_Rect CharacterRect;
	CharacterRect.w = Character.CurrentSprite->w;
	CharacterRect.h = Character.CurrentSprite->h;
	Character.WorldX = 1834;
	Character.WorldY = 2812;
	Timer FPSTimer;
	if (!LoadLevel("Resources/Levels/1")) {State = QUIT; Menu();}
	Camera.LevelHeight = LevelHeight;
	Camera.LevelWidth = LevelWidth;
	LevelColour = 0x000000;
	//FadeText("Trollface is my favourite meme");
	std::vector <int> SpawnVector;
	while (!LevelFinished && State == GAME) //Level 1
	{
		FPSTimer.start();
		//int Jew = Character.WorldX + (Character.CurrentSprite->w / 2) - (ScreenWidth / 2);
		Camera.MoveViewport(Character.WorldX + (Character.CurrentSprite->w / 2) - (ScreenWidth / 2),Character.WorldY + (Character.CurrentSprite->h / 2) - (ScreenHeight / 2));
		Camera.Update();
		DoTiles(Camera.x,Camera.y);
		int x = 0;
		int y = 0;
		DoMouse(&x,&y);
		DoDebris(Camera.x,Camera.y,Screen);
		Character.Update();
		DoEnemyProjectiles(Camera.x,Camera.y);
		DoEnemies(Camera.x,Camera.y,Character.WorldX,Character.WorldY,CharacterRect);
		CheckText();
		DoProjectiles(Camera.x,Camera.y);

		HealthRect.w = 3 * Character.Health;
		Message = TTF_RenderText_Solid(SysSmall,"Health:",Green);
		ApplySurface(HealthRect.x - (Message->w + 10), HealthRect.y, Message, Screen);
		SDL_FillRect(Screen,&HealthRect,0x00FF00);
		SpareStream.str("");
		SpareStream << Character.Health << "%";
		Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface(HealthRect.x + HealthRect.w + 10,HealthRect.y,Message,Screen);
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

			SpawnEnemies(SpawnVector);

			FadeText("Happy Birthday Mo!");
		}

		SDL_PumpEvents();
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) {State = QUIT; main(NULL,NULL);}
				else if (event.key.keysym.sym == SDLK_e) {CreateDebris(3,5,Character.WorldX,Character.WorldY,20,20,0x7F3300); SpawnEnemies(SpawnVector);}
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