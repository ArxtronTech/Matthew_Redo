/***************************************************************************//*!
* \file AMETEK_LIB.h
* \author Dwayne Alex
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 8/1/2019 3:38:29 PM
*******************************************************************************/

#ifndef __AMETEK_LIB_H__
#define __AMETEK_LIB_H__

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Include files
		
#include <ansi_c.h>
#include "SerialComm_LIB.h"
#include "ArxtronToolslib.h"
		
//==============================================================================
// Constants

//==============================================================================
// Global vaiables

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int Initialize_AMETEK_LIB(char errmsg[ERRLEN]);

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

int CVICALLBACK FunctionSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK RunFunction(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

// -------------- START GETTER FUNCTIONS --------------
int GetStatus_ESR(char errmsg[ERRLEN]);
int GetStatus_SCPI(char errmsg[ERRLEN]);
int GetStatus_PROT(char errmsg[ERRLEN]);
int GetStatus_ERRs(char errmsg[ERRLEN]);
int GetStatus_OUT(char errmsg[ERRLEN]);
double GetStatus_TRIP(char errmsg[ERRLEN]);
double GetVoltage(char errmsg[ERRLEN]);
double GetCurr(char errmsg[ERRLEN]);

// -------------- END GETTER FUNCTIONS --------------


// -------------- START SETTER FUNCTIONS --------------

int SetVolt(double Volts, char errmsg[ERRLEN]);
int SetLimit_Curr(double Current, char errmsg[ERRLEN]);
int SetFold(int Type, char errmsg[ERRLEN]);
int SetPolarity(int Pol, char errmsg[ERRLEN]);
int SetSense(int Sense, char errmsg[ERRLEN]);
int SetState(int State, char errmsg[ERRLEN]);
int SetIsolation(int Iso, char errmsg[ERRLEN]);
int SetDelay(double Time, char errmsg[ERRLEN]);

// -------------- END SETTER FUNCTIONS --------------

int InitPSU(double Volts, double Curr, char errmsg[ERRLEN]);
int SelfTest(char errmsg[ERRLEN]);
int ClearPSUStatus(char errmsg[ERRLEN]); 
int ResetPSU(char errmsg[ERRLEN]);
int ReportErrors(int NestNum, int TestNum, char errmsg[ERRLEN]);
void SetPSUName(char *Name);

#ifdef __cplusplus
	}
#endif

#endif  /* ndef __AMETEK_LIB_H__ */