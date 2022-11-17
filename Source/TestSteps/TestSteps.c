/***************************************************************************//*!
* \file TestSteps.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/10/2022 11:34:01 PM
* \brief This contains all the test steps used for this project.
* 
* This project compiles into a DLL file and is meant to be used as a CVI module
* 	within TestStand.
* 
* This Library was created using
* * Template version 1.0.0
* * ArxtronToolslib version 1.1.0
* * DebugToolslib version 0.0.0
* * GUIToolslib version 0.0.0
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include "TestSteps.h"
#include "Lib1.h"
#include "StructureDefine.h"
#include "NonTestStepFuc.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static LogStruct glbLogContent;
static short glbMessageArray[6] = {25928,27756,8303,28503,27762,8548};

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

void settesttimer (void);
void stoptesttimer (double *timeelapsed);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond


/***************************************************************************//*!
* \brief 
*******************************************************************************/

int TestStep_Step1 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo)
{
	TSInit;
	tsErrChk(NestNum<0,"Num number is wrong");
		
	
	settesttimer();
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int TestStep_Step2 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo)
{
	TSInit;
	tsErrChk(NestNum<0,"Num number is wrong");
	
	
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int TestStep_Step3 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo)
{
	TSInit;
	tsErrChk(NestNum<0,"Num number is wrong");
	ParseLogs ("TestLog.log", &glbLogContent);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 
*******************************************************************************/
int TestStep_Step4 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo)
{
	TSInit;
	tsErrChk(NestNum<0,"Num number is wrong");
	double TIME;
	stoptesttimer(&TIME);
	
Error:
	return error;
}

void TestStep_Step5 (void)
{
	glbMessageArray;
}

//! \cond
/// REGION END
//! \endcond