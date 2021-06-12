#include "pipeline.h"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

int openFile(const char *fileName, const char *mode,
   cmdStage *stages, int maxChildren)
{
   int fd, flags;

   if (0 == strcmp("r", mode))
      flags = O_RDONLY;
   else if (0 == strcmp("w", mode))
      flags = O_WRONLY | O_CREAT | O_TRUNC;
   else {
      fprintf(stderr, "Unknown openFile mode %s\n", mode);
      exit(EXIT_FAILURE);
   }

   if (-1 == (fd = open(fileName, flags, 0600)))
   {
      fprintf(stderr, "cshell: %s: ", fileName);
      perror(NULL);
      freeEverything(stages, maxChildren);
      exit(EXIT_FAILURE);
   }

   return fd;
}

void newPipe(int *pipeVal)
{
   if(pipe(pipeVal) == -1)
   {
      perror("Pipe creation failed.\n");
      exit(EXIT_FAILURE);
   }
}

void parentFunc(int *pipeVal, int *prevChildRead)
{
   if (*prevChildRead != 0)
      close(*prevChildRead);

   *prevChildRead = pipeVal[0];

   close(pipeVal[1]);
}

void closePipeIf(int maxChildren, int pipeVal)
{
   if (maxChildren != 1)
      close(pipeVal);
}

void childFunc(cmdStage *stages, int *prevChildRead,
   int maxChildren, int currentLoop, int *pipeVal)
{
   if (stages[currentLoop].redirectOut != NULL)
   {
      closePipeIf(maxChildren, pipeVal[1]);

      dup2(openFile(stages[currentLoop].redirectOut, "w",
         stages, maxChildren), STDOUT_FILENO);
   }
   else if ((maxChildren != currentLoop+1) && maxChildren != 1)
      dup2(pipeVal[1], STDOUT_FILENO);

   if (stages[currentLoop].redirectIn != NULL)
   {
      closePipeIf(maxChildren, *prevChildRead);

      dup2(openFile(stages[currentLoop].redirectIn, "r",
         stages, maxChildren), STDIN_FILENO);
   }
   else if (*prevChildRead != 0 && maxChildren != 1)
      dup2(*prevChildRead, STDIN_FILENO);

   closePipeIf(maxChildren, pipeVal[0]);

   execvp(stages[currentLoop].args[0], stages[currentLoop].args);
   fprintf(stderr, "cshell: %s: ", stages[currentLoop].args[0]);
   perror(NULL);
   freeEverything(stages, maxChildren);
   exit(EXIT_FAILURE);
}

void coreLogic(cmdStage *stages, allStages stageData,
   int *prevChildRead, int currentLoop)
{
   int pipeVal[2];
   pid_t pid=0;

   if (stageData.numStages != 1)
      newPipe(pipeVal);

   if ((pid = fork()) < 0)
      perror(NULL);
   else if (pid == 0) 
      childFunc(stages, prevChildRead,
         stageData.numStages, currentLoop, pipeVal);
   else if (stageData.numStages != 1)
      parentFunc(pipeVal, prevChildRead);
}