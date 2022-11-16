/*
	This library adds Analog IO control functions
*/

// Initializes the library to connect to the hardware based on the specified configuration file
int Initialize_AIO_Lib (char *ConfigPath);
// Changes the specified signal to the specified value
void AIO_Write (char *SignalName, const double Value);
// Gets the current signal value
void AIO_Read (char *SignalName, double *Value);