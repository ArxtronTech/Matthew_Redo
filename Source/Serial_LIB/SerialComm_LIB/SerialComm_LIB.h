/***************************************************************************//*!
* \file SerialComm_LIB.h
* \author Arxtron
* \copyright Arxtron Technologies Inc. All Rights Reserved.
* \date 11/9/2020 8:54:00 AM
*******************************************************************************/

#ifndef __SerialComm_LIB_H__
#define __SerialComm_LIB_H__

#ifdef __cplusplus
	extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

#define MAXNUMOFSERIALPORTS 50
#define MAXCHARARRAYLENGTH 400
#define SERIALLIBREV "1.0.1"
		
//==============================================================================
// Types

typedef struct
{
	char   		DeviceName[MAXCHARARRAYLENGTH];
	char		Comport[MAXCHARARRAYLENGTH];
	char 		BaudRate[MAXCHARARRAYLENGTH];
	char		Parity[MAXCHARARRAYLENGTH];
	char 		DataBits[MAXCHARARRAYLENGTH];
	char 		StopBits[MAXCHARARRAYLENGTH];
	char		CTSMode[MAXCHARARRAYLENGTH];
	char		XonXoff[MAXCHARARRAYLENGTH];
	char		Timeout[MAXCHARARRAYLENGTH];
	int			PortOpen;
} SerialFileInfoStruct;
		
//==============================================================================
// Global vaiables

//==============================================================================
// External variables

//==============================================================================
// Global functions
		
void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

int CVICALLBACK FunctionSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK RunFunction(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
		
int InitializeSerialPortLib(char *SerialConfigurationFile, int MainPanelHandle, char errmsg[ERRLEN]);
int ReadSerialConfigurationFile(char *filePath);
int GetSerialConfigurationPanelHandle(void);
int GetSerialDebugPanelHandle(void) ;

int InitSerialDevice (char *SerialDeviceName, char errmsg[ERRLEN]);
int CloseSerialDevice (char *SerialDeviceName, char errmsg[ERRLEN]);

int WriteSerialDevice(char *SerialDeviceName, char *data, char errmsg[ERRLEN]);
int WriteSerialDeviceRaw(char *SerialDeviceName, char *data, int dataLen, char errmsg[ERRLEN]);
int ReadSerialDevice(char *SerialDeviceName, char *ReadData, int numByteToRead, char errmsg[ERRLEN]);
int ReadSerialDeviceUntilTermChar(char *SerialDeviceName, char *ReadData, int numByteToRead, int terminationByte, char errmsg[ERRLEN]);

int FlushInQDevice(char *SerialDeviceName, char errmsg[ERRLEN]);
int FlushOutQDevice(char *SerialDeviceName, char errmsg[ERRLEN]);

char *getSerialLibRevision (void);

int GetTotalSerialDevices(void);
int GetDeviceName(int index, char *devName, char errmsg[ERRLEN]);
int GetInQLenForDeviceName(char *SerialDeviceName, char errmsg[ERRLEN]);

#ifdef __cplusplus
	}
#endif

#endif  /* ndef __SerialComm_LIB_H__ */
