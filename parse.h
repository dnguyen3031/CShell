#ifndef PARSE_H
#define PARSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

typedef struct
{
   char *token;
   char *args[10];
   int numArgs;
   char *redirectIn;
   char *redirectOut;
} cmdStage;

typedef struct
{
   int numStages;
} allStages;

int getInput(cmdStage *stages, allStages *stageData);

void freeEverything(cmdStage *stages, int maxChildren);

void freeAll(cmdStage *stages, int currentStage);

#endif