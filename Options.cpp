#include"Declarations.h"
#include"CreateButton.h"
#include"Options.h"

void Options()
{
	ClearButtons();

	SDL_Surface *ArrowRotated = rotateSurface90Degrees(Arrow,2); //rotozoomSurface(Arrow,3.1418,1,0);
	SDL_Surface *GreenArrowRotated = rotateSurface90Degrees(GreenArrow, 2);
	CreateButton(20, 200, 1, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(420, 200, 2, Arrow, GreenArrow, 0);
	CreateButton(20, 300, 3, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(420, 300, 4, Arrow, GreenArrow, 0);
	CreateButton(20, 400, 5, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(420, 400, 6, Arrow, GreenArrow, 0);
	CreateButton(20, 500, 7, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(420, 500, 8, Arrow, GreenArrow, 0);
	CreateButton(20, 600, 9, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(420, 600, 10, Arrow, GreenArrow, 0);

	SDL_Surface *Quit = TTF_RenderText_Blended(Start, "Exit", White);
	SDL_Surface *QuitMouseover = TTF_RenderText_Blended(Start, "Exit", Green);

	CreateButton((ScreenWidth - Quit->w) / 2, ScreenHeight - 150, 11, Quit, QuitMouseover, 0);

	int ScreenWidths[] = {1366, 1440, 1920};
	int ScreenHeights[] = {768, 900, 1080};

	int TempW = ScreenWidth, TempH = ScreenHeight;
	int Delay = 500;

	int ResolutionIncrement;
	if (ScreenWidth == 1366) ResolutionIncrement = 0;
	else if (ScreenWidth == 1440) ResolutionIncrement = 1;
	else ResolutionIncrement = 2;

	SDL_Surface *Text;
	int x = 0, y = 0;
	bool Done = false;
	while (!Done)
	{
		ClearScreen();
		DoMouse(&x, &y);
		Delay = 500;

		switch (DoButtons())
		{
			case 1: //Volume down
				if (Volume == 0) Volume = 100;
				else Volume -= 10;
				break;

			case 2: //Volume up
				Volume += 10;
				Volume %= 110;
				break;

			case 3: //Screen shake
			case 4:
				ScreenShake = !ScreenShake;
				break;

			case 5: //Step sounds
			case 6:
				StepSoundsEnabled = !StepSoundsEnabled;
				break;


			case 7: //Windowed
			case 8:
				Windowed = !Windowed;
				break;

			case 9: //Resolution down
				if (ResolutionIncrement == 0) ResolutionIncrement = 2;
				else ResolutionIncrement--;
				TempW = ScreenWidths[ResolutionIncrement];
				TempH = ScreenHeights[ResolutionIncrement];
				break;

			case 10: //Resolution up
				ResolutionIncrement++;
				ResolutionIncrement %= 3;
				TempW = ScreenWidths[ResolutionIncrement];
				TempH = ScreenHeights[ResolutionIncrement];
				break;

			case 11: //Exit
				Done = true;
				break;

			default:
				Delay = 10;
		}

		ApplyTextCentered("Options",Start,White,0,0,0,-(ScreenHeight / 2) + 50);
		if (ResolutionIncrement != 2) ApplyTextCentered("This game is best played at a 1920x1080 resolution", Small, White, 0, 0, 0, (ScreenHeight / 2) - 30);

		SpareStream.str("");
		SpareStream << "Volume: " << Volume;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w)/ 2, 200, Text, Screen);

		SpareStream.str("");
		SpareStream << "Screen shake: " << ScreenShake;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 300, Text, Screen);

		SpareStream.str("");
		SpareStream << "Step sounds: " << StepSoundsEnabled;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 400, Text, Screen);

		SpareStream.str("");
		SpareStream << "Windowed: " << Windowed;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 500, Text, Screen);

		SpareStream.str("");
		SpareStream << "Resolution: " << TempW << " x " << TempH;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 600, Text, Screen);

		SDL_Flip(Screen);
		SDL_Delay(Delay);
	}

	ScreenWidth = TempW;
	ScreenHeight = TempH;

	std::ofstream SettingsOutput;
	SettingsOutput.open("Resources/Settings", std::ofstream::trunc);
	SettingsOutput << Volume << std::endl;
	SettingsOutput << ScreenShake << std::endl;
	SettingsOutput << StepSoundsEnabled << std::endl;
	SettingsOutput << Windowed << std::endl;
	SettingsOutput << ScreenWidth << std::endl;
	SettingsOutput << ScreenHeight << std::endl;
	SettingsOutput << 0;
	SettingsOutput.close();

	SDL_FreeSurface(ArrowRotated);
	SDL_FreeSurface(Quit);
	SDL_FreeSurface(GreenArrowRotated);
	SDL_FreeSurface(QuitMouseover);
	SDL_FreeSurface(Text);
	ButtonVector.clear();
	QuitGame(true);
}