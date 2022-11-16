/***************************************************************************//*!
* \file TestSteps.h
* \author Biye Chen
* \copyright Arxtron Technologies Inc.. All Rights Reserved.
* \date 11/10/2022 11:34:01 PM
*******************************************************************************/

#ifndef __TESTSTEPS_H__
#define __TESTSTEPS_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "Lib1.h"
#include "tsutil.h"
#include <ansi_c.h>
#include "Lib2.h"
#include <utility.h>
#include "Lib3.h"
#include "cvidef.h"

#include "ArxtronToolslib.h"

//==============================================================================
// Constants

//==============================================================================
// Types


//==============================================================================
// External variables

double TestTimer;

//==============================================================================
// Global functions

int TestStep_Step1 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo);
int TestStep_Step2 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo);
int TestStep_Step3 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo);
int TestStep_Step4 (int NestNum, char *ReportText, tsErrorDataType *ErrInfo);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __TestSteps_H__ */
