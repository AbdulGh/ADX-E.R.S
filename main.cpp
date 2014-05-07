#include"Declarations.h"
#include"Menu.h"
#include"FadeText.h"
#include"Game.h"
#include"Options.h"

#include<time.h>	


int main(int argc, char *argv[])
{
	srand(time(NULL));
	if (!Init())
	{
		OpenDebugWindow("Failed to initalise SDL!");
		OpenDebugWindow(SDL_GetError());
		OpenDebugWindow("Open");
		return 1;
	}

	if (!Load())
	{
		OpenDebugWindow("Failed to load all files!");
		OpenDebugWindow("Open");
	}

	SDL_ShowCursor(SDL_DISABLE);
	SetClips();
	std::string IntroText = "ADX E.R.S";
	Message = TTF_RenderText_Solid(Sys,"_",White);
	ApplySurface((ScreenWidth - Message->w)/2,200,Message,Screen);
	SDL_Flip(Screen);
	SDL_Delay(900);
	ClearScreen();
	SDL_Flip(Screen);
	SDL_Delay(900);

	int x;
	bool EpicFlag = false;
	bool Skip = false;
	for (int i = 1; i <= IntroText.size() && Skip == false; i++)
	{
		std::string ApplyThis = IntroText.substr(0,i);
		Message = TTF_RenderText_Solid(Sys,ApplyThis.c_str(),White);
		x = (ScreenWidth - Message->w) / 2;
		if (i % 3 == 0) EpicFlag = !EpicFlag;
		if (EpicFlag)
		{
			ApplyThis += "_";
			Message = TTF_RenderText_Solid(Sys,ApplyThis.c_str(),White);
		}
		while(SDL_PollEvent(&event)) if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) Skip = true;
		ClearScreen();
		ApplySurface(x, 200, Message, Screen);
		SDL_Flip(Screen);
		SDL_Delay(300);
	}

	State = Menu();
	if (State == QUIT) return 0;
	else if (State == GAME) Game();
	else if (State == OPTIONS) Options();


	return 0;
}