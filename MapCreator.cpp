#include"MapCreator.h"
#include"DoTiles.h"
#include"LoadLevel.h"
#include<fstream>

int GetNumber(std::string String, int Min)
{
	bool Done = false;
	int Increment = 1;
	int ReturnThis = Min;
	while (!Done)
	{
		ClearScreen();
		SpareStream.str("");
		SpareStream << String << ReturnThis;
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
				else if (event.key.keysym.sym == SDLK_RIGHT) ReturnThis += Increment;
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					if (ReturnThis - Increment < Min) ReturnThis = Min; 
					else ReturnThis -= Increment;
				}
				else if (event.key.keysym.sym == SDLK_RETURN) Done = true;
			}
		}
		SDL_Delay(20);
		SDL_Flip(Screen);
	}
	return ReturnThis;
}

void MapCreator()
{
	bool Done = false;
	bool Grid = false;
	bool Delete = false;
	int Increment = 1;
	LevelColour = 0x0F0FFF;
	SDL_ShowCursor(SDL_ENABLE);

	if (!LoadLevel("Level"))
	{
		LevelWidth = GetNumber("Width: ", 1000);
		LevelHeight = GetNumber("Height: ", 1000);
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
		if (Keystates[SDLK_RIGHT]) CameraX += 8;
		if (Keystates[SDLK_LEFT]) CameraX -= 8;
		if (Keystates[SDLK_DOWN]) CameraY += 8;
		if (Keystates[SDLK_UP]) CameraY -= 8;
		EditorCamera.MoveViewport(CameraX,CameraY);
		EditorCamera.Update();
		CameraX = EditorCamera.TargetX;
		CameraY = EditorCamera.TargetY;


		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_GetMouseState(&TempX, &TempY);
				if (Delete)
				{
					for (int m = 0; m < LevelVector.size(); m++)
					{
						if (InBetween(LevelVector.at(m).WorldX, TempX + EditorCamera.x, LevelVector.at(m).WorldX + LevelVector.at(m).Width)
							&& InBetween(LevelVector.at(m).WorldY, TempY + EditorCamera.y, LevelVector.at(m).WorldY + LevelVector.at(m).Height))
						{
							LevelVector.erase(LevelVector.begin() + m);
							break;
						}
					}
				}

				else
				{

					bool Done2 = false;

					if (Grid)
					{
						TempX = TempX - TempX % 20;
						TempY = TempY - TempY % 20;
					}

					int CurrentX = 0;
					int CurrentY = 0;

					while (!Done2)
					{
						SDL_GetMouseState(&CurrentX, &CurrentY);

						if (Grid)
						{
							CurrentX = CurrentX - CurrentX % 20;
							CurrentY = CurrentY - CurrentY % 20;
						}

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

						DoTiles(EditorCamera.x, EditorCamera.y);
						SpareStream.str("");
						SpareStream << "X: " << EditorCamera.x + TempRect.x << " Y: " << EditorCamera.y + TempRect.y << " W: " << TempRect.w << " H: " << TempRect.h;
						Message = TTF_RenderText_Solid(SysSmall, SpareStream.str().c_str(), Green);
						SDL_FillRect(Screen, &TempRect, 0xFFFFFF);
						ApplySurface(0, 0, Message, Screen);
						SDL_Flip(Screen);
						ClearScreen();
						SDL_PumpEvents();
						while (SDL_PollEvent(&event))
						{
							if (event.type == SDL_MOUSEBUTTONDOWN)
							{
								Done2 = true;
								CreateTile(EditorCamera.x + TempRect.x, EditorCamera.y + TempRect.y, TempRect.w, TempRect.h);
							}
							else if (event.type == SDL_KEYDOWN)
							{
								if (event.key.keysym.sym == SDLK_ESCAPE) Done2 = true;
							}
						}
					}
				}
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) Done = true;

				else if (event.key.keysym.sym == SDLK_g)
				{
					Grid = !Grid;
					SpareStream.str("");
					SpareStream << "Grid mode: " << Grid;
					DebugWindow(SpareStream.str().c_str());
				}

				else if (event.key.keysym.sym == SDLK_d)
				{
					Delete = !Delete;
					SpareStream.str("");
					SpareStream << "Delete mode: " << Delete;
					DebugWindow(SpareStream.str().c_str());
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					std::ofstream Output;
					Output.open("Level");
					Output << LevelWidth << std::endl << LevelHeight << std::endl << LevelColour << std::endl;
					for (int i = 0; i < LevelVector.size(); i++) Output << LevelVector.at(i).WorldX << std::endl <<  LevelVector.at(i).WorldY << std::endl << LevelVector.at(i).Width << std::endl << LevelVector.at(i).Height << std::endl;
					Done = true;
				}

				else if (event.key.keysym.sym == SDLK_n)
				{
					SDL_Rect AddThis;
					AddThis.x = GetNumber("x: ", 0);
					AddThis.y = GetNumber("y: ", 0);
					AddThis.w = GetNumber("w: ", 1);
					AddThis.h = GetNumber("h: ", 1);
					CreateTile(AddThis);
				}

				else if (event.key.keysym.sym == SDLK_z)
				{
					LevelVector.erase(LevelVector.end() - 1);
				}
			}
		}
		DoTiles(EditorCamera.x,EditorCamera.y);
		SDL_GetMouseState(&TempX,&TempY);
		SpareStream.str("");
		SpareStream << "Camera X: " << EditorCamera.x << " Camera Y: " << EditorCamera.y << " MouseX: " << EditorCamera.x + TempX << " MouseY: " << EditorCamera.y + TempY;
		Message =TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
		ApplySurface(1920 - Message->w - 10, 0, Message, Screen);
		PrintDebugWindow(SysSmall,Green,Screen);
		SDL_Flip(Screen);
		ClearScreen();
		SDL_Delay(10);
	}
	SDL_ShowCursor(SDL_DISABLE);
}
