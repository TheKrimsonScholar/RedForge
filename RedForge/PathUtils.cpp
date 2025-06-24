#include "PathUtils.h"

#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

std::string GetExePath()
{
    std::filesystem::path exePath;

#ifdef _WIN32
    // Windows-specific: Get the path to the executable
    char buffer[1024];
    DWORD length = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    if (length > 0 && length < sizeof(buffer)) {
        exePath = buffer;
    }
    else {
        // Fallback or error handling
        exePath = std::filesystem::current_path();
    }
#else
    // Linux/Unix-specific: Read /proc/self/exe or use argv[0]
    // A common way on Linux is to read the symbolic link /proc/self/exe
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        exePath = buffer;
    }
    else {
        // Fallback: If /proc/self/exe fails, assume current working directory
        // or rely on argv[0] (which would need to be passed into this function)
        exePath = std::filesystem::current_path();
    }
#endif

    // Get the parent path (directory containing the executable)
    return exePath.parent_path().string();
}

std::string FixPath(const std::string& relativeFilePath)
{
	std::filesystem::path exePath = GetExePath();
	std::filesystem::path relPath = relativeFilePath;

    return (exePath/relativeFilePath).string();
}
std::wstring FixPath(const std::wstring& relativeFilePath)
{
    std::filesystem::path exePath = NarrowToWide(GetExePath());
    std::filesystem::path relPath = relativeFilePath;

    return (exePath/relativeFilePath).wstring();
}
std::string WideToNarrow(const std::wstring& str)
{
    // Similar caveats as NarrowToWide
    return std::string(str.begin(), str.end());
}
std::wstring NarrowToWide(const std::string& str)
{
    // This is a basic conversion and might not handle all encodings correctly
    // It's generally better to work with UTF-8 std::string paths
    // or use a more robust library for encoding conversions.
    return std::wstring(str.begin(), str.end());
}