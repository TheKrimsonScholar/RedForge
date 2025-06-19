#include "PathUtils.h"

#include <windows.h>

std::string GetExePath()
{
    // Assume the path is just the "current directory" for now
    std::string path = ".\\";

    // Get the real, full path to this executable
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    // Find the location of the last slash charaacter
    char* lastSlash = strrchr(currentDir, '\\');
    if (lastSlash)
    {
        // End the string at the last slash character, essentially
        // chopping off the exe's file name.  Remember, c-strings
        // are null-terminated, so putting a "zero" character in 
        // there simply denotes the end of the string.
        *lastSlash = 0;

        // Set the remainder as the path
        path = currentDir;
    }

    // Toss back whatever we've found
    return path;
}

std::string FixPath(const std::string& relativeFilePath)
{
    return GetExePath() + "\\" + relativeFilePath;
}
std::wstring FixPath(const std::wstring& relativeFilePath)
{
    return NarrowToWide(GetExePath()) + L"\\" + relativeFilePath;
}
std::string WideToNarrow(const std::wstring& str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0, 0, 0);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &result[0], size, 0, 0);
    return result;
}
std::wstring NarrowToWide(const std::string& str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}