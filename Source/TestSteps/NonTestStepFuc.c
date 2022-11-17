#include <ansi_c.h>
#include <utility.h>
#include "StructureDefine.h"
#include "NonTestStepFuc.h"

// Global variables
static double glbTestTimer; //why I can't use extern here

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int ParseLogs (char *LogPath, LogStruct *ParsedResults)
{
	TSInit;
	FILE* fp = fopen (LogPath, "r");
	tsErrChk(!fp,"File not open");
	
Error:
	return error;
	
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