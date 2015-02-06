#include"Declarations.h"
#include"Options.h"

void Options()
{
	ClearScreen();
	ApplyTextCentered("OPTIONS GO HERE", Start, White);
	SDL_Flip(Screen);
	SDL_Delay(2000);
}