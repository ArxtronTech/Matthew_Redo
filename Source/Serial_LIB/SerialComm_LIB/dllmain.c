//==============================================================================
//
// Title:		dllmain.c
// Purpose:		A short description of the implementation.
//
// Created on:	6/7/2020 at 4:35:52 PM by Biye Chen.
// Copyright:	Arxtron Technologies Inc.. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <utility.h>
#include "cvidef.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			if (InitCVIRTE (hinstDLL, 0, 0) == 0)
				return 0;	  /* out of memory */
			break;
		case DLL_PROCESS_DETACH:
			CloseCVIRTE ();
			break;
	}
	
	return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	/* Included for compatibility with Borland */

	return DllMain (hinstDLL, fdwReason, lpvReserved);
}

/***************************************************************************//*!
* \brief Get error message for standard error codes
*******************************************************************************/
void GetStandardErrMsg (int error, char errmsg[ERRLEN])
{
	memset (errmsg,0,ERRLEN);
	
	switch (error)
	{
		case ERR_LIB_NOT_INITIALIZED:
			strcpy (errmsg,"Serial_LIB library not initialized");
			break;
	}
}
