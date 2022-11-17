void TestStep_Step5 (void);
#include <advanlys.h>
#include <ansi_c.h>
#include "NonTestStepFuc.h"
int main (int argc, char *argv[])
{
	TestStep_Step5();
	char item[100] = {'\0'};
	ConvertIntToString(item);
	printf("%s",item);
	int a = fgets(item,10,stdin);
	return 0;
}