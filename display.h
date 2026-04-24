#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

void printLine();
void printHeader(string title);
void printSourceCode(string source);
void printTokens(vector<Token> tokens);
void runProgram(string source, string title);

#endif