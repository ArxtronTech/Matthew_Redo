#include <ansi_c.h>
#include "Serial_LIB.h"


// -------------- START GETTER FUNCTIONS --------------
int GetStatus_ESR ();
int GetStatus_SCPI ();
int GetStatus_PROT ();
int GetStatus_ERRs ( char *errMsg);
int GetStatus_OUT ();
double GetStatus_TRIP ();
double GetVoltage ();
double GetCurr ();


// -------------- END GETTER FUNCTIONS --------------


// -------------- START SETTER FUNCTIONS --------------

int SetVolt ( double volts);
int SetLimit_Curr ( double current);
int SetFold ( int type);
int SetPolarity ( int pol);
int SetSense ( int sense);
int SetState ( int state);
int SetIsolation ( int iso);
int SetDelay ( double time);

// -------------- END SETTER FUNCTIONS --------------


int InitPSU ( double volts, double curr);
int SelfTest ();
int ClearPSUStatus (); 
int ResetPSU ();
int ReportErrors ();
void SetPSUName (char *name);
