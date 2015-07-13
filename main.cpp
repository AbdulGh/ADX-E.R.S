#include"Declarations.h"
#include"Menu.h"
#include"FadeText.h"
#include"Game.h"
#include"Options.h"

#include<time.h>	


int main(int argc, char *argv[])
{
	if (State != QUIT)
	{
		srand(time(NULL));
		if (!Init())
		{
			fprintf(stderr, "Failed to initialize SDL!\n");
			fprintf(stderr, SDL_GetError());
			return 1;
		}

		if (!Load())
		{
			DebugWindow("Failed to load all files!");
		}

		if (FirstTime)
		{
			FirstTime = false;
			Options();
		}
		
		SDL_ShowCursor(SDL_DISABLE);

		SetClips();

		std::string IntroText = "ADX E.R.S";
		SDL_Surface *Message = TTF_RenderText_Solid(Sys,"_",White);
		ApplySurface((ScreenWidth - Message->w)/2,200,Message,Screen);
		SDL_Flip(Screen);
		SDL_Delay(900);
		ClearScreen();
		SDL_Flip(Screen);
		SDL_Delay(900);

		int x;
		bool Underscore = false;
		for (int i = 1; i <= IntroText.size(); i++)
		{
			std::string ApplyThis = IntroText.substr(0,i);
			Message = TTF_RenderText_Blended(Sys,ApplyThis.c_str(),White);
			x = (ScreenWidth - Message->w) / 2;
			if (i % 3 == 0) Underscore = !Underscore;
			if (Underscore)
			{
				ApplyThis += "_";
				SDL_FreeSurface(Message);
				Message = TTF_RenderText_Blended(Sys,ApplyThis.c_str(),White);
			}
			ClearScreen();
			ApplySurface(x, 200, Message, Screen);
			SDL_FreeSurface(Message);
			SDL_Flip(Screen);
			SDL_Delay(300);
		}
		Temp1 = x;

		State = Menu();
		if (State == QUIT) return 0;
		else if (State == GAME) 
		{
			LoadGameFiles();
			Game();
		}
		else Options();
	}
	return 0;
}