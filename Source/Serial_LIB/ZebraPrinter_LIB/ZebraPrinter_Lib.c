/***************************************************************************//*!
* \file ZebraPrinter_Lib.c
* \author Chao Zhang
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 5/29/2020 4:09:26 PM
* \brief A short description.
* 
* A longer description.
*
* Date		  | Name		  | Rev.		  | Description		
* ------------|---------------|---------------|--------------------
* 05-29-2020  | Chao Zhang	  | 1.0.0		  | Initial Release
* 09-01-2020  | Chao Zhang	  | 1.0.1		  | Added Teat off, Darkness
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond
//==============================================================================
// Include files

#include <ansi_c.h>
#include <utility.h>
#include "SerialComm_LIB.h"
#include "ZebraPrinter_Lib.h"

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

char ZebraPrinterCode[2048] = {0};	/* 20200530Biye: Double check what the max number of chars the printer or serial lib can send/receive at once*/

//==============================================================================
// Global functions

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond

/***************************************************************************//*!
* \brief Initializes the printer through SerialLib and checks the connection by
* 		 sending a status command
*******************************************************************************/
int Initialize_ZebraPrinter_Lib (char *PrinterSerialName, char errmsg[ERRLEN])
{
	fnInit;
	
	//initialize printer
	tsErrChk(InitSerialDevice(PrinterSerialName, errmsg), "Unable to initialize device: %s, ensure Serial library is initialized", PrinterSerialName);
	
	WriteSerialDevice(PrinterSerialName, "~HQES", errmsg);	//Send data to read printer status
	char resp[50] = {0};
	ReadSerialDevice(PrinterSerialName, resp, 3, errmsg);
	
	tsErrChk(strlen(resp)==0, "Printer No Response");
	
	memset (ZebraPrinterCode, 0, sizeof(ZebraPrinterCode));
	
	libInitialized = 1;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Adds new content to PrintCode, only relevant parameters need to be
* 		 set for params
* 
* \param [out]	PrintCode	The code to append to
* \param [in]	Type		The type of content to be added
* \param [in]	Params		The parameters related to the type of content,
* 							only parameters relevant to the content type needs to be set
* \param [in]	Content		The text content to be added
*******************************************************************************/
int ZebraPrinter_AddToPrintCode(char *PrintCode, ZebraPrinter_ContentType Type, ZebraPrinter_ContentParams Params, char *Content, char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (strstr(PrintCode,"^XZ")!=NULL,"PrintCode already contains a footer (^XZ), additional content cannot be added");
	
	if (Type >= ZP_String && Type < ZP_Logo)
	{
		libErrChk (strlen(Content)==0,"Content cannot be blank");
		libErrChk (strlen(Params.font)==0,"Font cannot be blank");
	}
	
	char code[2000] = {0};
	
	switch (Type)
	{
		case ZP_Header:
			libErrChk (strlen(PrintCode)>0,"PrintCode already has content\n%s\nHeader has to be at the beginning of the PrintCode",PrintCode);
			strcpy (code,"^XA");
			break;
		case ZP_Footer:
			strcpy (code,"\n^XZ");
			break;
		case ZP_PrintWidth:
			sprintf (code,"\n^PW%d",Params.printWidth);
			break;
		case ZP_TearOff:
			libErrChk ((Params.tearOff>120 || Params.tearOff<-120),"Tear Off param outside of acceptable range -120~120");
			sprintf (code,"\n~TA%d",Params.tearOff);
			break;
		case ZP_Darkness:
			libErrChk ((Params.darkness>30 || Params.darkness<0),"Darkness param outside of acceptable range -120~120");
			sprintf (code,"\n~SD%d",Params.darkness);
			break;
		case ZP_LabelTop:
			libErrChk ((Params.labelTop>120 || Params.labelTop<-120),"Label Top param outside of acceptable range -120~120");
			sprintf (code,"\n^LT%d",Params.labelTop);
			break;
		case ZP_LabelShift:
			libErrChk ((Params.labelShift>9999 || Params.labelShift<-9999),"Label Width param outside of acceptable range -9999~9999");
			sprintf (code,"\n^LS%d",Params.labelShift);
			break;
		case ZP_String:
			sprintf (code,"\n^FO%d,%d\n"	/* Location */
					 "^%s,%d,%d\n"			/* Font, Height, Width */
					 "^FD%s^FS",			/* Field Content */
					 Params.x,Params.y,
					 Params.font,Params.height,Params.width,
					 Content);
			break;
		case ZP_Barcode:
			sprintf (code,"\n^FO%d,%d\n"	/* Location */
					 "^%s,%d,%d,%d,%d\n"	/* Font, Height, Quality, Column, Row*/
					 /* 20200531Biye: Format, escape char, aspect ratio not added*/
					 "^FD%s^FS",			/* Field Content */
					 Params.x,Params.y,
					 Params.font,Params.height,Params.quality,Params.col,Params.row,
					 Content);
			break;
		case ZP_Logo:
			/* 20200531Biye: TODO */
			break;
		default:
			libErrChk (ERR_INVALID_CONTENT_TYPE,"Content Type %d is not valid",Type);
			break;
	}
	strcat (PrintCode,code);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief ZT610 Printer, start with ^XA
*******************************************************************************/
int ZebraPrinter_Code_Start (char errmsg[ERRLEN])
{
	libInit;
	
	strcpy(ZebraPrinterCode, "^XA");
	libErrChk(!strcmp(ZebraPrinterCode, ""), "Code Start Error!!");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 	 CodeContainer, x, y, font, height, width, print_string
*******************************************************************************/
int	ZebraPrinter_Code_String(char *x, char *y, char *font, char *height, char *width, char *print_string, char errmsg[ERRLEN])
{
	libInit;
	
	strcat(ZebraPrinterCode, "^FO"); //Start of location
	strcat(ZebraPrinterCode, x);strcat(ZebraPrinterCode, ",");  //x
	strcat(ZebraPrinterCode, y);  //y
	
	strcat(ZebraPrinterCode, "^");strcat(ZebraPrinterCode, font);strcat(ZebraPrinterCode, ","); //font
	strcat(ZebraPrinterCode, height);strcat(ZebraPrinterCode, ",");  //height
	strcat(ZebraPrinterCode, width);  //witdth
	
	strcat(ZebraPrinterCode, "^FD");strcat(ZebraPrinterCode, print_string);strcat(ZebraPrinterCode, "^FS");  //^FD**String**^FS
	
	libErrChk(!strcmp(ZebraPrinterCode, ""), "Code String Error!!");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 	 //CodeContainer, x, y, font, height, col, row, print_string
*******************************************************************************/
int	ZebraPrinter_Code_DataMatrix(char *x, char *y, char *font, char *height, char *col, char *row, char *print_string, char errmsg[ERRLEN])
{
	libInit;
	
	strcat(ZebraPrinterCode, "^FO"); //Start of location
	strcat(ZebraPrinterCode, x);strcat(ZebraPrinterCode, ",");  //x
	strcat(ZebraPrinterCode, y);  //y
	
	strcat(ZebraPrinterCode, "^");strcat(ZebraPrinterCode, font);strcat(ZebraPrinterCode, ","); //font
	strcat(ZebraPrinterCode, height);strcat(ZebraPrinterCode, ",");  //height
	
	strcat(ZebraPrinterCode, "200,"); //quality is set to be 200, the best quality
	strcat(ZebraPrinterCode, col);strcat(ZebraPrinterCode, ",");  //col
	strcat(ZebraPrinterCode, row);  //row
	
	strcat(ZebraPrinterCode, "^FD");strcat(ZebraPrinterCode, print_string);strcat(ZebraPrinterCode, "^FS");  //^FD**String**^FS
	
	libErrChk(!strcmp(ZebraPrinterCode, ""), "Code String Error!!");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief 	 //CodeContainer, x, y, font, height, col, row, print_string
*******************************************************************************/
int	ZebraPrinter_Code_End(char errmsg[ERRLEN])
{
	libInit;
	
	strcat(ZebraPrinterCode, "^XZ");	 //ZT610 Printer, end with ^XZ
	
	libErrChk(!strcmp(ZebraPrinterCode, ""), "Code End Error!!");
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Send PrintCode to the printer via serial
*******************************************************************************/
int	ZebraPrinter_Print(char *PrinterSerialName, char *PrintCode, char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk (strlen(PrintCode)==0,"Empty PrintCode");
	libErrChk (strncmp(PrintCode,"^XA",3)!=0,"PrintCode does not start with the expected header ^XA");
	libErrChk (strncmp(PrintCode+(strlen(PrintCode)-3),"^XZ",3)!=0,"PrintCode does not end with the expected footer ^XZ");
	
	WriteSerialDevice(PrinterSerialName, PrintCode, errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief close printer
*******************************************************************************/
void ZebraPrinter_Close_Printer(char *Printer, char errmsg[ERRLEN])
{
	CloseSerialDevice(Printer, errmsg);    //Close printer
	libInitialized = 0;
	
	return;
}
//! \cond
/// REGION END
//! \endcond
