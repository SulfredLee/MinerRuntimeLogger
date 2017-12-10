#pragma once

#include "resource.h"

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

std::vector<std::wstring> GetFileNamesFromPath(const std::wstring& strFolder,
	const std::wstring& strNameFormate);
std::string GetCurrentTimeStamp();