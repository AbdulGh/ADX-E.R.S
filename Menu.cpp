#include"Menu.h"
#include"Declarations.h"
#include"CreateButton.h"

Gamestate Menu()
{
	SDL_Surface *NewGame = TTF_RenderText_Solid(Start,"New Game",White);
	SDL_Surface *NewGameMouseover = TTF_RenderText_Solid(Start,"New Game",Green);
	SDL_Surface *Options = TTF_RenderText_Solid(Start,"Options",White);
	SDL_Surface *OptionsMouseover = TTF_RenderText_Solid(Start,"Options",Green);
	SDL_Surface *Quit = TTF_RenderText_Solid(Start,"Exit",White);
	SDL_Surface *QuitMouseover = TTF_RenderText_Solid(Start,"Exit",Green);

	ClearButtons();
	CreateButton(100,ScreenHeight - 200,1,NewGame,NewGameMouseover,0);
	CreateButton((ScreenWidth - Options->w)/2,ScreenHeight - 200,2,Options,OptionsMouseover,0);
	CreateButton(ScreenWidth - Quit->w - 150,ScreenHeight - 200,3,Quit,QuitMouseover,0);

	Message = TTF_RenderText_Solid(Sys,"ADX E.R.S_",White);
	int x = (ScreenWidth - Message->w)/2;

	bool EpicFlag = false;
	int Inc = 0;
	while(true)
	{
		Inc++;
		if (Inc > 60000) Inc = 0;
		ClearScreen();
		if ( Inc % 60 == 0 ) EpicFlag = !EpicFlag;
		if (EpicFlag) Message = TTF_RenderText_Solid(Sys,"ADX E.R.S",White);
		else Message = TTF_RenderText_Solid(Sys,"ADX E.R.S_",White);
		ApplySurface(x,200,Message,Screen);
		int x,y;
		DoMouse(&x,&y);
		int Clicked = DoButtons();
		switch(Clicked)
		{
		case 1: return GAME;
		case 2: return OPTIONS;
		case 3: return QUIT;
		}
		SDL_Flip(Screen);
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) MapCreator();
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return QUIT;
		}
		SDL_Delay(10);
	}
}