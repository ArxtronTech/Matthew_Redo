/***************************************************************************//*!
* \file ArxtronToolslib.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 7/27/2020 9:24:38 PM
* \brief A collection of standard macros for all CVI projects
*******************************************************************************/

#ifndef __ArxtronToolslib_H__
#define __ArxtronToolslib_H__

#ifdef __cplusplus
    extern "C" {
#endif

#define ARXTRON_TOOLSLIB_VERSION	"1.1.0"

//==============================================================================
// Include files

#include "cvidef.h"
		
//! \cond
/// REGION START Constants
//! \endcond
/***************************************************************************//*!
* Max buffer size of errmsg variable
*******************************************************************************/
#define	ERRLEN		1024

//! \cond
/// REGION END

/// REGION START Error Codes
//! \endcond
// Standard error codes (-10000 ~ -19999) 
#define	ERR_LIB_NOT_INITIALIZED		-10000

// Lib specific error codes (-20000 ~ -99998)

//! \cond
/// REGION END

/// REGION START Macros
//! \endcond
/***************************************************************************//*!
* \brief Standard function initialization
*******************************************************************************/
#define fnInit\
	int error = -99999;\
	memset (errmsg, 0, ERRLEN)

/***************************************************************************//*!
* Standard first lines of a LabWindows teststep function
*******************************************************************************/
#define tsInit\
	fnInit;\
	setInstructionMsgBox (NestNum,"",-1,-1,30);\
	InsertReport(TestStepFileInfo[NestNum][TestNum].testName)

/***************************************************************************//*!
* Error checking code for LabWindows teststep functions
*******************************************************************************/
#define tsErrChk(fnCall,...)\
	if (error = fnCall, error != 0)\
	{\
		char err1[ERRLEN] = {0};\
		char err2[ERRLEN] = {0};\
		sprintf (err1, "(%d) %s\n", error, __func__);\
		sprintf (err2, __VA_ARGS__);\
		sprintf (errmsg, "%s%s", err1, err2);\
		goto Error;\
	}

/***************************************************************************//*!
* Standard last lines of a LabWindows teststep function before function return
*******************************************************************************/
#define tsExit\
	if (strlen(TestStepFileInfo[NestNum][TestNum].testReportText)>0)\
	{\
		char disp[1024] = {0};\
		char tmp[1024] = {0};\
		strcpy (tmp,TestStepFileInfo[NestNum][TestNum].testReportText);\
		char *tok = strtok(tmp,"\n");\
		while (tok)\
		{\
			sprintf (disp,DISPLAYSPACING"%s", tok);\
			InsertTestReportListItem(NestNum, disp);\
			tok = strtok(NULL,"\n");\
		}\
	}\
	setStatusMsgBox (NestNum,"",-1,-1,-1);\
	error |= exitCond(errmsg);\
	if (error && strlen(errmsg)>0)\
	{\
		setInstructionMsgBox (NestNum,errmsg,-1,-1,12);\
	}\
	if (strlen(TestStepFileInfo[NestNum][TestNum].testResultVal)==0)\
	{\
		if (strlen(errmsg)>0)\
		{\
			strcpy (TestStepFileInfo[NestNum][TestNum].testResultVal, errmsg);\
		}\
		else\
		{\
			strcpy (TestStepFileInfo[NestNum][TestNum].testResultVal,error?"FAIL":"PASS");\
		}\
	}

/***************************************************************************//*!
* Standard first lines of a TestStand teststep function
*******************************************************************************/
#define TSInit\
	char* errmsg = ErrInfo->msg;\
	fnInit

/***************************************************************************//*!
* Error checking code for TestStand teststep functions
*******************************************************************************/
#define TSErrChk(fnCall,...)\
	error = fnCall;\
	ErrInfo->code = error;\
	ErrInfo->occurred = (error < 0);\
	if (error)\
	{\
		sprintf (errmsg, __VA_ARGS__);\
		sprintf (ReportText, "(%d) %s", error, errmsg);\
		goto Error;\
	}

/***************************************************************************//*!
* Standard first lines of a SourceLib function
*******************************************************************************/
#define libInit\
	fnInit;\
	if (!libInitialized)\
		libErrChk (ERR_LIB_NOT_INITIALIZED,"")

/***************************************************************************//*!
* Error checking code for SourceLib functions
*******************************************************************************/
#define libErrChk(fnCall,...)\
	if (error = fnCall, error != 0)\
	{\
		char err1[ERRLEN] = {0};\
		char err2[ERRLEN] = {0};\
		sprintf (err1, "(%d) %s\n", error, __func__);\
		sprintf (err2, __VA_ARGS__);\
		if (strlen(err2)==0)\
		{\
			GetStandardErrMsg(error, err2);\
		}\
		sprintf (errmsg, "%s%s", err1, err2);\
		goto Error;\
	}

#define mkdir(dir) SetBreakOnLibraryErrors(0); MakeDir (dir); SetBreakOnLibraryErrors(1);
//! \cond
/// REGION END

/// REGION START Function Parameter Modifiers
//! \endcond
/***************************************************************************//*!
* \brief Indicates parameter is input only
*******************************************************************************/
#define IN		const
/***************************************************************************//*!
* \brief Indicates parameter is optional (use 0 to ignore)
*******************************************************************************/
#define OPT		
//! \cond
/// REGION END

/// REGION START Version Checking
//! \endcond
#define DEPENDENCY_VERSION_CHECK(baselib,ExpectedVersionMajor,ExpectedVersionMinor,MismatchType)\
	baselib##_CheckVersionCompatibility(ExpectedVersionMajor,ExpectedVersionMinor,MismatchType)
//! \cond
/// REGION END
//! \endcond

//==============================================================================
// Types

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ArxtronToolslib_H__ */
