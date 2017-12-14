#include "stdafx.h"
#include "LogParser.h"

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

void Tokenize(std::string str, std::vector<std::string> &vToken, char delimiter)
{
	size_t start = str.find_first_not_of(delimiter), end = start;
	while (start != std::string::npos)
	{
		end = str.find(delimiter, start);
		vToken.push_back(str.substr(start, end - start));
		start = str.find_first_not_of(delimiter, end);
	}
}

CLogParser::CLogParser()
{
	m_vecPeriods.clear();
}


CLogParser::~CLogParser()
{
}

void CLogParser::ProcessLog(const std::wstring & strLogFile)
{
	std::ifstream FH(strLogFile);
	std::string strLine;
	while (std::getline(FH, strLine))
	{
		if (strLine.find("New job from ") != std::string::npos)
		{
			CTime CurTime = GetTimeStamp(strLine);
			if (m_vecPeriods.size() == 0)
			{
				m_vecPeriods.emplace_back(PeriodRange(CurTime));
			}
			else
			{
				CTime ExtendedTime = m_vecPeriods.back().End + CTimeSpan(0, 0, 30, 0); // 30 minutes buffer
				if (ExtendedTime < CurTime)
				{
					m_vecPeriods.emplace_back(PeriodRange(CurTime)); // add a new time period
				}
				else
				{
					m_vecPeriods.back().End = CurTime; // extend current time period
				}
			}
		}
	}
	FH.close();
}

void CLogParser::ReassignPeriodRange(const std::wstring & strRangeFile)
{
	std::ifstream FH(strRangeFile);

	if (!FH.is_open())
	{
		std::wcout << "Cannot open file " << strRangeFile << std::endl;
		exit(0);
	}

	std::vector<PeriodRange> vecPeriods;

	std::string line;
	while (std::getline(FH, line))
	{
		std::vector<std::string> vecTimes;
		Tokenize(line, vecTimes, ','); // get start time and end time

		CTime ctStartTime = GetTimeStamp_FromPeriodINI(vecTimes.front());
		CTime ctEndTime = GetTimeStamp_FromPeriodINI(vecTimes.back());

		for (auto it : m_vecPeriods)
		{
			if (IsBetween(ctStartTime, ctEndTime, it.Start)
				&& IsBetween(ctStartTime, ctEndTime, it.End))
			{
				vecPeriods.push_back(it);
			}
			else if (IsBetween(ctStartTime, ctEndTime, it.Start)
				&& !IsBetween(ctStartTime, ctEndTime, it.End))
			{
				vecPeriods.emplace_back(PeriodRange(it.Start, ctEndTime));
				vecPeriods.emplace_back(PeriodRange(ctEndTime, it.End));
			}
			else if (!IsBetween(ctStartTime, ctEndTime, it.Start)
				&& IsBetween(ctStartTime, ctEndTime, it.End))
			{
				vecPeriods.emplace_back(PeriodRange(it.Start, ctStartTime));
				vecPeriods.emplace_back(PeriodRange(ctStartTime, ctEndTime));
			}
			else
			{
				vecPeriods.push_back(it);
			}
		}
		m_vecPeriods = vecPeriods;
		vecPeriods.clear();
	}

	FH.close();
}

void CLogParser::PrintResult(const std::wstring & strResultFile)
{
	std::wofstream FH(strResultFile, std::ios::out);

	for (auto it : m_vecPeriods)
	{
		std::wstring strStartTime(it.Start.Format(_T("%Y/%m/%d %H:%M:%S")).GetString());
		std::wstring strEndTime(it.End.Format(_T("%Y/%m/%d %H:%M:%S")).GetString());
		FH << strStartTime << "\t" << strEndTime << std::endl;
	}

	FH.close();
}

CTime CLogParser::GetTimeStamp(const std::string & strIN)
{
	std::string strStart = "ETH: ";
	std::string strEnd = " - New job from ";
	size_t StartPos = strIN.find(strStart) + strStart.length();
	size_t EndPos = strIN.find(strEnd);
	size_t Length = EndPos - StartPos;

	std::string strTimeStamp = strIN.substr(StartPos, Length); // example: MM/DD/YY-HH:mm:SS

	int nYear = std::stoi(strTimeStamp.substr(6, 2)) + 2000;
	int nMonth = std::stoi(strTimeStamp.substr(0, 2));
	int nDay = std::stoi(strTimeStamp.substr(3, 2));
	int nHour = std::stoi(strTimeStamp.substr(9, 2));
	int nMin = std::stoi(strTimeStamp.substr(12, 2));
	int nSec = 0;
	CTime CurTime(nYear, nMonth, nDay, nHour, nMin, nSec);

	return CurTime;
}

CTime CLogParser::GetTimeStamp_FromPeriodINI(const std::string & strIN)
{
	int nYear = std::stoi(strIN.substr(0, 4)); // example: YYYY/MM/DD
	int nMonth = std::stoi(strIN.substr(5, 2));
	int nDay = std::stoi(strIN.substr(8, 2));
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;
	CTime CurTime(nYear, nMonth, nDay, nHour, nMin, nSec);

	return CurTime;
}

bool CLogParser::IsBetween(const CTime & ctStart, const CTime & ctEnd, const CTime & ctTarget)
{
	return ctStart <= ctTarget && ctTarget <= ctEnd ? true : false;
}
