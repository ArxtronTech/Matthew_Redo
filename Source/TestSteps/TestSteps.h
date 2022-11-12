/***************************************************************************//*!
* \file TestSteps.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/10/2022 11:34:01 PM
*******************************************************************************/

#ifndef __TestSteps_H__
#define __TestSteps_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "tsutil.h"
#include <ansi_c.h>
#include <utility.h>
#include "cvidef.h"
#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int TestStep (int NestNum, char *ReportText, tsErrorDataType *ErrInfo);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestSteps_H__ */
