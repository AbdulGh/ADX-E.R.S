#include"MapCreator.h"
#include"DoTiles.h"
#include"LoadLevel.h"
#include<fstream>

void MapCreator()
{
	bool Done = false;
	int Increment = 1;
	LevelColour = 0x0F0FFF;
	if (!LoadLevel("Level"))
	{
		while (!Done)
		{
			ClearScreen();
			SpareStream.str("");
			SpareStream << "Width: " << LevelWidth;
			Message = TTF_RenderText_Solid(Start,SpareStream.str().c_str(),White);
			ApplySurface((ScreenWidth - Message->w) / 2, 300, Message, Screen);
			SpareStream.str("");
			SpareStream << "Increment: " << Increment;
			Message = TTF_RenderText_Solid(Start,SpareStream.str().c_str(),White);
			ApplySurface((ScreenWidth - Message->w) / 2, 600, Message, Screen);
			while(SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_UP && Increment != 1000) Increment *= 10;
					else if (event.key.keysym.sym == SDLK_DOWN && Increment != 1) Increment /= 10;
					else if (event.key.keysym.sym == SDLK_RIGHT) LevelWidth += Increment;
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						if (LevelWidth - Increment < ScreenWidth) LevelWidth = ScreenWidth; 
						else LevelWidth -= Increment;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) Done = true;
				}
			}
			SDL_Delay(20);
			SDL_Flip(Screen);
		}
		Done = false;
		LevelHeight = 2000;
		Increment = 1;
		while (!Done)
		{
			ClearScreen();
			SpareStream.str("");
			SpareStream << "Height: " << LevelHeight;
			Message = TTF_RenderText_Solid(Start,SpareStream.str().c_str(),White);
			ApplySurface((ScreenWidth - Message->w) / 2, 300, Message, Screen);
			SpareStream.str("");
			SpareStream << "Increment: " << Increment;
			Message = TTF_RenderText_Solid(Start,SpareStream.str().c_str(),White);
			ApplySurface((ScreenWidth - Message->w) / 2, 600, Message, Screen);
			while(SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_UP && Increment != 1000) Increment *= 10;
					else if (event.key.keysym.sym == SDLK_DOWN && Increment != 1) Increment /= 10;
					else if (event.key.keysym.sym == SDLK_RIGHT) LevelHeight += Increment;
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						if (LevelHeight - Increment < ScreenHeight) LevelHeight = ScreenHeight;
						else LevelHeight -= Increment;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) Done = true;
				}
			}
			SDL_Flip(Screen);
			SDL_Delay(20);
		}
		CreateTile(0,0,20,LevelHeight);
		CreateTile(0,0,LevelWidth,20);
		CreateTile(LevelWidth - 20,0,20,LevelHeight);
		CreateTile(0,LevelHeight - 20,LevelWidth,20);
	}
	Viewport EditorCamera;
	EditorCamera.LevelHeight = LevelHeight;
	EditorCamera.LevelWidth = LevelWidth;
	EditorCamera.x = 0;
	EditorCamera.y = 0;
	Done = false;
	int CameraX = 0;
	int CameraY = 0;
	int TempX = 0;
	int TempY = 0;
	int TempW = 0;
	int TempH = 0;
	while(!Done)
	{
		Uint8 *Keystates = SDL_GetKeyState(NULL);
		if (Keystates[SDLK_RIGHT]) CameraX+=1;
		if (Keystates[SDLK_LEFT]) CameraX-=1;
		if (Keystates[SDLK_DOWN]) CameraY+=1;
		if (Keystates[SDLK_UP]) CameraY-=1;
		EditorCamera.MoveViewport(CameraX,CameraY);
		EditorCamera.Update();
		CameraX = EditorCamera.TargetX;
		CameraY = EditorCamera.TargetY;
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				bool Done2 = false;
				SDL_GetMouseState(&TempX,&TempY);
				while(!Done2)
				{
					int CurrentX = 0;
					int CurrentY = 0;
					SDL_GetMouseState(&CurrentX, &CurrentY);

					SDL_Rect TempRect;
					TempRect.x = TempX;
					TempRect.y = TempY;
					TempRect.w = TempW;
					TempRect.h = TempH;

					TempW = CurrentX - TempX;
					if (TempW < 0) 
					{
						TempW *= -1;
						TempRect.x = CurrentX;
					}

					TempH = CurrentY - TempY;
					if (TempH < 0)
					{
						TempH *= -1;
						TempRect.y = CurrentY;
					}

					DoTiles(EditorCamera.x,EditorCamera.y);
					SpareStream.str("");
					SpareStream << "X: " << EditorCamera.x + TempRect.x << " Y: " << EditorCamera.y + TempRect.y << " W: " << TempRect.w << " H: " << TempRect.h;
					Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
					SDL_FillRect(Screen,&TempRect,0xFFFFFF);
					ApplySurface(0,0,Message,Screen);
					SDL_Flip(Screen);
					SDL_PumpEvents();
					while(SDL_PollEvent(&event))
					{
						if(event.type == SDL_MOUSEBUTTONDOWN)
						{
							Done2 = true;
							CreateTile(EditorCamera.x + TempRect.x,EditorCamera.y + TempRect.y,TempRect.w,TempRect.h);
						}
						else if (event.type == SDL_KEYDOWN)
						{
							if (event.key.keysym.sym == SDLK_ESCAPE) Done2 = true;
						}
					}
				}
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) Done = true;
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					std::ofstream Output;
					Output.open("Level");
					Output << LevelWidth << std::endl << LevelHeight << std::endl << LevelColour << std::endl;
					for (int i = 0; i < LevelVector.size(); i++) Output << LevelVector.at(i).WorldX << std::endl <<  LevelVector.at(i).WorldY << std::endl << LevelVector.at(i).Width << std::endl << LevelVector.at(i).Height << std::endl;
					Done = true;
				}
				else if (event.key.keysym.sym == SDLK_n) __debugbreak();
			}
		}
		DoTiles(EditorCamera.x,EditorCamera.y);
		SDL_GetMouseState(&TempX,&TempY);
		SpareStream.str("");
		SpareStream << "Camera X: " << EditorCamera.x << " Camera Y: " << EditorCamera.y << " MouseX: " << EditorCamera.x + TempX << " MouseY: " << EditorCamera.y + TempY;
		Message =TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface(0,0,Message,Screen);
		SDL_Flip(Screen);
		SDL_Delay(10);
	}
}
