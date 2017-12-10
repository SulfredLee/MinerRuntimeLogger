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

std::vector<std::wstring> GetFileNamesFromPath(const std::wstring& strFolder,
	const std::wstring& strNameFormate)
{
	std::vector<std::wstring> result;
	std::wstring strSearchPath;
	strSearchPath = strFolder + _T("\\") + strNameFormate;
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				result.push_back(strFolder + _T("\\") + fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return result;
}

std::string GetCurrentTimeStamp()
{
	std::stringstream ss;

	SYSTEMTIME time; // UTC time
	//GetSystemTime(&time);
	GetLocalTime(&time);

	ss << std::setw(4) << std::setfill('0') << time.wYear << "/";
	ss << std::setw(2) << std::setfill('0') << time.wMonth << "/";
	ss << std::setw(2) << std::setfill('0') << time.wDay;
	ss << " ";
	ss << std::setw(2) << std::setfill('0') << time.wHour << ":";
	ss << std::setw(2) << std::setfill('0') << time.wMinute << ":";
	ss << std::setw(2) << std::setfill('0') << time.wSecond;

	return ss.str();
}

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

	Parser.PrintResult(_T(".\\RunTimeResult.txt"));
    return nRetCode;
}
