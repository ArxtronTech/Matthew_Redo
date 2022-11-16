/***************************************************************************//*!
* \file main.c
* \author Jai Prajapati
* \copyright Arxtron Technologies Inc. All Rights Reserved.
* \date 11/9/2020 10:58:00 AM
* \brief Used to create an example of how the library should be used
* 
* Main should contain a series of functions from the library demonstrating how
* 	its expected to be used. Make sure to comment adequetly if anything might be
* 	confusing. It should cover every public function of the library.
* 
* If there is a debug panel for the library, also add the ability to launch
* 	it as a standalone application.
* 
* Main MUST also contain AUTOMATED test cases that cover possible error cases
* 	and should be ran and updated every time new features are added or major
* 	changes are made.
* 
* To use main.c, include it in the build and change the build target to Executable
*******************************************************************************/

//! \cond
/// REGION START HEADER
//! \endcond
//==============================================================================
// Include files

#include "SerialComm_LIB.h"
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "toolbox.h"
#include "GUIToolsLib.h"
#include "DebugToolsLib.h"
#include <string.h>

//==============================================================================
// Constants

/* Uncomment if using GUI debug panel */
//#define HASGUI

// Base coordinates for debug panel, all UI should be based on this so that it can be copy pasted onto another UI as a whole
#define OFFSET_X	25
#define OFFSET_Y	25
#define MainPanelWidth		550
#define MainPanelHeight		550

// Arbitrary count limits
#define MAX_FUNCTIONS 100
#define MAX_PARAMETERS 16

//==============================================================================
// Types

//==============================================================================
// Static global variables

// UI Vars
int glbFunctionCount = 0;
static int glbMainPanelHandle = 0;
static int glbFunctionChoiceHandle = 0;
static int glbParamHandles[MAX_PARAMETERS] = {0};
static int glbOutMessageHandle = 0;
static int glbOutErrorHandle = 0;
static int glbRunButtonHandle = 0;
static int glbFnIndex = 0;

// Logging Vars
static FILE *glbLogFile = 0;
static int glbCounter = 0;
static char glbLogParamFormat[1024] = {0};
static char glbLogParam[1024] = {0};
static char *glbTok = 0;

// Vars for Storing Function Parameters
static __int64  glbFunctionDebugParamTypes[MAX_FUNCTIONS][2];
static char glbFunctionParameters[MAX_FUNCTIONS][100];

//==============================================================================
// Static functions

static void usage (char *name)
{
	fprintf (stderr, "usage: %s <argument>\n", name);
	fprintf (stderr, "A short summary of the functionality.\n");
	fprintf (stderr, "    <argument>    is an argument\n");
	fprintf (stderr, "Order of operation: Change based on library\n");
	fprintf (stderr, "flag: function 1\n");
	fprintf (stderr, "flag: function 2\n");
	exit (1);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

int CVICALLBACK QuitUICB(int panel, int event, void *callbackData, int eventData1, int eventData2);

/***************************************************************************//*!
* \brief Adds function to GUI dropdown and add user defined input types
* 
* \param [in] FnCall		Pointer to the function being added
* \param [in] InputTypes	Hex representation of if parameter is a string (left to right)
* 							See BuildInputTypes macro for generating the InputTypes value
* \param [in] ...			The names of the function arguments
*******************************************************************************/
#define AddFnToDropdown(FnCall, InputTypes, ...)\
	InsertListItem(glbMainPanelHandle, glbFunctionChoiceHandle, glbFunctionCount, #FnCall, glbFunctionCount);\
	glbFunctionDebugParamTypes[glbFunctionCount][0] = InputTypes;\
	{\
		char inputStr[MAX_PARAMETERS] = {0};\
		sprintf(inputStr, "%I64x", (__int64)InputTypes);\
		glbFunctionDebugParamTypes[glbFunctionCount][1] = strlen(inputStr);\
	}\
	strcpy(glbFunctionParameters[glbFunctionCount++],#__VA_ARGS__)

//! \cond
/// REGION END

/// REGION START UI Callbacks
//! \endcond
/***************************************************************************//*!
* \brief Closes panel using X in upper LEFT corner.
*******************************************************************************/
int CVICALLBACK QuitUICB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief CVI Callback for the function dropdown to set parameter entry box names
*******************************************************************************/
int CVICALLBACK FunctionSelect (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_RING_BEGIN_MENU:
		case EVENT_COMMIT:
			int numArgs = -1;
			char *tok = 0;
			char params[100] = {0};
			GetCtrlIndex(panel, glbFunctionChoiceHandle, &glbFnIndex);
			numArgs = (int)glbFunctionDebugParamTypes[glbFnIndex][1];
			strcpy(params, glbFunctionParameters[glbFnIndex]);
			if (strstr(params,"errmsg")) --numArgs;
			tok = strtok(params, ",");
			for (int i = 0; i < MAX_PARAMETERS; ++i)
			{
				if (i < numArgs)
					SetCtrlAttribute(panel, glbParamHandles[i], ATTR_LABEL_TEXT, tok);
				else
					SetCtrlAttribute(panel, glbParamHandles[i], ATTR_LABEL_TEXT, "");
				tok = strtok(NULL, ",");
			}
			break;
	}
	return 0;
}

/***************************************************************************//*!
* \brief CVI Callback to run the selected function in the debug panel
*******************************************************************************/
int CVICALLBACK RunFunction(int panel, int control, int event, 
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int error = -99999;
			char params[MAX_PARAMETERS][1024] = {0};
			int numArgs = -1;
			__int64 inputTypes = -1;
			int argCounter = -1;
			char fnName[100] = {0};
			char errmsg[ERRLEN] = {0};
			 
			for(int i = 0; i < MAX_PARAMETERS; i++)
				GetCtrlVal(panel, glbParamHandles[i], params[i]);
		
			GetLabelFromIndex(panel, glbFunctionChoiceHandle, glbFnIndex, fnName);
			inputTypes = glbFunctionDebugParamTypes[glbFnIndex][0];
			numArgs = (int)glbFunctionDebugParamTypes[glbFnIndex][1];
			argCounter = numArgs;

			#define parseArg (((inputTypes>>((argCounter - 1) * 4))&0xF) == INT) ? atoi(params[numArgs - argCounter--]) : params[numArgs - argCounter--]

			/* 
				Add calls to functions added to GUI dropdown
				
				Format: DebugLibFn(FnPointer, inputTypes, parseArg *-> Number of function arguments)
			*/
			/*if(!strcmp(fnName, "Lib_Fn"))
				DebugLibFn(Lib_Fn, inputTypes, errmsg);*/
			 
			SetCtrlVal(panel, glbOutMessageHandle, errmsg);
			SetCtrlVal(panel, glbOutErrorHandle, error);
			
			break;
	}
	return 0;
}
//! \cond
/// REGION END

/// REGION START Main
//! \endcond
int main (int argc, char *argv[])
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	char currentDir[MAX_PATHNAME_LEN] = {0}, logPath[MAX_PATHNAME_LEN] = {0};
	GetDir(currentDir);
	sprintf (logPath,"%s\\debuglog.txt",currentDir);
	glbLogFile = fopen(logPath, "w");
	
#ifdef HASGUI
	
	/* Create the debug panel */
	CreatePanel(0, glbMainPanelHandle, "Function Testing", MainPanelHeight, MainPanelWidth, QuitUICB);
	
	/* Panel Title */
	int PanelTitleHandle = NewCtrl(glbMainPanelHandle, CTRL_TEXT_MSG, "Function Testing Panel", OFFSET_Y, 0);
	int TitleWidth = 0;
	SetCtrlAttribute(glbMainPanelHandle, PanelTitleHandle, ATTR_TEXT_POINT_SIZE, 14);
	GetCtrlAttribute(glbMainPanelHandle, PanelTitleHandle, ATTR_WIDTH, &TitleWidth);
	SetCtrlAttribute(glbMainPanelHandle, PanelTitleHandle, ATTR_TEXT_JUSTIFY, VAL_CENTER_JUSTIFIED);
	SetCtrlAttribute(glbMainPanelHandle, PanelTitleHandle, ATTR_LEFT, (MainPanelWidth-TitleWidth)/2);
	
	/* Add Debug Controls */
	AddRingList(glbMainPanelHandle, glbFunctionChoiceHandle, "Function Selection", OFFSET_X+25, OFFSET_Y+50, 200, 14);
	SetCtrlAttribute(glbMainPanelHandle, glbFunctionChoiceHandle, ATTR_CALLBACK_FUNCTION_POINTER, FunctionSelect);
	
	AddSquareButton(glbMainPanelHandle, glbRunButtonHandle, "RUN", OFFSET_X+300, OFFSET_Y+25, 50, 100, RunFunction);
	
	int CoordX = OFFSET_X+25, CoordY = OFFSET_Y+100;
	for (int i = 0; i < MAX_PARAMETERS; ++i)
	{
		AddStringBox(glbMainPanelHandle, glbParamHandles[i], "Param", CoordX, CoordY, 100, 0, 0);
		CoordX += 110;
		if(i % 4 == 3)
		{
			CoordY += 50;
			CoordX = OFFSET_X+25;
		}
	}
	
	AddTextBox(glbMainPanelHandle, glbOutMessageHandle, "Output", OFFSET_X+25, OFFSET_Y+300, 300, 0, 0);
	SetCtrlAttribute(glbMainPanelHandle, glbOutMessageHandle, ATTR_VISIBLE_LINES, 8);
	AddNumberBox(glbMainPanelHandle, glbOutErrorHandle, "Error Code", OFFSET_X+350, OFFSET_Y+300, 0, 0, 0, 0);
	SetCtrlAttribute(glbMainPanelHandle, glbOutErrorHandle, ATTR_NO_EDIT_TEXT, 1);
	SetCtrlAttribute(glbMainPanelHandle, glbOutErrorHandle, ATTR_DATA_TYPE, VAL_INTEGER);
	
	/*
		Add functions to the debug panel function selection dropdown 
	
		Format: AddFnToDropdown(FnPointer, InputTypes, arg1Name, arg2Name, ...)
		Note: Can use BuildInputTypes(...) function for InputTypes and specify inputs
			  by constants in DebugToolslib
			  Ex. AddFnToDropdown(FnPointer, BuildInputTypes(CHAR,INT), arg1Char, arg2Int)
	*/
	// AddFnToDropdown(Lib_Fn,BuildInputTypes(CHAR),errmsg);
	
	tsErrChk (DisplayPanel(glbMainPanelHandle), errmsg);
	tsErrChk (RunUserInterface(), errmsg);
	
#else	/* ifdef HASGUI */
	fprintf (stderr, "Use --help to see instructions\n");
	if(argc > 1 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")))
		usage(argv[0]);
	
	fprintf (stderr, "Initializing Library\n");
	DebugLibFn(InitializeSerialPortLib, 0x121, "SampleSerialConfig.xml", glbMainPanelHandle, errmsg);
	fprintf (stderr, "Library Initialized\n");
		
	// Parser for input arguments
	for(int i = 0; i < argc; i++)
	{
		if(!strcmp(argv[i], "-flag"))
		{
			// Call to some function(s), other setup, etc.
		}
	}
	
	/*
		Test a function using the DebugLibFn Macro
		
		Format: DebugLibFn(FnPointer, InputTypes, arg1, arg2, ...)
		Note: Can use BuildInputTypes(...) function for InputTypes and specify inputs
			  by constants in DebugToolslib
			  Ex. DebugLibFn(FnPointer, BuildInputTypes(CHAR,INT), arg1Char, arg2Int)
	*/
	DebugLibFn(InitSerialDevice, 0x11, "DeviceName", errmsg);
	
	DebugLibFn(WriteSerialDevice, 0x111, "DeviceName", "Data", errmsg);
	
#endif	/* ifdef HASGUI */

Error:
	/* clean up */
	if (glbMainPanelHandle > 0)
		DiscardPanel (glbMainPanelHandle);
	if (error)
	{
		fprintf (stderr, errmsg);
		GetKey();
	}
	if (glbLogFile) fclose(glbLogFile);
	return error;
}
//! \cond
/// REGION END
//! \endcond