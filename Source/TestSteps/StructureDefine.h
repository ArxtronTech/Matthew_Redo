#ifndef __STRUCTUREDEFINE_H__
#define __STRUCTUREDEFINE_H__
//Log parse structure
typedef struct {
	char TestName[256];
	int TestNum;
	char TestVal[256];
	char TestLoLim[256];
	char TestHiLim[256];
} LogStruct;
#endif