// NetTest.cpp : Defines the entry point for the console application.
//
#ifdef _MEM_CHECK
#include "vld.h"
#ifdef _M_X64 
#pragma comment(lib, "vld64.lib")
#else
#pragma comment(lib, "vld32.lib")
#endif
#endif

#ifdef _BUG_TRAP
#include <windows.h>
#include "BugTrap.h"
#pragma comment (lib, "BugTrap-x64.lib")

void SetupCrashCollector()
{
	BT_SetAppName("VChatApp");
	BT_SetAppVersion("1.0.2");
	BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT);
	BT_SetActivityType(BTA_SAVEREPORT);
	BT_SetReportFilePath("crashlogs\\");
	BT_InstallSehFilter();
}
#endif

void ChatStart(char *pPath);

int main(int argc, char* argv[])
{
#ifdef _BUG_TRAP
	SetupCrashCollector();
#endif

	ChatStart(argv[0]);
	return 0;
}
