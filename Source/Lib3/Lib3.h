/*
	This library creates log files
*/

#include "Lib2.h"

// Initializes the log file
int Initialize_Log_Lib (const char * const LogPath);
// Backs up current log and create a new log file
void Log_ArchiveAndCreateNew (void);
// Writes a new log entry
void Log_WriteEntry (const char * const TestName, const int TestNum, const char * const TestVal, const char * const TestLoLim, const char * const TestHiLim);