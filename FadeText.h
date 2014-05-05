#ifndef FADETEXT_H
#define FADETEXT_H
#include"Declarations.h"

extern std::vector <std::string> FadeVector;
extern std::vector <int> AlphaVector;
extern std::vector <int> DirectionVector;

void FadeText(std::string Text);
void CheckText();

#endif