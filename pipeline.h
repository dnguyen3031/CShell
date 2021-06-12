#ifndef PIPELINE_H
#define PIPELINE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

void coreLogic(cmdStage *stages, allStages stageData, int *prevChildRead, int currentLoop);

void oneArg(cmdStage *stages, allStages stageData);

#endif