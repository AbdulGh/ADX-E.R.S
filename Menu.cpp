#include"Menu.h"
#include"Declarations.h"
#include"CreateButton.h"

Gamestate Menu()
{	
	SDL_Surface *NewGame = TTF_RenderText_Blended(Start,"New Game",White);
	SDL_Surface *NewGameMouseover = TTF_RenderText_Blended(Start,"New Game",Green);
	SDL_Surface *Options = TTF_RenderText_Blended(Start,"Options",White);
	SDL_Surface *OptionsMouseover = TTF_RenderText_Blended(Start,"Options",Green);
	SDL_Surface *Quit = TTF_RenderText_Blended(Start,"Exit",White);
	SDL_Surface *QuitMouseover = TTF_RenderText_Blended(Start,"Exit",Green);

	ClearButtons();
	CreateButton(100,ScreenHeight - 200,1,NewGame,NewGameMouseover,0);
	CreateButton((ScreenWidth - Options->w)/2,ScreenHeight - 200,2,Options,OptionsMouseover,0);
	CreateButton(ScreenWidth - Quit->w - 150,ScreenHeight - 200,3,Quit,QuitMouseover,0);

	bool Underscore = false;
	int Inc = 0;

	SDL_Surface *Message = TTF_RenderText_Blended(Sys, "ADX E.R.S", White);
	SDL_Surface *Message2 = TTF_RenderText_Blended(Sys, "ADX E.R.S_", White);

	while(true)
	{
		ClearScreen();
		
		Inc++;
		if (Inc == 80)
		{
			Underscore = !Underscore;
			Inc = 0;
		}

		if (Underscore) ApplySurface(Temp1, 200, Message2, Screen);
		else ApplySurface(Temp1, 200, Message, Screen);

		int x,y;
		DoMouse(&x,&y);
		int Clicked = DoButtons();

		if (Clicked != 0)
		{
			switch (Clicked)
			{
			case 1: return GAME;
			case 2: return OPTIONS;
			case 3: return QUIT;
			}
		}

		SDL_Flip(Screen);
		while(SDL_PollEvent(&event))
		{
			//if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) MapCreator();
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return QUIT;
		}
		SDL_Delay(10);
	}

	ClearButtons();
	SDL_FreeSurface(Message);
	SDL_FreeSurface(Message2);

}