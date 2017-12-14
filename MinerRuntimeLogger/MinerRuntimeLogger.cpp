// MinerRuntimeLogger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MinerRuntimeLogger.h"
#include "LogParser.h"

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;



int main()
{
    int nRetCode = 0;

	//GetFileNames(_T("C:\\Users\\Sulfred\\Documents\\software_dev\\MinerRuntimeLogger"), _T("*.txt"));
	auto LogFiles = GetFileNamesFromPath(_T("."), _T("*.txt"));

	if (LogFiles.size() == 0)
	{
		std::cout << GetCurrentTimeStamp() << "Info: No log file found" << std::endl;
		return nRetCode;
	}

	CLogParser Parser;
	for (auto it : LogFiles)
	{
		Parser.ProcessLog(it);
	}
	Parser.ReassignPeriodRange(_T(".\\TimeRange.ini"));
	Parser.PrintResult(_T(".\\RunTimeResult.txt"));
    return nRetCode;
}
