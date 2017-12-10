#include "stdafx.h"
#include "LogParser.h"


CLogParser::CLogParser()
{
	vecPeriods.clear();
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
			if (vecPeriods.size() == 0)
			{
				vecPeriods.emplace_back(PeriodRange(CurTime));
			}
			else
			{
				CTime ExtendedTime = vecPeriods.back().End + CTimeSpan(0, 0, 3, 0); // 3 minutes buffer
				if (ExtendedTime < CurTime)
				{
					vecPeriods.emplace_back(PeriodRange(CurTime)); // add a new time period
				}
				else
				{
					vecPeriods.back().End = CurTime; // extend current time period
				}
			}
		}
	}
	FH.close();
}

void CLogParser::PrintResult(const std::wstring & strResultFile)
{
	std::wofstream FH(strResultFile, std::ios::out);

	for (auto it : vecPeriods)
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

	std::string strTimeStamp = strIN.substr(StartPos, Length);

	int nYear = std::stoi(strTimeStamp.substr(6, 2)) + 2000;
	int nMonth = std::stoi(strTimeStamp.substr(0, 2));
	int nDay = std::stoi(strTimeStamp.substr(3, 2));
	int nHour = std::stoi(strTimeStamp.substr(9, 2));
	int nMin = std::stoi(strTimeStamp.substr(12, 2));
	int nSec = 0;
	CTime CurTime(nYear, nMonth, nDay, nHour, nMin, nSec);

	return CurTime;
}
