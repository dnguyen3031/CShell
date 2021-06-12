#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parse.h"
#include "pipeline.h"

void childWait(int numChildren)
{
   int i, status;

   for (i = 0; i < numChildren; i++)
      wait(&status);
}

void mainElse(int *prevChildRead, cmdStage *stages,
   allStages stageData)
{
   int i;

   for (i=0; i<stageData.numStages; i++)
      coreLogic(stages, stageData, prevChildRead, i);

   if (stageData.numStages != 1)
      close(*prevChildRead);

   *prevChildRead = 0;
}

int main(int argc, char **argv)
{
   int prevChildRead = 0, getInputCheck = 0;
   cmdStage stages[20];
   allStages stageData;

   setbuf(stdout, NULL);

   while(1)
   {
      memset(stages, 0, 20*sizeof(cmdStage));

      while (getInputCheck != 1)
         getInputCheck = getInput(stages, &stageData);
         
      mainElse(&prevChildRead, stages, stageData);

      freeEverything(stages, stageData.numStages);

      childWait(stageData.numStages);

      getInputCheck = 0;
   }
   
   return 0;
}