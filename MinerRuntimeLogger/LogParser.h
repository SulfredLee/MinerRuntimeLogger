#pragma once
#include <string>
#include <vector>
#include <fstream>

class CLogParser
{
public:
	CLogParser();
	~CLogParser();

	void ProcessLog(const std::wstring& strLogFile);
	void PrintResult(const std::wstring& strResultFile);
private:
	CTime GetTimeStamp(const std::string& strIN);
private:
	struct PeriodRange
	{
	public:
		PeriodRange(const CTime& TimeIN)
		: Start(TimeIN)
		, End(TimeIN) {}
		~PeriodRange() {}
	public:
		CTime Start;
		CTime End;
	};
	std::vector<PeriodRange> vecPeriods;
};

