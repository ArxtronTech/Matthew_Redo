/*
	This library adds Analog IO control functions
*/

//int AIOHandle=0;

// Initializes the library to connect to the hardware based on the specified configuration file
int Initialize_AIO_Lib (const char * const ConfigPath);
// Gets the current signal value
void AIO_Read (const char * const SignalName, double *Value);
// Gets the average of signals in multiple channels
void AIO_GetAvg (const double * const ValueArray, const unsigned int NumChannels, const unsigned int NumValsPerChannel, double *AvgArray);
// Resets the library connection to hardware
void AIO_Exit (void);