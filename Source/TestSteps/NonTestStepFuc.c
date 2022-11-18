#include <ansi_c.h>
#include <utility.h>
#include "NonTestStepFuc.h"
#include "ArxtronToolslib.h"

// Global variables
short glbMessageArray[6] = {25928,27756,8303,28503,27762,8548};
glbTr[2] = {1,2};
static double glbTestTimer; //why I can't use extern here
char errmsg[1024];
/***************************************************************************//*!
* \brief 
*******************************************************************************/
int ParseLogs (char *LogPath, LogStruct *ParsedResults)
{
	fnInit;
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
	fclose(fp);
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

void ConvertIntToString(char *arr)
{
	memcpy (arr, &glbMessageArray, sizeof(glbMessageArray));
	printf("%f",glbTestTimer);
	//union {
	//	char msg[16];
	//	short num[8];
	//} test;
	//memcpy (&test.num, glbMessageArray, sizeof(glbMessageArray));
}