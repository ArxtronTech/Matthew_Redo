/***************************************************************************//*!
* \file SMC_Actuators.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 8/2/2019 5:39:59 PM
*******************************************************************************/

#ifndef __SMC_Actuators_H__
#define __SMC_Actuators_H__

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Include files

#include <stdint.h>
#include "cvidef.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

#define LEC6	// Controller series
#ifdef LEC6
		
//==============================================================================
// Global vaiables

//==============================================================================
// Types
		
/***************************************************************************//*!
* \brief State Change Flags (Y10-Y3F bits), can be read or written
*******************************************************************************/
enum StateChangeFlags
{
	IN0				= 0x10,
	IN1				= 0x11,
	IN2				= 0x12,
	IN3				= 0x13,
	IN4				= 0x14,
	IN5				= 0x15,
	HOLD			= 0x18,
	SVON			= 0x19,		//! Motor On/Off
	DRIVE			= 0x1A,		//! Start/stop motion
	RESET			= 0x1B,		//! Error/alarm reset
	SETUP			= 0x1C,		//! Return to origin
	JOGN			= 0x1D,
	JOGP			= 0x1E,
	SERIALINPUT		= 0x30		//! 0 = Parallel Input Driving Mode, 1 = Serial Input Driving Mode
};

/***************************************************************************//*!
* \brief Status Flags (X40-X4F bits), can only be read
*******************************************************************************/
enum StatusFlags
{
	OUT0			= 0x40,
	OUT1			= 0x41,
	OUT2			= 0x42,
	OUT3			= 0x43,
	OUT4			= 0x44,
	OUT5			= 0x45,
	BUSY			= 0x48,		//! Servo is moving
	SVRE			= 0x49,		//! Servo Ready, on when SVON=1
	SETON			= 0x4A,		//! On when return to origin is done
	INP				= 0x4B,		//! In position, on when operation is complete
	AREA			= 0x4C,		//! On when between Area1 and Area2
	WAREA			= 0x4D,
	ESTOP			= 0x4E,
	ALARM			= 0x4F
};

/***************************************************************************//*!
* \brief State Data (D9000-D9006 and D000E words)
*******************************************************************************/
enum StateData
{
	CurrPos		= 0x9000,	//! 4 bytes, +-2147483647 0.01mm
	CurrSpd		= 0x9002,	//! 2 bytes, 0-65535 mm/s
	CurrThrust	= 0x9003,	//! 2 bytes, 0-300 %
	TargPos		= 0x9004,	//! 4 bytes, +-2147483647 0.01mm
	DriveDataNo	= 0x9006,	//! 2 bytes, 0-63 step no.
	EquipName	= 0x000E	//! 16 bytes, 14 letter ASCII
};

/***************************************************************************//*!
* \brief Specified Data used for running a move command manually
*******************************************************************************/
enum SpecifiedData
{
	StartOp	= 0x9100	//! 1 byte, starts operation according to specified data	 
};

/***************************************************************************//*!
* \brief Sequence of steps stored in the controller
*******************************************************************************/
struct StepData
{
	uint16_t	MoveMode;	//! 2 bytes, 1 = absolute, 2 = relative
	uint16_t	Spd;		//! 2 bytes, 0-65535 mm/s
	int			Pos;		//! 4 bytes, +-2147483647 0.01mm
	uint16_t	Acc;		//! 2 bytes, 0-65535 mm/s2
	uint16_t	Dec;		//! 2 bytes, 0-65535 mm/s2
	uint16_t	PushForce;	//! 2 bytes, 0-100 %
	uint16_t	TrigLevel;	//! 2 bytes, 0-100 %
	uint16_t	PushSpd;	//! 2 bytes, 0-65535 mm/s
	uint16_t	MoveForce;	//! 2 bytes, 0-300 %
	int			AreaOut1;	//! 4 bytes, +-2147483647 0.01mm
	int			AreaOut2;	//! 4 bytes, +-2147483647 0.01mm
	int			InPos;		//! 4 bytes, +-2147483647 0.01mmA											
};

#define SENDDELAY 0.02	// Roughly 20ms delay between messages based on default settings
#define MAXREPLYLEN 2060	// 2048+9 for reading from D0410 to D07FF and a little buffer
#endif

//==============================================================================
// External variables

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

int CVICALLBACK FunctionSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK RunFunction(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

int Initialize_SMC_Actuators (char *SerialConfigFile, int MainPanelHandle, char errmsg[ERRLEN]);

int SMCMotorOn (char* SerialDeviceName,
				uint8_t Address,
				char errmsg[ERRLEN]);
int SMCMotorOff (char* SerialDeviceName,
				 uint8_t Address,
				 char errmsg[ERRLEN]);
int SMCCheckError (char* SerialDeviceName,
				   uint8_t Address,
				   char errmsg[ERRLEN]);
int SMCClearError (char* SerialDeviceName,
				   uint8_t Address,
				   char errmsg[ERRLEN]);
int SMCSetStep (char* SerialDeviceName,
				uint8_t Address,
				uint8_t Step,
				char errmsg[ERRLEN]);
int SMCRun (char* SerialDeviceName,
				uint8_t Address,
				char errmsg[ERRLEN]);
int SMCRunStep (char* SerialDeviceName,
				uint8_t Address,
				uint8_t Step,
				char errmsg[ERRLEN]);
int SMCStopStep (char* SerialDeviceName,
				 uint8_t Address,
				 char errmsg[ERRLEN]);
int SMCWriteStep (char* SerialDeviceName,
				  uint8_t Address,
				  uint8_t Step,
				  struct StepData StepData,
				  char errmsg[ERRLEN]);
int SMCRunWithSpecified (char* SerialDeviceName,
						 uint8_t Address,
						 struct StepData StepData,
						 char errmsg[ERRLEN]);


int SMCReadOutput (char* SerialDeviceName,
				   uint8_t Address,
				   enum StateChangeFlags Flag,
				   uint16_t NumBitsToRead,
				   uint8_t DataOut[48],
				   char errmsg[ERRLEN]);
int SMCReadInput (char* SerialDeviceName,
				  uint8_t Address,
				  enum StatusFlags Flag,
				  uint16_t NumBitsToRead,
				  uint8_t DataOut[16],
				  char errmsg[ERRLEN]);
int SMCReadData (char* SerialDeviceName,
				 uint8_t Address,
				 uint16_t DataStartAddress,
				 uint16_t NumWordsToRead,
				 uint16_t DataOut[1024],
				 char errmsg[ERRLEN]);
int SMCForceOutput (char* SerialDeviceName,
				    uint8_t Address,
				    enum StateChangeFlags Flag,
				    int State,
				    char errmsg[ERRLEN]);
int SMCEcho (char* SerialDeviceName,
			 uint8_t* DataIn,
			 uint8_t DataLen,
			 uint8_t* DataOut,
			 char errmsg[ERRLEN]);
int SMCWriteBatchOutput (char* SerialDeviceName,
						 uint8_t Address,
						 enum StateChangeFlags Flag,
						 uint16_t NumBitsToWrite,
						 uint8_t NumOfData,
						 uint8_t* BatchData,
						 char errmsg[ERRLEN]);
int SMCWriteData (char* SerialDeviceName,
				  uint8_t Address,
				  uint16_t DataStartAddress,
				  uint16_t NumWordsToWrite,
				  uint8_t* BatchData,
				  char errmsg[ERRLEN]);
int SMCGetStateData (char* SerialDeviceName,
					 uint8_t Address,
					 int* CurPos,
					 uint16_t* CurSpd,
					 uint16_t* CurThrust,
					 int* TargPos,
					 uint16_t* StepNo,
					 char errmsg[ERRLEN]);

#ifdef __cplusplus
	}
#endif

#endif  /* ndef __SMC_Actuators_H__ */
