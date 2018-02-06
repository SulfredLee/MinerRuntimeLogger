#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

std::vector<std::wstring> GetFileNamesFromPath(const std::wstring& strFolder,
	const std::wstring& strNameFormate);
std::string GetCurrentTimeStamp();
void Tokenize(std::string str, std::vector<std::string> &vToken, char delimiter);

class CLogParser
{
public:
	CLogParser();
	~CLogParser();

	void ProcessLog(const std::wstring& strLogFile);
	void ReassignPeriodRange(const std::wstring& strRangeFile);
	void PrintResult(const std::wstring& strResultFile);
private:
	CTime GetTimeStamp(const std::string& strIN);
	CTime GetTimeStamp_FromPeriodINI(const std::string& strIN);
	bool IsBetween(const CTime& ctStart, const CTime& ctEnd, const CTime& ctTarget);
private:
	struct PeriodRange
	{
	public:
		PeriodRange(const CTime& TimeIN)
		: Start(TimeIN)
		, End(TimeIN) {}
		PeriodRange(const CTime& StartTimeIN, const CTime& EndTimeIN)
			: Start(StartTimeIN)
			, End(EndTimeIN) {}
		~PeriodRange() {}
	public:
		CTime Start;
		CTime End;
	};
	std::vector<PeriodRange> m_vecPeriods;
};

