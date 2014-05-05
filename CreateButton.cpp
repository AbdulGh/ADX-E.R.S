#include"CreateButton.h"

std::vector <Button> ButtonVector;

void CreateButton(int Wx, int Wy, int WID,SDL_Surface *WUnPressed, SDL_Surface *WPressed, int LOffset = 0)
{
	Button VectorButton;
	VectorButton.ButtonRect.h = WUnPressed->h;
	VectorButton.ButtonRect.w = WUnPressed->w;
	VectorButton.ButtonRect.x = Wx; 
	VectorButton.ButtonRect.y = Wy;
	VectorButton.ID = WID;
	VectorButton.Normal = WUnPressed;
	VectorButton.NotNormal = WPressed; //Or maybe mouseover
	VectorButton.Offset = LOffset;
	ButtonVector.push_back(VectorButton);
}

#define CURRENTBUTTON ButtonVector.at(i) 
int CurrentMouseX, CurrentMouseY = 0;
int DoButtons()
{
	bool MouseOver = false;
	bool Click = false;
	SDL_PumpEvents();
	if (ButtonVector.size() != 0)
	{
		SDL_PumpEvents();
		if (SDL_GetMouseState(&CurrentMouseX,&CurrentMouseY)&SDL_BUTTON(1)) Click = true;
		for (int i = 0; i < ButtonVector.size(); i++)
		{
			if (CURRENTBUTTON.IsMouseOver(CurrentMouseX,CurrentMouseY))
			{
				MouseOver = true;
				ApplySurface(CURRENTBUTTON.ButtonRect.x - CURRENTBUTTON.Offset,CURRENTBUTTON.ButtonRect.y - CURRENTBUTTON.Offset,CURRENTBUTTON.NotNormal,Screen);
			}
			else 
			{
				MouseOver = false;
				ApplySurface(CURRENTBUTTON.ButtonRect.x,CURRENTBUTTON.ButtonRect.y,CURRENTBUTTON.Normal,Screen);
			}
			if(Click == true && MouseOver == true) return CURRENTBUTTON.ID;
		}
	}
	return 0;
}
void ClearButtons()
{
	ButtonVector.erase(ButtonVector.begin(),ButtonVector.end());
}