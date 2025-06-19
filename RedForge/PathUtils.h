#pragma once

#include <string>

#include "Exports.h"

REDFORGE_API std::string GetExePath();
REDFORGE_API std::string FixPath(const std::string& relativeFilePath);
REDFORGE_API std::wstring FixPath(const std::wstring& relativeFilePath);
REDFORGE_API std::string WideToNarrow(const std::wstring& str);
REDFORGE_API std::wstring NarrowToWide(const std::string& str);