/***************************************************************************//*!
* \file SerialComm_LIB.c
* \author Arxtron
* \copyright Arxtron Technologies Inc. All Rights Reserved.
* \date 11/9/2020
* \brief Serial communication library.
* 
* Library used to communicate to serial devices. Device configuration is loaded
* by an XML file. A panel for the configuration table and debug is created on
* initialization of this library and can be used by accessing the panel handles
* stored as global variables.
* 
* Version     |   Date        |   Author          |   Description
* ------------|---------------|-------------------|-----------------------------
* 1.0.0       | May 5, 2014   | Arxtron      	  | Initial Release
* 1.0.1		  | Nov 9, 2020   | Jai Prajapati     | Updated with library format
*******************************************************************************/

//! \cond
/// REGION START Header
//! \endcond

//==============================================================================
// Include files

#include "cvixml.h"
#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include <formatio.h>
#include "SerialComm_LIB.h"
#include <rs232.h>

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

char tempCheckSum[4] = {0};
char glbPathToSerialConfigFile[256] = {0};
int glbSerialConfigurationPanelHandle = 0;
int glbSerialConfigTableHandle = 0;
int glbSerialDebugPanelHandle;
int glbNumOfComPorts = 0;
int glbSerialRingDebugMenuHandle;
int glbWriteBoxHandle;
int glbSerialThreadID = 0;
int glbSerialThread = 0;
static int glbSerialReadThreadHandle;
int glbReadBoxHandle;

SerialFileInfoStruct glbSerialFileInfo[MAXNUMOFSERIALPORTS]= {0};
char glbSerialParamName[9][20]= {"DeviceName","Comport","BaudRate","Parity","DataBits","StopBits","CTSMode","XonXoff","Timeout"};

//==============================================================================
// Global functions

void CreateSerialConfigurationTable(int MainPanelHandle);
void CreateSerialDebugPanel(int MainPanelHandle);

void PopulateComboBox(int RowNum);
void LoadSerialConfigFile(void);

int CVICALLBACK  SaveSerialConfigTableCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  SerialTableCBFunction (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  QuitSerialConfigTableCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  DelRowSerialConfigTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  AddRowSerialConfigTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

int CVICALLBACK  InitSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  QuitSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  WriteSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  ReadSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK  ClearSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);

void DisplayRS232Error (int RS232Error);
int getFileInfoIndexFromName(char * DeviceName);
int SerialReadThread (void *dummy);
int SaveBackupXmlFilenameSerial(const char *filename);
int HexToCharInString(char *string);
int ISValidXMLSerial (char * string);

//! \cond
/// REGION END

/// REGION START Code Body
//! \endcond

/***************************************************************************//*!
* \brief Initialize the serial library with configuration file
*
* Serial Port configuration table panel and the Serial debug panel are created
* 
* \param [in] SerialConfiguarationFile 	Path to config XML file
* \param [in] MainPanelHandle			Parent panel handle to create serial child
* 									    panels under
*******************************************************************************/
int InitializeSerialPortLib(char *SerialConfigurationFile, int MainPanelHandle, char errmsg[ERRLEN])
{
	fnInit;
	
	glbNumOfComPorts = ReadSerialConfigurationFile(SerialConfigurationFile);
	tsErrChk(glbNumOfComPorts < 0 ? -1 : 0, "No communication ports found in configuration file");
	
	CreateSerialConfigurationTable(MainPanelHandle);
	CreateSerialDebugPanel(MainPanelHandle);
	
	libInitialized = 1;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Create the Serial configuration table. Read from the specified file 
* 		 and populates the Serial config table.
*
* \param [in] MainPanelHandle 	Parent panel handle to create serial child
* 							    panel under
*******************************************************************************/
void CreateSerialConfigurationTable(int MainPanelHandle)
{
	glbSerialConfigurationPanelHandle = NewPanel (MainPanelHandle, "Serial Configuration", 56, 8, 700, 620);
	glbSerialConfigTableHandle = NewCtrl (glbSerialConfigurationPanelHandle, CTRL_TABLE, "", 0, 0);
	SetCtrlAttribute(glbSerialConfigurationPanelHandle,glbSerialConfigTableHandle,ATTR_HEIGHT,650);
	SetCtrlAttribute(glbSerialConfigurationPanelHandle,glbSerialConfigTableHandle,ATTR_WIDTH,615);
	SetCtrlAttribute( glbSerialConfigurationPanelHandle,glbSerialConfigTableHandle,ATTR_DATA_TYPE,VAL_INTEGER);
	SetCtrlAttribute( glbSerialConfigurationPanelHandle,glbSerialConfigTableHandle,ATTR_CALLBACK_FUNCTION_POINTER ,SerialTableCBFunction);

	int SaveButtonHandle = NewCtrl (glbSerialConfigurationPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Save", 660, 500);
	SetCtrlAttribute (glbSerialConfigurationPanelHandle,SaveButtonHandle, ATTR_CALLBACK_FUNCTION_POINTER,SaveSerialConfigTableCB);

	int ExitButtonHandle = NewCtrl (glbSerialConfigurationPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Exit", 660, 550);
	SetCtrlAttribute (glbSerialConfigurationPanelHandle,ExitButtonHandle, ATTR_CALLBACK_FUNCTION_POINTER,QuitSerialConfigTableCB);

	int AddButtonHandle = NewCtrl (glbSerialConfigurationPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Add Row", 660, 10);
	SetCtrlAttribute (glbSerialConfigurationPanelHandle,AddButtonHandle, ATTR_CALLBACK_FUNCTION_POINTER,AddRowSerialConfigTableCB);

	int DelButtonHandle = NewCtrl (glbSerialConfigurationPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Delete Row", 660, 75);
	SetCtrlAttribute (glbSerialConfigurationPanelHandle,DelButtonHandle, ATTR_CALLBACK_FUNCTION_POINTER,DelRowSerialConfigTableCB);

	InsertTableRows(glbSerialConfigurationPanelHandle,glbSerialConfigTableHandle,-1,glbNumOfComPorts,VAL_CELL_STRING);
	
	#define insertTableColumn(colIndex, paramIndex, cellType, colWidth)\
		InsertTableColumns (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, colIndex, 1, cellType);\
		SetTableColumnAttribute (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, colIndex, ATTR_LABEL_TEXT, glbSerialParamName[paramIndex]);\
		SetTableColumnAttribute (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, colIndex, ATTR_USE_LABEL_TEXT, 1);\
		SetTableColumnAttribute (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, colIndex, ATTR_COLUMN_WIDTH, colWidth);\
		SetTableColumnAttribute (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, colIndex, ATTR_CELL_TYPE, cellType)

	insertTableColumn(1, 0, VAL_CELL_STRING, 150);
	insertTableColumn(2, 1, VAL_CELL_COMBO_BOX, 50);
	insertTableColumn(3, 2, VAL_CELL_COMBO_BOX, 50);
	insertTableColumn(4, 3, VAL_CELL_COMBO_BOX, 50);
	insertTableColumn(5, 4, VAL_CELL_COMBO_BOX, 50);
	
	if(glbNumOfComPorts>0)
	{
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM1" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM2" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM3" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM4" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM5" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM6" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM7" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM8" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM9" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 2, glbNumOfComPorts, 1), -1,"COM10" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"110" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"110" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"300" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"600" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"1200" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"2400" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"4800" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"9600" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"19200" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"38400" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"57600" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"115200" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 3, glbNumOfComPorts, 1), -1,"230400" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 4, glbNumOfComPorts, 1), -1,"None" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 4, glbNumOfComPorts, 1), -1,"Odd" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 4, glbNumOfComPorts, 1), -1,"Even" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 4, glbNumOfComPorts, 1), -1,"Mark" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 4, glbNumOfComPorts, 1), -1,"Space" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 5, glbNumOfComPorts, 1), -1,"5" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 5, glbNumOfComPorts, 1), -1,"6" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 5, glbNumOfComPorts, 1), -1,"7" );
		InsertTableCellRangeRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakeRect(1, 5, glbNumOfComPorts, 1), -1,"8" );
	}
	
	insertTableColumn(6, 5, VAL_CELL_BUTTON, 50);
	insertTableColumn(7, 6, VAL_CELL_BUTTON, 50);
	insertTableColumn(8, 7, VAL_CELL_BUTTON, 50);
	insertTableColumn(9, 8, VAL_CELL_NUMERIC, 50);

	LoadSerialConfigFile();
}

/***************************************************************************//*!
* \brief read the xml Serial configuration from specified path and populate 
* 		 glbSerialFileInfo structure
*
* \param [in] filePath 		Path to serial configuration XML file
* 
* \return The number of serial devices found in XML file
*******************************************************************************/
int ReadSerialConfigurationFile(char *filePath)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	CVIXMLElement    curElem = 0;
	CVIXMLDocument   doc;
	CVIXMLElement 	 childElem = 0;
	CVIXMLElement 	 childElem1 = 0;
	int numChildren = 0;
	int numSubChildren = 0;
	char value[120];
	int address;
	
	sprintf(glbPathToSerialConfigFile,"%s",filePath);
	
	// load document
	tsErrChk(CVIXMLLoadDocument(filePath, &doc), errmsg);
	tsErrChk(CVIXMLGetRootElement(doc, &curElem), errmsg);

	tsErrChk(CVIXMLGetNumChildElements(curElem,&numChildren), errmsg);
	
	for (int i=0; i<numChildren; i++)
	{
		CVIXMLGetChildElementByIndex(curElem,i,&childElem);
		CVIXMLGetNumChildElements(childElem,&numSubChildren);
		for(int j=0; j<numSubChildren; j++)
		{
			CVIXMLGetChildElementByTag(childElem,glbSerialParamName[j],&childElem1);
			CVIXMLGetElementValue(childElem1,value);

			//Take the address glbSerialFileInfo[k].DeviceName[0] and cast it to int. Since all elements of this
			//SerialInfo[0] struct are the same size to go to the next element we simply add the size of it.
			address =  ((int)&(glbSerialFileInfo[i].DeviceName[0]))+(j*sizeof(glbSerialFileInfo[0].DeviceName)) ;
			sprintf((char*)address,"%s",value);

			if(childElem1)
				CVIXMLDiscardElement(childElem1);
		}
		if(childElem)
			CVIXMLDiscardElement(childElem);
	}
	
	if(curElem)
		CVIXMLDiscardElement(curElem);
	
	if(doc)
		CVIXMLDiscardDocument(doc);
	
Error:
	if(error)
		return error;
	else
		return(numChildren);
}

/***************************************************************************//*!
* \brief Get SerialConfiguration panel handle
*
* \return glbSerialConfigurationPanelHandle
*******************************************************************************/
int GetSerialConfigurationPanelHandle(void)
{
	return  glbSerialConfigurationPanelHandle;
}

/***************************************************************************//*!
* \brief Populates the combo box for newly added row
*
*******************************************************************************/
void PopulateComboBox(int RowNum)
{
	InsertTableCellRingItem(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM1" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM2" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM3" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM4" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM5" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM6" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM7" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM8" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM9" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(2,RowNum), -1,"COM10" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"110" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"300" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"600" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"1200" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"2400" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"4800" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"9600" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"19200" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"38400" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"57600" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"115200" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(3,RowNum), -1,"230400" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(4,RowNum), -1,"None" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(4,RowNum), -1,"Odd" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(4,RowNum), -1,"Even" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(4,RowNum), -1,"Mark" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(4,RowNum), -1,"Space" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(5,RowNum), -1,"5" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(5,RowNum), -1,"6" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(5,RowNum), -1,"7" );
	InsertTableCellRingItem (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint(5,RowNum), -1,"8" );
	SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(6,RowNum),"1");
	SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(7,RowNum),"Off");
	SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(8,RowNum),"Off");
	SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(9,RowNum),5);
}

/***************************************************************************//*!
* \brief Save backup copy of the existing Serial config xml file
*
*******************************************************************************/
int SaveBackupXmlFilenameSerial(const char *filename)
{
	char errmsg[ERRLEN] = {0};
	fnInit;
	
	char backUpFilename[MAXCHARARRAYLENGTH];
	char tempStr[MAXCHARARRAYLENGTH];
	int year,month,day,hour,min,sec;
	GetSystemTime(&hour,&min,&sec);
	GetSystemDate(&month,&day,&year);
	strcpy(backUpFilename,filename);
	char * pch;
	pch = strstr(backUpFilename,".xml");
	sprintf(tempStr,"%04d%02d%02d%02d%02d%02d.bak",year,month,day,hour,min,sec);
	strncpy(pch,tempStr,19);
	tsErrChk(CopyFile(filename,backUpFilename), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Create the Serial debug Panel used to manually turn on and off signals.
*
* \param [in] MainPanelHandle 	Parent panel handle to  create debug panel under
*******************************************************************************/
void CreateSerialDebugPanel(int MainPanelHandle)
{
	CmtNewThreadPool(1,&glbSerialReadThreadHandle);
	glbSerialDebugPanelHandle = NewPanel (MainPanelHandle, "Serial Debug Utility", 56, 8, 530, 500);

	int SerialDebugPanelExitButtonHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Exit", 500, 450);
	SetCtrlAttribute (glbSerialDebugPanelHandle,SerialDebugPanelExitButtonHandle, ATTR_CALLBACK_FUNCTION_POINTER,QuitSerialDebugCB);
	glbWriteBoxHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_TEXT_BOX, "String To Write(Use \\0xXX for ASCII char eg:\\0x0D)", 420, 35);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbWriteBoxHandle, ATTR_LABEL_LEFT,35);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbWriteBoxHandle, ATTR_WIDTH ,200);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbWriteBoxHandle, ATTR_HEIGHT,25);
	int WriteSerialHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Write", 420, 238);
	SetCtrlAttribute (glbSerialDebugPanelHandle,WriteSerialHandle, ATTR_CALLBACK_FUNCTION_POINTER,WriteSerialDebugCB);

	glbReadBoxHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_TEXT_BOX, "Read Buffer", 30, 35);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbReadBoxHandle, ATTR_SCROLL_BARS,VAL_VERT_SCROLL_BAR);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbReadBoxHandle, ATTR_NO_EDIT_TEXT,1);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbReadBoxHandle, ATTR_WIDTH ,410);
	SetCtrlAttribute (glbSerialDebugPanelHandle,glbReadBoxHandle, ATTR_HEIGHT,300);
	int ReadSerialBufferHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Start Reading", 340, 195);
	SetCtrlAttribute (glbSerialDebugPanelHandle,ReadSerialBufferHandle, ATTR_CALLBACK_FUNCTION_POINTER,ReadSerialDebugCB);

	int ClearSerialBufferHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Clear", 340, 400);
	SetCtrlAttribute (glbSerialDebugPanelHandle,ClearSerialBufferHandle, ATTR_CALLBACK_FUNCTION_POINTER,ClearSerialDebugCB);

	glbSerialRingDebugMenuHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_RECESSED_MENU_RING, "Select Device", 420, 320);
	int InitSerialHandle = NewCtrl (glbSerialDebugPanelHandle, CTRL_SQUARE_COMMAND_BUTTON, "Initialize", 417, 400);
	SetCtrlAttribute (glbSerialDebugPanelHandle,InitSerialHandle, ATTR_CALLBACK_FUNCTION_POINTER,InitSerialDebugCB);
	for(int i=0; i<glbNumOfComPorts; i++)
		InsertListItem(glbSerialDebugPanelHandle,glbSerialRingDebugMenuHandle,-1,glbSerialFileInfo[i].DeviceName,i);
}

/***************************************************************************//*!
* \brief Get SerialDebug panel handle
*
*******************************************************************************/
int GetSerialDebugPanelHandle(void)
{
	return  glbSerialDebugPanelHandle;
}

/***************************************************************************//*!
* \brief Get device name of specified index (0based)
*
* \param [in] 	index 	 	Index of serial file information
* \param [out]  devName		Serial device name
*******************************************************************************/
int GetDeviceName(int index, char *devName, char errmsg[ERRLEN])
{
	libInit;
	
	libErrChk(glbNumOfComPorts <= (index-1) ? -1 : 0, "Invalid serial port index: %d. Ports initialized: %d", index, glbNumOfComPorts);
	
	strcpy (devName, glbSerialFileInfo[index-1].DeviceName);
	
Error:
	if(error)
		devName = NULL;
	return error;
}

/***************************************************************************//*!
* \brief Get number of devices in SerialInfo
*
*******************************************************************************/
int GetTotalSerialDevices(void)
{
	return glbNumOfComPorts;
}

/***************************************************************************//*!
* \brief Serial Read  thread to continiously read the buffer
*
*******************************************************************************/
int SerialReadThread (void *dummy)
{
	char errmsg[ERRLEN] = {0};
	int value=0;
	while(glbSerialThread==1 && (GetPanelAttribute(glbSerialDebugPanelHandle,ATTR_HEIGHT,&value)>-1))
	{
		int index=0;
		char deviceName[256]= {0};
		char data[1000]= {0};
		GetCtrlVal(glbSerialDebugPanelHandle,glbSerialRingDebugMenuHandle,&index);
		GetLabelFromIndex (glbSerialDebugPanelHandle, glbSerialRingDebugMenuHandle,index, deviceName);

		int i = getFileInfoIndexFromName(deviceName);
		if(glbSerialFileInfo[i].PortOpen==1)
		{
			int inqlen = GetInQLenForDeviceName(deviceName, errmsg);
			if(inqlen>0)
			{
				ReadSerialDevice(deviceName, data, inqlen, errmsg);
				strcat(data,"\0");
				SetCtrlVal (glbSerialDebugPanelHandle, glbReadBoxHandle, data);
			}
		}
		DelayWithEventProcessing(0.2);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Initialize a Serial Device
*
* \param [in] SerialDeviceName 		Name of serial device to initialize
*******************************************************************************/
int InitSerialDevice (char *SerialDeviceName, char errmsg[ERRLEN])
{
	libInit;
	
	int index = 0;
	int comport = 0;
	int parity = 0;
	int baudRate = 0;
	int databits = 0;
	int stopbits = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);

	if(stricmp(glbSerialFileInfo[index].Parity, "None")==0)
		parity = 0;
	else if(stricmp(glbSerialFileInfo[index].Parity, "Odd")==0)
		parity = 1;
	else if(stricmp(glbSerialFileInfo[index].Parity, "Even")==0)
		parity = 2;
	else if(stricmp(glbSerialFileInfo[index].Parity, "Mark")==0)
		parity = 3;
	else if(stricmp(glbSerialFileInfo[index].Parity, "Space")==0)
		parity = 4;

	DisableBreakOnLibraryErrors ();
	baudRate = atoi(glbSerialFileInfo[index].BaudRate);
	databits = atoi(glbSerialFileInfo[index].DataBits);
	stopbits = atoi(glbSerialFileInfo[index].StopBits);

	error = OpenComConfig(comport, glbSerialFileInfo[index].Comport, baudRate, parity,databits, stopbits, 512, 512);
	EnableBreakOnLibraryErrors ();

	if (error)
	{
		glbSerialFileInfo[index].PortOpen=0;
		DisplayRS232Error (error);
		return -1;
	}
	else
	{
		glbSerialFileInfo[index].PortOpen=1;
		if(stricmp(glbSerialFileInfo[index].XonXoff, "On")==0)
			SetXMode (comport, 1);
		else
			SetXMode (comport, 0);
		if(stricmp(glbSerialFileInfo[index].CTSMode, "On")==0)
			SetCTSMode (comport, 1);
		else
			SetCTSMode (comport, 0);
		SetComTime (comport, atoi(glbSerialFileInfo[index].Timeout));
	}
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Close a Serial Device
*
* \param [in] SerialDeviceName 		Name of serial device to close
*******************************************************************************/
int CloseSerialDevice (char *SerialDeviceName, char errmsg[ERRLEN])
{
	libInit;
	
	int index = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
			  
	error = CloseCom(atoi(glbSerialFileInfo[index].Comport + 3));
	if (error)
		DisplayRS232Error (error);
	glbSerialFileInfo[index].PortOpen=0;
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Write To a specified serial device
*
* \param [in] SerialDeviceName 		Name of serial device to write to
* \param [in] data 					Data to write
*
* \return The number of bytes written or negative error code
*******************************************************************************/
int WriteSerialDevice(char *SerialDeviceName, char *data, char errmsg[ERRLEN])
{
	int bytesWritten = 0;
	libInit;
	
	int index = 0;
	int comport = 0;
	
	int stringsize = StringLength (data);
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	if(glbSerialFileInfo[index].PortOpen!=1)
	{
		sprintf(errmsg,"Please Initalize device %s before trying to write",SerialDeviceName);
		MessagePopup ("WriteSerialDevice Error", errmsg);
		libErrChk(-1, errmsg);
	}
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	FlushInQ(comport);
	bytesWritten = ComWrt(comport, data, stringsize);
	
Error:
	if(error)
		return error;
	else
		return bytesWritten;
}

/***************************************************************************//*!
* \brief Write To specified serial device with specified data length
*
* \param [in] SerialDeviceName 		Name of serial device to write to
* \param [in] data 					Data to write
* \param [in] dataLen 				Length of data
* 
* \return The number of bytes written or negative error code
*******************************************************************************/
int WriteSerialDeviceRaw(char *SerialDeviceName, char *data, int dataLen, char errmsg[ERRLEN])
{
	int bytesWritten = 0;
	libInit;
	
	int index = 0;
	int comport = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	if(glbSerialFileInfo[index].PortOpen!=1)
	{
		sprintf(errmsg,"Please Initalize device %s before trying to write",SerialDeviceName);
		MessagePopup ("WriteSerialDevice Error", errmsg);
		libErrChk(-1, errmsg);
	}
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	FlushInQ(comport);
	bytesWritten = ComWrt(comport, data, dataLen);
	
Error:
	if(error)
		return error;
	else
		return bytesWritten;
}

/***************************************************************************//*!
* \brief Read from specified serial device
*
* \param [in] 	SerialDeviceName 			Name of serial device to read from
* \param [out]  ReadData 					
* \param [in] 	numBytesToRead 				Number of bytes to read
*
* \return The number of bytes read or negative errorcode
*******************************************************************************/
int ReadSerialDevice(char *SerialDeviceName, char *ReadData, int numByteToRead, char errmsg[ERRLEN])
{
	int bytesRead = 0;
	libInit;
	
	int index = 0;
	int comport = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	if(glbSerialFileInfo[index].PortOpen!=1)
	{
		sprintf(errmsg,"Please Initalize device %s before trying to read",SerialDeviceName);
		MessagePopup ("ReadSerialDevice Error", errmsg);
		libErrChk(-1, errmsg);
	}
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	bytesRead = ComRd(comport, ReadData, numByteToRead);
	
Error:
	if(error)
		return error;
	else
		return bytesRead;
}

/***************************************************************************//*!
* \brief Read from specified serial device until termination char is found or 
* 		 timeout or number of byte to read is reached.
*
* \param [in] 	SerialDeviceName 			Name of serial device to read from
* \param [out]  ReadData 					
* \param [in] 	numBytesToRead 				Number of bytes to read
* \param [in]   terminationByte				Termination char to stop reading
*
* \return The number of bytes read or negative error code
*******************************************************************************/
int ReadSerialDeviceUntilTermChar(char *SerialDeviceName, char *ReadData, int numByteToRead, int terminationByte, char errmsg[ERRLEN])
{
	int bytesRead = 0;
	libInit;
	
	int index = 0;
	int comport = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	if(glbSerialFileInfo[index].PortOpen!=1)
	{
		sprintf(errmsg,"Please Initalize device %s before trying to read",SerialDeviceName);
		MessagePopup ("ReadSerialDevice Error", errmsg);
		libErrChk(-1, errmsg);
	}
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	bytesRead = ComRdTerm(comport, ReadData, numByteToRead, terminationByte);
	
Error:
	if(error)
		return error;
	else
		return bytesRead;
}

/***************************************************************************//*!
* \brief returns the index in the glbSerialFileInfo array for the deviceName
*
* \param [in] DeviceName 		Name of device to find
*******************************************************************************/
int getFileInfoIndexFromName(char *DeviceName)
{
	for(int i=0; i< glbNumOfComPorts; i++)
	{
		if(stricmp(glbSerialFileInfo[i].DeviceName,DeviceName)==0)
			return i;
	}
	return -1;
}

/***************************************************************************//*!
* \brief This function takes the data previously loaded into Serial struct
* 		 and populats the test configuration table for each com port opened
*
*******************************************************************************/
void LoadSerialConfigFile(void)
{
	for(int i=0; i<glbNumOfComPorts; i++)
	{
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (1, i+1), glbSerialFileInfo[i].DeviceName);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (2, i+1), glbSerialFileInfo[i].Comport);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (3, i+1), glbSerialFileInfo[i].BaudRate);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (4, i+1), glbSerialFileInfo[i].Parity);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (5, i+1), glbSerialFileInfo[i].DataBits);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (6, i+1), glbSerialFileInfo[i].StopBits);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (7, i+1), glbSerialFileInfo[i].CTSMode);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (8, i+1), glbSerialFileInfo[i].XonXoff);
		SetTableCellVal (glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle, MakePoint (9, i+1), atoi(glbSerialFileInfo[i].Timeout));
	}
}

/***************************************************************************//*!
* \brief Check if string is a valid XML tag
*
* \param [in] string
*******************************************************************************/
int ISValidXMLSerial (char * string)
{
	int 	i = 0;
	size_t	len = 0;
	int 	allNums = 0;

	if (string[0] == '\0')
		return 0;

	len = strlen(string);
	for (i = 0; i < len; i++)
	{
		if (isdigit (string[i]))
		{
			if (i == 0)
				allNums = 1;
		}
		else
		{
			allNums = 0;
		}

		if (!isalnum (string[i]) || isspace (string[i]))
			return 0;
	}

	if (allNums)
		return 0;

	return 1;
} 

/***************************************************************************//*!
* \brief Display help information to the user based on RS232Error code
*
* \param [in] RS232Error 	Error code
*******************************************************************************/
void DisplayRS232Error (int RS232Error)
{
	char ErrorMessage[200];
	switch (RS232Error)
	{
		default :
			if (RS232Error < 0)
			{
				Fmt (ErrorMessage, "%s<RS232 error number %i", RS232Error);
				MessagePopup ("RS232 Message", ErrorMessage);
			}
			break;
		case 0  :
			MessagePopup ("RS232 Message", "No errors.");
			break;
		case -2 :
			Fmt (ErrorMessage, "%s", "Invalid port number (must be in the "
				 "range 1 to 8).");
			MessagePopup ("RS232 Message", ErrorMessage);
			break;
		case -3 :
			Fmt (ErrorMessage, "%s", "No port is open.\n"
				 "Check COM Port setting in Configure.");
			MessagePopup ("RS232 Message", ErrorMessage);
			break;
		case -99 :
			Fmt (ErrorMessage, "%s", "Timeout error.\n\n"
				 "Either increase timeout value,\n"
				 "       check COM Port setting, or\n"
				 "       check device.");
			MessagePopup ("RS232 Message", ErrorMessage);
			break;
	}
}

/***************************************************************************//*!
* \brief Get the in queue length for a specified device
*
* \param [in] SerialDeviceName 	Name of device  
* 
* \return the in queue length or error code
* 		  - 0 -> COM Port status error
*******************************************************************************/
int GetInQLenForDeviceName(char *SerialDeviceName, char errmsg[ERRLEN])
{
	int queueLength = 0;
	libInit;
	
	int comport = 0;
	int index = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);  // comport number, i.e. skip COM prefix in the name

	if (GetComStat(comport))
		return 0;
	
	queueLength = GetInQLen(comport);
	
Error:
	if(error)
		return error;
	else
		return queueLength;
}

int HexToCharInString(char *string)
{
	while(strstr(string,"\\0x"))
	{
		char temp[512]= {0};
		strcpy(temp,string);
		char *tempA = strstr(temp,"\\0x");
		char newstring[512]= {0};
		strncpy(newstring,string,strlen(string)-strlen(tempA));
		char tempB[512]= {0};
		strncpy(tempB,tempA+3,2);
		//strcat(string,atoi(tempB));
		int val = strtol(tempB,NULL,16);
		sprintf(newstring,"%s%c",newstring,val);
		strcat(newstring, tempA+5);
		strcpy(string,newstring);
	}
	return 0;
}

/***************************************************************************//*!
* \brief get library revision
*
*******************************************************************************/
char *getSerialLibRevision (void)
{
	return SERIALLIBREV;
}

int ReadMotor(char *Motor)
{
	char errmsg[ERRLEN] = {0};
	libInit;
	
	char SmallBuff[256]= {0};
	char BigBuff[512]= {0};
	int inqlen=0;
	int timeout=0;
	FlushInQ(1);
	while(1)
	{
		inqlen = GetInQLenForDeviceName(Motor, errmsg);
		if(inqlen>0)
		{
			memset(SmallBuff,0,sizeof(SmallBuff));
			ReadSerialDevice(Motor, SmallBuff, inqlen, errmsg);
			strcat(BigBuff,SmallBuff);
		}
		if(strstr(BigBuff,"ACK"))
		{
			break;
		}
		timeout++;
		if(timeout>100)
		{
			MessagePopup(Motor,"No Response from Motor");
			return -1;
		}
		DelayWithEventProcessing(0.1);
	}

	FlushInQ(1);
	
Error:
	return 0;
}

/***************************************************************************//*!
* \brief Flush the IN queue for the specified serial device COM port
*
* \param [in] SerialDeviceName 	Name of device
*******************************************************************************/
int FlushInQDevice(char *SerialDeviceName, char errmsg[ERRLEN])
{
	libInit;
	
	int index = 0;
	int comport = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	
	libErrChk(FlushInQ(comport), errmsg);
	
Error:
	return error;
}

/***************************************************************************//*!
* \brief Flush the OUT queue for the specified serial device COM port
*
* \param [in] SerialDeviceName 	Name of device
*******************************************************************************/
int FlushOutQDevice(char *SerialDeviceName, char errmsg[ERRLEN])
{
	libInit;
	
	int index = 0;
	int comport = 0;
	
	index = getFileInfoIndexFromName(SerialDeviceName);
	libErrChk(index < 0 ? -1 : 0, "Serial information for device: %s not available. Ensure config file contains information", SerialDeviceName);
	
	comport = atoi(glbSerialFileInfo[index].Comport + 3);
	
	libErrChk(FlushOutQ(comport), errmsg);
	
Error:
	return error;
}

/// REGION START CVI Callbacks
//! \endcond

/***************************************************************************//*!
* \brief Callback function for add test button. Adds a test row below the selected
* 		 row in the Serial configuration table.
*******************************************************************************/
int CVICALLBACK  AddRowSerialConfigTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK_UP)
	{
		Point SelectedCell;
		int SelectedRow=0;
		GetActiveTableCell(panel,glbSerialConfigTableHandle,&SelectedCell);
		SelectedRow = SelectedCell.y;
		if (SelectedRow == 0)
		{
			InsertTableRows(panel,glbSerialConfigTableHandle,-1,1,VAL_USE_MASTER_CELL_TYPE);	// insert top row
			PopulateComboBox(1);
		}
		else
		{
			InsertTableRows(panel,glbSerialConfigTableHandle,SelectedRow+1,1,VAL_USE_MASTER_CELL_TYPE);
			PopulateComboBox(SelectedRow+1);
		}
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback function for delete row button. Delete the slected test row 
* 		 from Serial configuration table
*******************************************************************************/
int CVICALLBACK  DelRowSerialConfigTableCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK_UP)
	{
		Point SelectedCell;
		int SelectedRow=0;
		GetActiveTableCell(panel,glbSerialConfigTableHandle,&SelectedCell);
		SelectedRow = SelectedCell.y;
		if (SelectedRow > 0)
		{
			DeleteTableRows(panel,glbSerialConfigTableHandle,SelectedRow,1);
		}
		else
		{
			char msg[256];
			sprintf(msg,"Unable to delete row %i",SelectedRow);
			MessagePopup("Error",msg);
		}
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback function for Quit/hide Serial Config panel
*******************************************************************************/
int CVICALLBACK  QuitSerialConfigTableCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		HidePanel(glbSerialConfigurationPanelHandle);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for the Save button in Serial Configuration panel
*		
* This function create a backup of the current serial Configuration xml file
* and saves the new config into the file.
*******************************************************************************/
int CVICALLBACK  SaveSerialConfigTableCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{

	if(event == EVENT_COMMIT )
	{
		int error;
		char msg[MAXCHARARRAYLENGTH];
		CVIXMLDocument   doc;
		CVIXMLElement    curElem = 0;
		CVIXMLElement 	 childElem = 0;
		CVIXMLElement 	 childElem1 = 0;
		char rootTag[]= {"SerialHW"};
		int numRows;
		char tag[120];
		char value[120];
		int intVal=0;
		error = SaveBackupXmlFilenameSerial(glbPathToSerialConfigFile);

		//CVIXMLLoadDocument(filePath,&doc);
		CVIXMLNewDocument(rootTag,&doc);
		CVIXMLGetRootElement(doc,&curElem);

		// get number of table rows
		GetNumTableRows(panel,glbSerialConfigTableHandle,&numRows);
		if (numRows <= 0)
		{
			sprintf(msg,"Invalid number of row %i",numRows);
			MessagePopup("Error",msg);
			return(-1);
		}
		for (int i=numRows-1; i>=0; i--)
		{
			CVIXMLNewElement(curElem,0,"Serial",&childElem);
			for(int j=8; j>=0; j--)
			{
				GetTableColumnAttribute(panel,glbSerialConfigTableHandle,j+1,ATTR_LABEL_TEXT,tag);
				if(j==8)
				{
					GetTableCellVal(panel,glbSerialConfigTableHandle,MakePoint(j+1,i+1),&intVal);
					sprintf(value,"%d",intVal);
				}
				else
					GetTableCellVal(panel,glbSerialConfigTableHandle,MakePoint(j+1,i+1),value);
				if (ISValidXMLSerial(tag))
				{
					CVIXMLNewElement(childElem,0,tag,&childElem1);
					CVIXMLSetElementValue(childElem1,value);
				}
				else
				{
					MessagePopup("Invalid XML tag",tag);
				}
				if (childElem1)
				{
					CVIXMLDiscardElement(childElem1);
					childElem1=0;
				}
			}
			if (childElem1)
			{
				CVIXMLDiscardElement(childElem1);
				childElem1=0;
			}
			if (childElem)
			{
				CVIXMLDiscardElement(childElem);
				childElem=0;
			}
		}
		error = CVIXMLSaveDocument(doc,1,glbPathToSerialConfigFile);
		if (error == 0)
		{
			sprintf(msg,"%s is saved successfully.",glbPathToSerialConfigFile);
			MessagePopup("Message",msg);
		}
		else
		{
			sprintf(msg,"Unable to save xml file %s",glbPathToSerialConfigFile);
			MessagePopup("Error",msg);
			return(-1);
		}
		// release and free xml memory
		if (curElem)
		{
			CVIXMLDiscardElement(curElem);
		}
		if (doc)
		{
			CVIXMLDiscardDocument(doc);
		}
		ReadSerialConfigurationFile(glbPathToSerialConfigFile);
		//CreateSerialDebugPanel(MainPanelHandlebk);
	}

	return 0;
}

/***************************************************************************//*!
* \brief Serial config table cb function for toggling between input and output
*******************************************************************************/
int CVICALLBACK  SerialTableCBFunction (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT && eventData2==6)
	{
		char CellVal[MAXCHARARRAYLENGTH];
		GetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),CellVal);
		if(!stricmp(CellVal,"2"))
			SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),"1");
		if(!stricmp(CellVal,"1"))
			SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),"2");
	}
	if(event == EVENT_COMMIT && ((eventData2==7) || (eventData2==8)))
	{
		char CellVal[MAXCHARARRAYLENGTH];
		GetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),CellVal);
		if(!stricmp(CellVal,"On"))
			SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),"Off");
		if(!stricmp(CellVal,"Off"))
			SetTableCellVal(glbSerialConfigurationPanelHandle, glbSerialConfigTableHandle,MakePoint(eventData2,eventData1),"On");
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Quit/hide on Serial Debug panel
*******************************************************************************/
int CVICALLBACK  QuitSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		glbSerialThread=0;
		HidePanel(glbSerialDebugPanelHandle);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Write Serial port button on Serial Debug panel
*******************************************************************************/
int CVICALLBACK  WriteSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		char errmsg[ERRLEN] = {0};
		int index=0;
		glbSerialThread=0;
		DelayWithEventProcessing(0.1);
		char deviceName[256]= {0};
		char data[1000]= {0};
		GetCtrlVal(glbSerialDebugPanelHandle,glbSerialRingDebugMenuHandle,&index);
		GetLabelFromIndex (glbSerialDebugPanelHandle, glbSerialRingDebugMenuHandle,index, deviceName);
		GetCtrlVal (glbSerialDebugPanelHandle, glbWriteBoxHandle, data);
		HexToCharInString(data);
		int i = getFileInfoIndexFromName(deviceName);
		if(glbSerialFileInfo[i].PortOpen==1)
		{
			WriteSerialDevice(deviceName, data, errmsg);
		}
		ReadSerialDebugCB (0, 0, EVENT_COMMIT, 0, 0, 0);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Read Serial port button on Serial Debug Panel
*******************************************************************************/
int CVICALLBACK  ReadSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		glbSerialThreadID=0;
		glbSerialThread=1;
		CmtScheduleThreadPoolFunction(glbSerialReadThreadHandle,SerialReadThread,0,&glbSerialThreadID);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Read Serial port button on Serial Debug Panel
*******************************************************************************/
int CVICALLBACK  ClearSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		DeleteTextBoxLines (panel, glbReadBoxHandle,0,-1);
	}
	return 0;
}

/***************************************************************************//*!
* \brief Callback for Init Serial port button on Serial Debug Panel
*
*
*******************************************************************************/
int CVICALLBACK  InitSerialDebugCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT )
	{
		char errmsg[ERRLEN] = {0};
		int index = 0;
		char deviceName[256]= {0};
		GetCtrlVal(glbSerialDebugPanelHandle,glbSerialRingDebugMenuHandle,&index);
		GetLabelFromIndex (glbSerialDebugPanelHandle, glbSerialRingDebugMenuHandle,index, deviceName);
		InitSerialDevice(deviceName, errmsg);
	}
	return 0;
}

//! \cond
/// REGION END

//! \cond
/// REGION END
//! \endcond