#include "parse.h"

void trimEnd(char * str)
{
   int index=0, i=0;

   while(str[i] != '\0')
   {
      if(str[i] == '\n')
         index= i;

      i++;
   }

   if (index != 0)
      str[index] = '\0';
}

void parseElse(cmdStage *stages, int currentStage, int *current, char *token)
{
   stages[currentStage].args[*current] = malloc(strlen(token)+1);
   memcpy(stages[currentStage].args[*current], token, strlen(token)+1);
   *current += 1;
}

void parseRedirectOut(cmdStage *stages, int currentStage, char *token)
{
   trimEnd(token);
   if (stages[currentStage].redirectOut != NULL)
      free(stages[currentStage].redirectOut);
   stages[currentStage].redirectOut = malloc(strlen(token)+1);
   memcpy(stages[currentStage].redirectOut, token, strlen(token)+1);
}

void parseRedirectIn(cmdStage *stages, int currentStage, char *token)
{
   trimEnd(token);
   if (stages[currentStage].redirectIn != NULL)
      free(stages[currentStage].redirectIn);
   stages[currentStage].redirectIn = malloc(strlen(token)+1);
   memcpy(stages[currentStage].redirectIn, token, strlen(token)+1);
}

void parseArgs(cmdStage *stages, int currentStage)
{
   char *token;
   int current = 0;

   token = strtok(stages[currentStage].token, " ");

   if (!(strcmp(token, "exit")) || !(strcmp(token, "exit\n")))
      exit(EXIT_SUCCESS);

   while (token != NULL)
   {
      trimEnd(token);
      if (!strcmp(token, "<"))
      {
         token = strtok(NULL, " ");
         parseRedirectIn(stages, currentStage, token);
      }
      else if(!strcmp(token, ">"))
      {
         token = strtok(NULL, " ");
         parseRedirectOut(stages, currentStage, token);
      }
      else
         parseElse(stages, currentStage, &current, token);

      token = strtok(NULL, " ");
   }
   stages[currentStage].args[stages[currentStage].numArgs] = NULL;
}

void getNumArgs(cmdStage *stages, int currentStage)
{
   int i, hasPrint = 0;
   stages[currentStage].numArgs = 0;
   for (i=0; stages[currentStage].token[i] != '\0'; i++)
   {
      if (!isspace(stages[currentStage].token[i]) &&
         isprint(stages[currentStage].token[i]))
         hasPrint = 1;

      if (isspace(stages[currentStage].token[i])
         && !isspace(stages[currentStage].token[i+1]))
      {
         stages[currentStage].numArgs += 1;
         hasPrint = 0;
      }
   }
   stages[currentStage].numArgs += hasPrint;
}

int tooManyArgs(cmdStage *stages, int currentLoop)
{
   char *token;
   token = strtok(stages[currentLoop].token, " ");
   fprintf(stderr, "cshell: %s: Too many arguments\n", token);
   return 0;
}

int tooManyCommands(int current)
{
   if (current != 20)
      return 0;
   fprintf(stderr, "cshell: Too many commands\n");
   return 1;
}

int longCMDLine()
{
   fprintf(stderr, "cshell: Command line too long\n");
   return 0;
}

void foundEOF()
{
   printf("exit\n");
   exit(EXIT_SUCCESS);
}

int invalidPipe(char *token)
{
   int i = 0;
   while (token[i] != '\0')
   {
      if (!isspace(token[i]))
         return 0;
      i+=1;
   }
   fprintf(stderr, "cshell: Invalid pipe\n");
   return 1;
}

void whileLoop(int *current, cmdStage *stages,
   char *token, allStages *stageData)
{
   int leadTrim;

   stages[*current].redirectIn = NULL;
   stages[*current].redirectOut = NULL;
   leadTrim = 0;
   while (isspace(token[leadTrim]))
      leadTrim += 1;
   
   stages[*current].token = token + leadTrim;
      
   *current += 1;
   stageData->numStages += 1;
}

int isWhitespace(char *input)
{
   int i;
   for (i=0; i<strlen(input); i++)
      if (!isspace(input[i]))
         return 0;
   
   return 1;
}

int processInput(cmdStage *stages, allStages *stageData)
{
   int i;

   for (i = 0; i < stageData->numStages; i++)
   {
      getNumArgs(stages, i);
      if (stages[i].numArgs > 10)
         return tooManyArgs(stages, i);
   }

   for (i = 0; i < stageData->numStages; i++)
      parseArgs(stages, i);

   return 1;
}

int getInput(cmdStage *stages, allStages *stageData)
{
   char input[9999], *token;
   int current = 0;

   stageData->numStages = 0;

   printf(":-) ");

   if ((fgets(input, 9999, stdin) == NULL))
      foundEOF();
   else if (strlen(input) > 1024)
      return longCMDLine();

   if (isWhitespace(input) == 1)
      return 0;

   token = strtok(input, "|");

   while (token != NULL)
   {
      if (invalidPipe(token) == 1 || tooManyCommands(current) == 1)
         return 0;
      whileLoop(&current, stages, token, stageData);
      token = strtok(NULL, "|");
   }

   return processInput(stages, stageData);
}

void freeEverything(cmdStage *stages, int maxChildren)
{
   int i;

   for(i=0; i<maxChildren; i++)
      freeAll(stages, i);
}

void freeAll(cmdStage *stages, int currentStage)
{
   int i;

   for (i=0; i<stages[currentStage].numArgs; i++)
      free(stages[currentStage].args[i]);

   if (stages[currentStage].redirectIn != NULL)
      free(stages[currentStage].redirectIn);

   if (stages[currentStage].redirectOut != NULL)
      free(stages[currentStage].redirectOut);
}
