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
	char buff[1024];
	
	while (buff)
	{
		memset(buff,0,1024);
		fgets(buff,1024,fp);
		strtok(buff,"\n");
		strcpy(ParsedResults->TestName, strtok(buff, ","));
		ParsedResults->TestNum = atoi(strtok(NULL, ","));
		strcpy(ParsedResults->TestVal,strtok(NULL, ","));
		strcpy(ParsedResults->TestLoLim,strtok(NULL,","));
		strcpy(ParsedResults->TestHiLim,strtok(NULL,","));
		ParsedResults++;
	}
	
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