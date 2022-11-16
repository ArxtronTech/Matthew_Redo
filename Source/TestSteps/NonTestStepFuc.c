#include <ansi_c.h>
#include <utility.h>
#include "StructureDefine.h"
#include "NonTestStepFuc.h"

// Global variables
static double glbTestTimer; //why I can't use extern here

/***************************************************************************//*!
* \brief 
*******************************************************************************/
void ParseLogs (char *LogPath, LogStruct *ParsedResults)
{
	FILE* fp = fopen (LogPath, "r");
	
}


/***************************************************************************//*!
* \brief 
*******************************************************************************/
void settesttimer (void)
{
	glbTestTimer = Timer();
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
void stoptesttimer (double *timeelapsed)
{
	*timeelapsed = Timer()-glbTestTimer;
}