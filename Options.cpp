#include"Declarations.h"
#include"CreateButton.h"
#include"Options.h"

void Options()
{
	ClearButtons();

	SDL_Surface *ArrowRotated = rotateSurface90Degrees(Arrow,2); //rotozoomSurface(Arrow,3.1418,1,0);
	SDL_Surface *GreenArrowRotated = rotateSurface90Degrees(GreenArrow, 2);

	SDL_Surface *ScreenShakeNormal = TTF_RenderText_Blended(Small, "Screen Shake:", White);
	SDL_Surface *ScreenShakeOver = TTF_RenderText_Blended(Small, "Screen Shake:", Green);
	SDL_Surface *StepSoundsNormal = TTF_RenderText_Blended(Small, "Step Sounds:", White);
	SDL_Surface *StepSoundsOver = TTF_RenderText_Blended(Small, "Step Sounds:", Green);
	SDL_Surface *WindowedNormal = TTF_RenderText_Blended(Small, "Windowed:", White);
	SDL_Surface *WindowedOver = TTF_RenderText_Blended(Small, "Windowed:", Green);
	SDL_Surface *ExitNormal = TTF_RenderText_Blended(Start, "Exit", White);
	SDL_Surface *Resolution1 = TTF_RenderText_Blended(Small, "1366 x 768", White);
	SDL_Surface *Resolution2 = TTF_RenderText_Blended(Small, "1440 x 900", White);
	SDL_Surface *Resolution3 = TTF_RenderText_Blended(Small, "1920 x 1080", White);
	SDL_Surface *Resolution1O = TTF_RenderText_Blended(Small, "1366 x 768", Green);
	SDL_Surface *Resolution2O = TTF_RenderText_Blended(Small, "1440 x 900", Green);
	SDL_Surface *Resolution3O = TTF_RenderText_Blended(Small, "1920 x 1080", Green);
	SDL_Surface *ExitOver = TTF_RenderText_Blended(Start, "Exit", Green);

	CreateButton(ScreenWidth / 2.2, 200, 1, ArrowRotated, GreenArrowRotated, 0);
	CreateButton(ScreenWidth - ScreenWidth / 2.2, 200, 2, Arrow, GreenArrow, 0);
	CreateButton((ScreenWidth - ScreenShakeNormal->w) / 2, 300, 3, ScreenShakeNormal, ScreenShakeOver, 0);
	CreateButton((ScreenWidth - StepSoundsNormal->w) / 2, 400, 4, StepSoundsNormal, StepSoundsOver, 0);
	CreateButton((ScreenWidth - WindowedNormal->w) / 2, 500, 5, WindowedNormal, WindowedOver, 0);
	CreateButton((ScreenWidth - Resolution1->w) / 3, 600, 6, Resolution1, Resolution1O, 0);
	CreateButton((ScreenWidth - Resolution2->w) / 2 , 600, 7, Resolution2, Resolution2O, 0);
	CreateButton((2 * ScreenWidth) / 3 - (2 * Resolution1->w) / 3  /*HELP*/, 600, 8, Resolution3, Resolution3O, 0);
	CreateButton((ScreenWidth - ExitNormal->w) / 2, ScreenHeight - 150, 9, ExitNormal, ExitOver, 0);

	int TempW = ScreenWidth, TempH = ScreenHeight;
	int Delay = 200;

	SDL_Rect SelectedRes;
	SelectedRes.w = 10;
	SelectedRes.h = 10;
	SelectedRes.y = 603;

	if (TempW == 1366) SelectedRes.x = (ScreenWidth - Resolution1->w) / 3 - 20;
	else if (TempW == 1440) SelectedRes.x = (ScreenWidth - Resolution2->w) / 2 - 20;
	else SelectedRes.x = (2 * ScreenWidth) / 3 - (2 * Resolution1->w) / 3 - 20;

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
				ScreenShake = !ScreenShake;
				break;

			case 4: //Step sounds
				StepSoundsEnabled = !StepSoundsEnabled;
				break;

			case 5: //Windowed
				Windowed = !Windowed;
				break;

			case 6: //Resolutions
				TempW = 1366;
				TempH = 768;
				SelectedRes.x = (ScreenWidth - Resolution1->w) / 3 - 20;
				break;

			case 7: 
				TempW = 1440;
				TempH = 900;
				SelectedRes.x = (ScreenWidth - Resolution2->w) / 2 - 20;
				break;

			case 8:
				TempW = 1920;
				TempH = 1080;
				SelectedRes.x = (2 * ScreenWidth) / 3 - (2 * Resolution1->w) / 3 - 20;
				break;

			case 9: //Exit
				Done = true;
				break;

			default:
				Delay = 10;
		}

		ApplyTextCentered("Options",Start,White,0,0,0,-(ScreenHeight / 2) + 50);
		if (TempW != 1920) ApplyTextCentered("This game is best played at a 1920x1080 resolution", Small, White, 0, 0, 0, (ScreenHeight / 2) - 30);

		SpareStream.str("");
		SpareStream << "Volume: " << Volume;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface((ScreenWidth - Text->w) / 2, 200, Text, Screen);
		SDL_FreeSurface(Text);

		Text = TTF_RenderText_Solid(Small, std::to_string(ScreenShake).c_str(), White);
		ApplySurface((ScreenWidth - ScreenShakeNormal->w) / 2 + ScreenShakeNormal->w + 10, 300, Text, Screen);
		SDL_FreeSurface(Text);
		
		Text = TTF_RenderText_Solid(Small, std::to_string(StepSoundsEnabled).c_str(), White);
		ApplySurface((ScreenWidth - StepSoundsNormal->w) / 2 + StepSoundsNormal->w + 10, 400, Text, Screen);
		SDL_FreeSurface(Text);

		Text = TTF_RenderText_Solid(Small, std::to_string(Windowed).c_str(), White);
		ApplySurface((ScreenWidth - WindowedNormal->w) / 2 + WindowedNormal->w + 10, 500, Text, Screen);
		SDL_FreeSurface(Text);

		SDL_FillRect(Screen, &SelectedRes, 0x00FF00);

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

	ClearButtons();
	/*SDL_FreeSurface(ArrowRotated);
	SDL_FreeSurface(Quit);
	SDL_FreeSurface(GreenArrowRotated);
	SDL_FreeSurface(QuitMouseover);
	SDL_FreeSurface(Text);*/
	QuitGame(true);
}