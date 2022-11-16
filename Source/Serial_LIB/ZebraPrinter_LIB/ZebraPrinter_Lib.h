/***************************************************************************//*!
* \file ZebraPrinter_Lib.h
* \author Chao Zhang
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 5/29/2020 4:09:26 PM
* 
*******************************************************************************/

#ifndef __ZebraPrinter_Lib_H__
#define __ZebraPrinter_Lib_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants
		
// Lib specific error codes (-20000 ~ -99998)
#define ERR_INVALID_CONTENT_TYPE	-21001
		
//==============================================================================
// Global vaiables

//==============================================================================
// Types

typedef enum {
	ZP_Header = 1,
	ZP_Footer = 2,
	ZP_LabelTop = 3,
	ZP_PrintWidth = 4,
	ZP_LabelShift = 5,
	ZP_TearOff = 6,
	ZP_Darkness = 7,
	ZP_String = 10,
	ZP_Barcode = 11,
	ZP_Logo = 30
} ZebraPrinter_ContentType;

typedef struct {
	/* Common Params */
	unsigned int x;
	unsigned int y;
	unsigned int height;
	unsigned int printWidth;
	int labelTop;
	int labelShift;
	int tearOff;
	int darkness; 
	char font[20];
	
	/* Text Params */
	unsigned int width;
	
	/* Barcode Params */
	unsigned int col;
	unsigned int row;
	unsigned int quality;
} ZebraPrinter_ContentParams;

//==============================================================================
// External variables

//==============================================================================
// Global functions

void GetStandardErrMsg (int error, char errmsg[ERRLEN]);

int CVICALLBACK FunctionSelect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK RunFunction(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

int Initialize_ZebraPrinter_Lib (char *PrinterSerialName, char errmsg[ERRLEN]); 

int ZebraPrinter_AddToPrintCode(char *PrintCode, ZebraPrinter_ContentType Type, ZebraPrinter_ContentParams Params, char *Content, char errmsg[ERRLEN]);
int	ZebraPrinter_Print(char *PrinterSerialName, char *PrintCode, char errmsg[ERRLEN]);
void ZebraPrinter_Close_Printer(char *Printer, char errmsg[ERRLEN]);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ZebraPrinter_Lib_H__ */
