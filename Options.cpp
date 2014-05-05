#include"Declarations.h"
#include"Options.h"

void Options()
{
	ClearScreen();
	Message = TTF_RenderText_Solid(Start,"OPTIONS GOES HERE",White);
	ApplySurface((ScreenWidth - Message->w) /2, (ScreenHeight - Message->h)/2,Message,Screen);
	SDL_Flip(Screen);
	SDL_Delay(2000);
}