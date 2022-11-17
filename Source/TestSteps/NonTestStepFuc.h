#include "StructureDefine.h"

#ifndef __NONTESTSTEPFUC_H__
#define __NONTESTSTEPFUC_H__

//define non-test function
int ParseLogs (char *LogPath, LogStruct *ParsedResults);
void settesttimer (void);
void stoptesttimer (double *timeelapsed);
void ConvertIntToString(char *arr);
#endif