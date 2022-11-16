/***************************************************************************//*!
* \file SMC_Actuators.c
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 8/2/2019 5:39:59 PM
* \brief Actuator control library based on "LEC Serial Communication Information"
* 	document from SMC, for LEC_6 Series controllers
* 
* Date			| Name			| Rev.			| Description		
* ------------	|---------------|---------------|--------------------
* 05-15-2013  	| Arxtron		| 1.0.0			| Initial Release
* 09-25-2020	| Chao Zhang	| 1.0.1			| Seperate RunStep into two functions SetStep and Run
* 11-03-2020	| Jai Prajapati | 1.0.2			| Update main with library template
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond

//==============================================================================
// Include files

#include "toolbox.h"
#include <ansi_c.h>
#include <utility.h>
#include "CRC_LIB.h"
#include "SerialComm_LIB.h"
#include "SMC_Actuators.h"

//==============================================================================
// Constants

#define TIMEOUT 5.0

//==============================================================================
// Types

#define whileTO(cond,timeOut,...)\
	startTime = Timer();\
	while (cond && (Timer()-startTime)<timeOut)\
	{\
		ProcessSystemEvents();\
		__VA_ARGS__\
	}\
	libErrChk ((Timer()-startTime)>timeOut,"%s\nFunction timed out",__func__);

//==============================================================================
// Static global variables

static int libInitialized = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

void L2BE (int Input, int Size, uint8_t* Buffer);
void B2LE (uint8_t* Input, int Size, int* Buffer);
void checkStepData (struct StepData *StepData);
int SMCQuery (char* SerialDeviceName,
			  uint8_t Address,
			  uint8_t Function,
			  uint8_t Data[256],
			  uint8_t DataSize,
			  uint8_t Reply[MAXREPLYLEN],
			  char errmsg[ERRLEN]);
int SMCGetErrMsg (uint8_t SMCErrCode,char errmsg[ERRLEN]);
int SMCMotorOn (char* SerialDeviceName,
				uint8_t Address,
				char errmsg[ERRLEN]);
int SMCMotorOff (char* SerialDeviceName,
				 uint8_t Address,
				 char errmsg[ERRLEN]);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond

/***************************************************************************//*!
* \brief Initialize SMC_Actuators library
* 
* \param [in] SerialConfigFile 	XML Configuration file for Serial_LIB
* \param [in] MainPanelHandle	Parent panel handle for serial panel
* 								Pass 0 to create as parent panel
*******************************************************************************/
int Initialize_SMC_Actuators (char *SerialConfigFile, int MainPanelHandle, char errmsg[ERRLEN])
{
	fnInit;
	
	// Make sure CRC_LIB.h is in CRC16MODBUS mode and the .lib is compiled as such
	Initialize_CRC_LIB();
	
	tsErrChk(InitializeSerialPortLib(SerialConfigFile, MainPanelHandle, errmsg),
			 "Unable to initialize Serial Library, check config file path: %s", SerialConfigFile);
	
	libInitialized = 1;
	error = 0;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Turns motor on and finds origin if not already done
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
*******************************************************************************/
int SMCMotorOn (char* SerialDeviceName,
				uint8_t Address,
				char errmsg[ERRLEN])
{
	libInit;
	
	double startTime = 0.0;
	uint8_t DataOut = 0;
	
	// Change to test mode
	libErrChk (SMCForceOutput(SerialDeviceName,Address,SERIALINPUT,1,errmsg),errmsg);
	
	// Turn servo on
	libErrChk (SMCForceOutput(SerialDeviceName,Address,SVON,1,errmsg),errmsg);
	// Wait until Servo Ready
	whileTO(!DataOut,TIMEOUT,
		libErrChk (SMCReadInput(SerialDeviceName,Address,SVRE,1,&DataOut,errmsg),errmsg);
	)
	
	// Return to origin if not done already
	uint8_t busyFlag = 0;
	DataOut = 0;
	whileTO(!DataOut,20.0,
		libErrChk (SMCReadInput(SerialDeviceName,Address,SETON,1,&DataOut,errmsg),errmsg);\
		libErrChk (SMCReadInput(SerialDeviceName,Address,BUSY,1,&busyFlag,errmsg),errmsg);\
		if (!busyFlag && !DataOut)\
		{\
			libErrChk (SMCForceOutput(SerialDeviceName,Address,SETUP,1,errmsg),errmsg);\
		}
	)
	libErrChk (SMCForceOutput(SerialDeviceName,Address,SETUP,0,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Turns motor off
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
*******************************************************************************/
int SMCMotorOff (char* SerialDeviceName,
				 uint8_t Address,
				 char errmsg[ERRLEN])
{
	libInit;
	
	double startTime = 0.0;
	uint8_t DataOut = 1;
	
	// Change to test mode
	libErrChk (SMCForceOutput(SerialDeviceName,Address,SERIALINPUT,0,errmsg),errmsg);
	
	// Turn servo off
	libErrChk (SMCForceOutput(SerialDeviceName,Address,SVON,0,errmsg),errmsg);
	// Wait until Servo Ready off
	whileTO(DataOut,60.0,
		libErrChk (SMCReadInput(SerialDeviceName,Address,SVRE,1,&DataOut,errmsg),errmsg);
	)
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Checks the controller for errors, returns error code if there is an error, 0 otherwise
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
*******************************************************************************/
int SMCCheckError (char* SerialDeviceName,
				   uint8_t Address,
				   char errmsg[ERRLEN])
{
	libInit;
	
	uint8_t DataOut = 0;
	libErrChk (SMCReadInput(SerialDeviceName,Address,ALARM,1,&DataOut,errmsg),errmsg);
	libErrChk (DataOut,"");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Sets the RESET output to try and clear the error. Times out after 5s
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
*******************************************************************************/
int SMCClearError (char* SerialDeviceName,
				   uint8_t Address,
				   char errmsg[ERRLEN])
{
	libInit;
	
	uint8_t DataOut = 1;
	double startTime = 0.0;
	
	whileTO(DataOut,60.0,	
		libErrChk (SMCForceOutput(SerialDeviceName,Address,RESET,1,errmsg),errmsg);\
		libErrChk (SMCReadInput(SerialDeviceName,Address,ALARM,1,&DataOut,errmsg),errmsg);\
	)
	libErrChk (SMCForceOutput(SerialDeviceName,Address,RESET,0,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Motor set step
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] Step 0-63 step number to run
*******************************************************************************/
int SMCSetStep (char* SerialDeviceName,
				uint8_t Address,
				uint8_t Step,
				char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (Step>63,"Step # is from 0 to 63 only, please input a valid step #");
	
	if (SMCCheckError(SerialDeviceName,Address,errmsg))
	{
		libErrChk (SMCClearError(SerialDeviceName,Address,errmsg),errmsg);
	}
	// Set IN0-IN5 to the step number
	libErrChk (SMCWriteBatchOutput(SerialDeviceName,Address,IN0,6,1,&Step,errmsg),errmsg);
	
Error:
	
	return error;
}

/***************************************************************************//*!
* \brief Run the specified step stored in the controller
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] step 0-63 step number to run
*******************************************************************************/
int SMCRun (char* SerialDeviceName,
				uint8_t Address,
				char errmsg[ERRLEN])
{
	libInit;
	
	if (SMCCheckError(SerialDeviceName,Address,errmsg))
	{
		libErrChk (SMCClearError(SerialDeviceName,Address,errmsg),errmsg);
	}
	// Start driving and wait until INP
	libErrChk (SMCForceOutput(SerialDeviceName,Address,DRIVE,1,errmsg),errmsg);
	uint8_t DataOut = 0;
	double startTime = 0.0;
	whileTO(!DataOut,TIMEOUT,
		libErrChk (SMCReadInput(SerialDeviceName,Address,INP,1,&DataOut,errmsg),errmsg);
	)
	libErrChk (SMCForceOutput(SerialDeviceName,Address,DRIVE,0,errmsg),errmsg);
	
Error:
	
	return error;
}

/***************************************************************************//*!
* \brief Run the specified step stored in the controller
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] Step 0-63 step number to run
*******************************************************************************/
int SMCRunStep (char* SerialDeviceName,
				uint8_t Address,
				uint8_t Step,
				char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (SMCMotorOn(SerialDeviceName, Address, errmsg),errmsg);

	libErrChk (SMCSetStep(SerialDeviceName, Address, Step, errmsg),errmsg);

	libErrChk (SMCRun(SerialDeviceName,Address,errmsg),errmsg); 
	
	//libErrChk (SMCMotorOff(SerialDeviceName,Address,errmsg),errmsg);
	
Error:
	
	return error;
}

/***************************************************************************//*!
* \brief Stop running the current step
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
*******************************************************************************/
int SMCStopStep (char* SerialDeviceName,
				 uint8_t Address,
				 char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (SMCForceOutput(SerialDeviceName,Address,DRIVE,0,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Run the step stored in the controller
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] Step 0-63 step number to write
* \param [IN] StepData StepData structure containing all of the information required for a step
*******************************************************************************/
int SMCWriteStep (char* SerialDeviceName,
				  uint8_t Address,
				  uint8_t Step,
				  struct StepData StepData,
				  char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (Step>63,"Step # is from 0 to 63 only, please input a valid step #");
	checkStepData(&StepData);
	
	uint8_t BatchData[32] = {0};
	L2BE (StepData.MoveMode,2,BatchData);
	L2BE (StepData.Spd,2,BatchData+2);
	L2BE (StepData.Pos,4,BatchData+4);
	L2BE (StepData.Acc,2,BatchData+8);
	L2BE (StepData.Dec,2,BatchData+10);
	L2BE (StepData.PushForce,2,BatchData+12);
	L2BE (StepData.TrigLevel,2,BatchData+14);
	L2BE (StepData.PushSpd,2,BatchData+16);
	L2BE (StepData.MoveForce,2,BatchData+18);
	L2BE (StepData.AreaOut1,4,BatchData+20);
	L2BE (StepData.AreaOut2,4,BatchData+24);
	L2BE (StepData.InPos,4,BatchData+28);
	libErrChk (SMCWriteData(SerialDeviceName,Address,(uint16_t) (0x400+16*Step),16,BatchData,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Run a one time command (Specified data)
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] StepData StepData structure containing all of the information required for a step
*******************************************************************************/
int SMCRunWithSpecified (char* SerialDeviceName,
						 uint8_t Address,
						 struct StepData StepData,
						 char errmsg[ERRLEN])
{
	libInit;
	
	checkStepData(&StepData);
	
	uint8_t BatchData[32] = {0};
	L2BE (StepData.MoveMode,2,BatchData);
	L2BE (StepData.Spd,2,BatchData+2);
	L2BE (StepData.Pos,4,BatchData+4);
	L2BE (StepData.Acc,2,BatchData+8);
	L2BE (StepData.Dec,2,BatchData+10);
	L2BE (StepData.PushForce,2,BatchData+12);
	L2BE (StepData.TrigLevel,2,BatchData+14);
	L2BE (StepData.PushSpd,2,BatchData+16);
	L2BE (StepData.MoveForce,2,BatchData+18);
	L2BE (StepData.AreaOut1,4,BatchData+20);
	L2BE (StepData.AreaOut2,4,BatchData+24);
	L2BE (StepData.InPos,4,BatchData+28);
	libErrChk (SMCWriteData(SerialDeviceName,Address,(uint16_t) 0x9102,16,BatchData,errmsg),errmsg);
	
	// Start motor
	libErrChk (SMCMotorOn(SerialDeviceName,Address,errmsg),errmsg);
	if (SMCCheckError(SerialDeviceName,Address,errmsg))
	{
		libErrChk (SMCClearError(SerialDeviceName,Address,errmsg),errmsg);
	}
	
	// Start specified step
	uint8_t StartOp[2] = {1,0};
	libErrChk (SMCWriteData(SerialDeviceName,Address,(uint16_t) 0x9100,1,StartOp,errmsg),errmsg);
	
	// Wait until INP
	uint8_t DataOut = 0;
	double startTime = 0.0;
	whileTO(!DataOut,TIMEOUT,
		libErrChk (SMCReadInput(SerialDeviceName,Address,INP,1,&DataOut,errmsg),errmsg);
	)
	
	// Motor off
	libErrChk (SMCMotorOff(SerialDeviceName,Address,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Get the current state of the controller
* 
* NOTE: May need to consider changing implementation of drive fns since they all
* 	have while loops within. Which means this function can't be called while other
* 	fns are running.
* 
* Either have this fn run in a separate thread, or have monitoring threads to turn
* 	off the motor once INP is reached. Either way, there needs to be threads created.
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	Address 1-255 for Controller ID, 0 for broadcast
* \param [OUT] 	CurPos Current position +- 2147483647 0.01mm
* \param [OUT] 	CurSpd Current speed	0-65535 mm/s
* \param [OUT] 	CurThrust Current thrust 0-300 %
* \param [OUT] 	TargPos Target position +- 2147483647 0.01mm
* \param [OUT] 	StepNo Current step number 0-63
*******************************************************************************/
int SMCGetStateData (char* SerialDeviceName,
					 uint8_t Address,
					 int* CurPos,
					 uint16_t* CurSpd,
					 uint16_t* CurThrust,
					 int* TargPos,
					 uint16_t* StepNo,
					 char errmsg[ERRLEN])
{
	libInit;
	
	uint16_t DataOut[7] = {0};
	libErrChk (SMCReadData(SerialDeviceName,Address,CurrPos,7,DataOut,errmsg),errmsg);
	*CurPos		= (int) (DataOut[0] /*hiByte*/ << 16) & DataOut[1] /*loByte*/;
	*CurSpd		= DataOut[2];
	*CurThrust	= DataOut[3];
	*TargPos	= (int) (DataOut[4] /*hiByte*/ << 16) & DataOut[5] /*loByte*/;
	*StepNo		= DataOut[6];
	
Error:
	return error;
}
//! \cond
/// REGION END

/// REGION START Base Functions
//! \endcond
/***************************************************************************//*!
* \brief Compiles a message and sends it to the controller via serial communication
* 	(RS485) and populates a reply if the command is not a broadcast.
* 
* Compiles a message and sends it to the controller via serial communication
* 	(RS485) and populates a reply if the command is not a broadcast. Message
* 	and reply integrity are checked via CRC16MODBUS. Reply is also checked for
* 	error SMC errors.
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] 	Function SMC functions, see documentation "LEC Serial Communication Information"
* \param [IN] 	Data The data related to the function being called
* \param [IN] 	DataSize The size of the Data portion of the message (No of bytes in Data)
* \param [OUT] 	Reply The reply based on the message sent
*******************************************************************************/
int SMCQuery (char* SerialDeviceName,
			  uint8_t Address,
			  uint8_t Function,
			  uint8_t Data[256],
			  uint8_t DataSize,
			  uint8_t Reply[MAXREPLYLEN],
			  char errmsg[ERRLEN])
{
	int numRetry = 0;
CRCRetry:
	libInit;
	
	uint8_t msg[261] = {0};
	crc crcMsg, crcCheck, * crcReply;
		
	msg[0] = Address;
	msg[1] = Function;
	
	for (int i=0; i<DataSize; ++i)
		msg[2+i] = Data[i];
	
	crcMsg = crcCalc((uint8_t*) msg,2+DataSize,errmsg);
	libErrChk ((strlen(errmsg)>0),errmsg);
	
	// crc should be type uint16_t (CRC16MODBUS) and message starts with the lower byte
	// crcMsg should be in little Endian notation so it should already start with LB
	msg[2+DataSize] = ((uint8_t*) &crcMsg)[0];
	msg[2+DataSize+1] = ((uint8_t*) &crcMsg)[1];
	
	int bytesWritten = WriteSerialDeviceRaw(SerialDeviceName,(char*) msg,DataSize+4, errmsg);
	
	libErrChk (bytesWritten!=DataSize+4 || bytesWritten<0,
			"%s\nError writing to %s",__func__,SerialDeviceName);
	
	DelayWithEventProcessing(SENDDELAY);
	
	// Not a broadcast
	if (Address!=0)
	{
		// Read reply
		int replyLen = 0;
		int replyLen2 = 0;
		double startTime = 0.0;
		
		for (int i=0;i<5;++i)
		{
			whileTO((!replyLen || replyLen<3),60.0,
				replyLen = GetInQLenForDeviceName(SerialDeviceName, errmsg);\
				DelayWithEventProcessing(SENDDELAY);\
			)
			if (replyLen2 != replyLen)
			{
				replyLen2 = replyLen;
				i = 0;
			}
		}
		
		libErrChk (ReadSerialDevice(SerialDeviceName,(char*) Reply,replyLen, errmsg) < 0,
					"%s\nError reading from %s",__func__,SerialDeviceName);
	
		// Check CRC
		// Last 2 chars are CRC from reply
		crcReply = (crc*) (Reply+(replyLen-2));
		crcCheck = crcCalc((uint8_t*) Reply,replyLen-2,errmsg);
		if (*crcReply!=crcCheck)
		{
			if (numRetry<5)
			{
				/* 20200608Biye: For some reason CRC for the motor response is just a huge pain in the a */
				++numRetry;
				DelayWithEventProcessing(1);
				goto CRCRetry;
			}
		}
			
		libErrChk ((strlen(errmsg)>0),errmsg);
		
		libErrChk ((*crcReply!=crcCheck),"CRC from reply does not match calculated CRC");
		
		// Check error
		// MSB of 2nd reply byte is set when there's an error
		if (Reply[1] & 0x80)
			libErrChk (SMCGetErrMsg(Reply[2],errmsg),errmsg);
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Returns the error message from the documentation related to the error code
* 
* \param [IN] SMCErrCode The third byte of the return message if there is an error
*******************************************************************************/
int SMCGetErrMsg (uint8_t SMCErrCode,char errmsg[ERRLEN])
{
	libInit;
	
	error = (int) SMCErrCode;
	switch (error)
	{
		case 1:
			strcpy (errmsg,"An undefined function code was specified.");
			break;
		case 2:
			strcpy (errmsg,"1) An address outside the range was set in the read or write start address.\n"
					"2) In echo back, the test code was not 0000h.");
			break;
		case 3:
			strcpy (errmsg,"1) The number of points set meant that the read or write last number was outside the range.\n"
					"2) There was an instruction meaning that the size of \"Data\" in the communication frame exceeded 256 Bytes.\n"
					"3) In Function 05 (Forced signal output), the data of the specified \"terminal state\" was not FF00h(ON) or 0000h (OFF).\n"
					"4) In Function 0F (Output signals batch writing), the specified \"Write points\" exceeded 256.\n"
					"5) The read or write specified size was 0.");
			break;
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x01 of SMC controller, reads status of #StateChangeFlags
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] 	Flag #StateChangeFlags
* \param [IN] 	NumBitsToRead Number of bits to read starting from the flag address
* \param [OUT] 	DataOut Array of uint8_t storing the returned data bytes (not BITS!!)
*******************************************************************************/
int SMCReadOutput (char* SerialDeviceName,
				   uint8_t Address,
				   enum StateChangeFlags Flag,
				   uint16_t NumBitsToRead,
				   uint8_t DataOut[48],
				   char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (Address==0,"%s cannot use broadcasts",__func__);
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (Flag,2,Data);
	L2BE (NumBitsToRead,2,Data+2);
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x01,Data,4,reply,errmsg),errmsg);
	
	for (int i=0; i<reply[2]; ++i)
		DataOut[i] = reply[3+i];
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x02 of SMC controller, reads status of #StatusFlags
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] 	Flag #StatusFlags
* \param [IN] 	NumBitsToRead Number of bits to read starting from the flag address
* \param [OUT] 	DataOut Array of uint8_t storing the returned data bytes (not BITS!!)
*******************************************************************************/
int SMCReadInput (char* SerialDeviceName,
				  uint8_t Address,
				  enum StatusFlags Flag,
				  uint16_t NumBitsToRead,
				  uint8_t DataOut[16],
				  char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (Address==0,"%s cannot use broadcasts",__func__);
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (Flag,2,Data);
	L2BE (NumBitsToRead,2,Data+2);
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x02,Data,4,reply,errmsg),errmsg);
	
	for (int i=0; i<reply[2]; ++i)
		DataOut[i] = reply[3+i];
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x03 of SMC controller, reads specified words
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] 	DataStartAddress Address of the first word to read
* \param [IN] 	NumWordsToRead Number of words to read starting from the flag address
* \param [OUT] 	DataOut Array of uint16_t storing the returned data words
*******************************************************************************/
int SMCReadData (char* SerialDeviceName,
				 uint8_t Address,
				 uint16_t DataStartAddress,
				 uint16_t NumWordsToRead,
				 uint16_t DataOut[1024],
				 char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (Address==0,"%s cannot use broadcasts",__func__);
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (DataStartAddress,2,Data);
	L2BE (NumWordsToRead,2,Data+2);
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x03,Data,4,reply,errmsg),errmsg);
	
	for (int i=0; i<(reply[2]/2); ++i)
		B2LE(reply+3+2*i,2,(int*) (DataOut+i));
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x05 of SMC controller, forces status of #StateChangeFlags
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] Flag #StateChangeFlags
* \param [IN] State 0 = OFF, 1 = ON
*******************************************************************************/
int SMCForceOutput (char* SerialDeviceName,
				    uint8_t Address,
				    enum StateChangeFlags Flag,
				    int State,
				    char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (State!=0&&State!=1,"%s\nInvalid state input",__func__);
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (Flag,2,Data);
	Data[2] = (State ? 0xFF : 0x00);
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x05,Data,4,reply,errmsg),errmsg);
	
	libErrChk(reply[4]!=Data[2],"%s\nForce Output failed to set output",__func__);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x08 of SMC controller, echos the input data
* 
* \param [IN] 	SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] 	DataIn Arbitrary data
* \param [IN] 	DataLen Length of the arbitrary data
* \param [OUT] 	DataOut Echo of DataIn
*******************************************************************************/
int SMCEcho (char* SerialDeviceName,
			 uint8_t* DataIn,
			 uint8_t DataLen,
			 uint8_t* DataOut,
			 char errmsg[ERRLEN])
{
	libInit;
	
	uint8_t reply[MAXREPLYLEN] = {0};
	
	libErrChk(SMCQuery(SerialDeviceName,0x00,0x08,DataIn,DataLen,reply,errmsg),errmsg);
	
	for (int i=0; i<DataLen; ++i)
		DataOut[i] = reply[4+i];
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x0F of SMC controller, batch writes #StateChangeFlags
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] Flag #StateChangeFlags
* \param [IN] NumBitsToWrite Number of bits to write starting from the flag address
* \param [IN] NumOfData Number of bytes of data to write
* \param [IN] BatchData The bytes of data to write, the flag (starting) bit corresponds
* 			  to the LSB of the first byte (Eg. BatchData[0] = 0b00000001 => setting the flag
* 			  bit to 1
*******************************************************************************/
int SMCWriteBatchOutput (char* SerialDeviceName,
						 uint8_t Address,
						 enum StateChangeFlags Flag,
						 uint16_t NumBitsToWrite,
						 uint8_t NumOfData,
						 uint8_t* BatchData,
						 char errmsg[ERRLEN])
{
	libInit;
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (Flag,2,Data);
	L2BE (NumBitsToWrite,2,Data+2);
	Data[4] = NumOfData;
	for (int i=0; i<NumOfData; ++i)
		Data[5+i] = BatchData[i];
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x0F,Data,5+NumOfData,reply,errmsg),errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Function 0x10 of SMC controller, writes specified words
* 
* \param [IN] SerialDeviceName Name of the controller found in configuration\\Serial.xml
* \param [IN] Address 1-255 for Controller ID, 0 for broadcast
* \param [IN] DataStartAddress Address of the first word to read
* \param [IN] NumWordsToWrite Number of word to write starting from the flag address
* \param [IN] BatchData The bytes of data to write. Keep in mind that all numerical
* 			  data written should be in Big Endian form.
*******************************************************************************/
int SMCWriteData (char* SerialDeviceName,
				  uint8_t Address,
				  uint16_t DataStartAddress,
				  uint16_t NumWordsToWrite,
				  uint8_t* BatchData,
				  char errmsg[ERRLEN])
{
	libInit;
	
	uint8_t reply[MAXREPLYLEN] = {0};
	uint8_t Data[256] = {0};
	L2BE (DataStartAddress,2,Data);
	L2BE (NumWordsToWrite,2,Data+2);
	Data[4] = (uint8_t) NumWordsToWrite*2;
	for (int i=0; i<NumWordsToWrite*2; ++i)
		Data[5+i] = BatchData[i];
	
	libErrChk(SMCQuery(SerialDeviceName,Address,0x10,Data,(uint8_t) (5+NumWordsToWrite*2),reply,errmsg),errmsg);
	
Error:
	return error;
}
//! \cond
/// REGION END

/// REGION START Utility Fns
//! \endcond
/***************************************************************************//*!
* \brief Converts an int input to big endian notation and store it into buffer
* 
* \param [IN] 	Input Input integer to be converted
* \param [IN] 	Size Size of the buffer (1-4)
* \param [OUT] 	buffer Buffer to store the converted value
*******************************************************************************/
void L2BE (int Input, int Size, uint8_t* buffer)
{
	for (int i=0; i<Size; ++i)
		buffer[i] = ((uint8_t*) &Input)[Size-i-1];
}

/***************************************************************************//*!
* \brief Converts a uint8_t input to little endian notation (int) and store it into buffer
* 
* \param [IN] 	Input Input byte array to be converted
* \param [IN] 	Size Size of the input (1-4)
* \param [OUT] 	Buffer Buffer to store the converted value
*******************************************************************************/
void B2LE (uint8_t* Input, int Size, int* Buffer)
{
	for (int i=0; i<Size; ++i)
		((uint8_t*) Buffer)[i] = Input[Size-i-1];
}

#define checkLim(var,lowlim,hilim)\
	var = (var<lowlim ? lowlim : var);\
	var = (var>hilim ? hilim : var)
void checkStepData (struct StepData *StepData)
{
	checkLim (StepData->MoveMode,1,2);
	checkLim (StepData->Spd,1,65535);
	checkLim (StepData->Pos,-((int) pow(2,32)),((int) pow(2,32))-1);
	checkLim (StepData->Acc,1,65535);
	checkLim (StepData->Dec,1,65535);
	checkLim (StepData->PushForce,0,100);
	checkLim (StepData->TrigLevel,0,100);
	checkLim (StepData->PushSpd,1,65535);
	checkLim (StepData->MoveForce,0,300);
	checkLim (StepData->AreaOut1,-((int) pow(2,32)),((int) pow(2,32))-1);
	checkLim (StepData->AreaOut2,-((int) pow(2,32)),((int) pow(2,32))-1);
	checkLim (StepData->InPos,-((int) pow(2,32)),((int) pow(2,32))-1);
}
//! \cond
/// REGION END
//! \endcond