/***************************************************************************//*!
* \file AMETEK_LIB.c
* \author Dwayne Alex
* \copyright Arxtron Technologies Inc. All Rights Reserved.
* \date 8/1/2019 3:38:29 PM
* \brief Serial communication wrapper for Ametek used in RCM8 Run-In-Tester
* 
* A longer description
*
* Version     |   Date        |   Author          |   Description
* ------------|---------------|-------------------|-----------------------------
* 1.0.0       | Aug 1, 2019   | Dwayne Alex       | Initial Release
* 1.0.1		  | Nov 9, 2020   | Jai Prajapati     | Updated with library format
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond

//==============================================================================
// Include files
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include "AMETEK_LIB.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int libInitialized = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

char psuName [25] = {0};
char projectDir[MAX_PATHNAME_LEN] = {0};

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond

/***************************************************************************//*!
* \brief Initialize Ametek library. Requires SerialComm_LIB to be previously
* 		 initialized and configured.
*
*******************************************************************************/
int Initialize_AMETEK_LIB(char errmsg[ERRLEN])
{
	fnInit;
	
	GetProjectDir(projectDir);
	
	libInitialized = 1;
	error = 0;
	
Error:
	return error;
}

// -------------- START GETTER FUNCTIONS --------------

/***************************************************************************//*!
* \brief Get Event status register
*
* /return errror code listed below
* Possible vals: 1 - Operation Complete
* 				 2 - Request control - not used
*                4 - Query Error
*                8 - Device Dependent Error
*                10 - Execution error
*                20 - Command error
*                40- User Request - not used
*                80 - Power On
*******************************************************************************/
int GetStatus_ESR(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};

	libErrChk(WriteSerialDevice(psuName, "*ESR?", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0)
	{
		return error;
	}
	return atoi(readBuff);
}

/***************************************************************************//*!
* \brief Get SCPI status
*
* /return errror code listed below
* Possible vals: 1 - Not used
*                2 - PROT Event
*                4 - error/event queue message available
*                8 - Questionable Status
*                10 - Message available
*                20 - Summary bit for ESR
*                40 - Request service bit
*                80 - Operational Status
*******************************************************************************/
int GetStatus_SCPI(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "*STB?", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) 
	{
		return error;
	}
	return atoi(readBuff);
}

/***************************************************************************//*!
* \brief Get protection fault status
*
* /return errror code listed below
* Possible vals: 1 - Constant Voltage
*                2 - Constant current
*                4 - Not used
*                8 - Over voltage protection tripped
*                10 - Overtemperature protection tripped
*                20 - Supply external shutdwn active
*                40- Foldback mode operation
*                80 - Remote programming error
*******************************************************************************/
int GetStatus_PROT(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "STAT:PROT:COND?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) {
		return error;
	}
	return atoi(readBuff);
}

/***************************************************************************//*!
* \brief Gets the errror status
*
* This keeps a queue of the last 10 errors to occur. The error list is extensive, 
* refer to the M9 Programming Manual, section 3.2.5 Error/Event Queue.
* The ClearPSU() function is used to clear all errors.
*******************************************************************************/
int GetStatus_ERRs(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName,"SYST:ERR?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 12, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	return error;	
}

/***************************************************************************//*!
* \brief Get protection falut status
*
* \return 1 - Output ON. 0 - Output off.
*******************************************************************************/
int GetStatus_OUT(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "SOUR:ONL?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) {
		return error;
	}
	return atoi(readBuff);
}

/***************************************************************************//*!
* \brief Get tripped status
*
* \return 0 - Not tripped. 1 - Tripped. 
*******************************************************************************/
double GetStatus_TRIP(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "OUTP:TRIP?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) {
		return error;
	}
	return atoi(readBuff);
}

/***************************************************************************//*!
* \brief Get voltage output level
*
* \return Double value of voltage.
*******************************************************************************/
double GetVoltage(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "MEAS:VOLT:AVE?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(error & ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) {
		return error;
	}
	return atof(readBuff);
}

/***************************************************************************//*!
* \brief Get current output level
*
* /return Double value of current.
*******************************************************************************/
double GetCurr(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};
	
	libErrChk(WriteSerialDevice(psuName, "MEAS:CURR:AVE?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
Error:
	if (error < 0) {
		return error;
	}
	return atof(readBuff);
}


// -------------- END GETTER FUNCTIONS --------------


// -------------- START SETTER FUNCTIONS --------------

/***************************************************************************//*!
* \brief Sets the voltage on the PSU from the paramter given
*
*******************************************************************************/
int SetVolt(double Volts, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "SOUR:VOLT %2.2f\r", Volts);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the current limit on the PSU from the paramter given
*
*******************************************************************************/
int SetLimit_Curr(double Current, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "SOUR:CURR %3.2f\r", Current);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the protection type on the PSU from the paramter given.
*
* The control type is fold, there are three modes:
* 		type = 0: Do nothing
*       type = 1: Program to down to zero volts upon entering constant voltage mode
*       type = 2: Program down to zero upon entering constant current mode.
*******************************************************************************/
int SetFold(int Type, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:PROT:FOLD %d\r", Type);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the polarity of the PSU from the paramter given.
*
*******************************************************************************/
int SetPolarity(int Pol, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:POL %d\r", Pol);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the sense relay signal open/closed on the PSU from the paramter given.
*
* \param [in] Sense 	1 - ON, 2 - OFF
*******************************************************************************/
int SetSense(int Sense, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:SENS %d\r", Sense);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the output on the PSU from the paramter given.
*
* \param [in] State 	1 - ON, 2 - OFF
*******************************************************************************/
int SetState(int State, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:STAT %d\r", State);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the isolation relay control signal on the PSU from the parameter given
*
* \param [in] Iso 	1 - ON, 2 - OFF
*******************************************************************************/
int SetIsolation(int Iso, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:STAT %d\r", Iso);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Sets the delay for fault reporting on the PSU from the paramter given.
*
* \param [in] Time 		time in seconds
*******************************************************************************/
int SetDelay(double Time, char errmsg[ERRLEN])
{
	libInit;
	
	char command [20] = {0};
	sprintf (command, "OUTP:PROT:DEL %f\r", Time);
	
	libErrChk(WriteSerialDevice(psuName, command, errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

// -------------- END SETTER FUNCTIONS --------------

/***************************************************************************//*!
* \brief Intializes the PSU based off the inputs.
*
*******************************************************************************/
int InitPSU(double Volts, double Curr, char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk(ResetPSU(psuName), errmsg);
	   
	libErrChk(ReportErrors(0, 0, errmsg), errmsg);
	
	DelayWithEventProcessing(2.0); 
	
	libErrChk(ClearPSUStatus(errmsg), errmsg);
	
	libErrChk(ReportErrors(0, 0, errmsg), errmsg);
	
	libErrChk(SetVolt(Volts, errmsg), errmsg);
	
	libErrChk(ReportErrors(0, 0, errmsg), errmsg);
	
	libErrChk(SetLimit_Curr(Curr, errmsg), errmsg);
	
	libErrChk(ReportErrors(0, 0, errmsg), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Let the PSU Self-Check
*
* \return 0 - No errors, 1 - Error(s) occured
*******************************************************************************/
int SelfTest(char errmsg[ERRLEN])
{
	libInit;
	
	char readBuff[500] = {0};

	libErrChk(WriteSerialDevice(psuName, "*TST?\r", errmsg) ? -2 : 0, "Serial interface write error");
	
	libErrChk(ReadSerialDeviceUntilTermChar(psuName, readBuff, 10, 13, errmsg) ? -2 : 0, "Serial interface read error");
	
	if (strcmp(errmsg, "0")) //error reset psu
	{
		MessagePopup("ERROR", "PSU HAS ENCOUNTERED A SELF TEST ERROR. PLEASE RESTART PSU");
		return -1;
	}
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Clears PSU registers
*
*******************************************************************************/
int ClearPSUStatus(char errmsg[ERRLEN]) 
{
	libInit;
	
	libErrChk(WriteSerialDevice(psuName, "*CLS\r", errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief Clears PSU registers and resets to default settings
*
*******************************************************************************/
int ResetPSU(char errmsg[ERRLEN]) 
{
	libInit;
	
	libErrChk(WriteSerialDevice(psuName, "*RST\r", errmsg) ? -2 : 0, "Serial interface write error");
	
Error:
	if (error < 0) {
		return error;
	}
	return 0;
}

/***************************************************************************//*!
* \brief 
*
*******************************************************************************/
int ReportErrors (int nestNum, int testNum, char errmsg[ERRLEN])
{
	libInit;

	char errCode [255] = {0};
		
	libErrChk(GetStatus_ERRs(errCode), errmsg);
	
	if (error)
	{
//		Ctl24V(0);
		DelayWithEventProcessing(0.2);
		
		snprintf(errmsg, sizeof errmsg, "Error: Could not communicate to PSU");

		if (!strcmp(errCode ,""))
			 strcpy(errCode,"NULL");
		
		snprintf(errmsg, sizeof errmsg, "Couldn't communicate to PSU. Error Code: %s", errCode);
		
		return -1;
	}
	
Error:
	return error;;
}


void SetPSUName(char *Name)
{
	strcpy(psuName, Name);
}

//! \cond
/// REGION END
