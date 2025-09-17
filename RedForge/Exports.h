#pragma once

//#ifdef _WIN32
	//#define _CRTDBG_MAP_ALLOC
	//#include <crtdbg.h>
//#endif

#include <chrono>
#include <filesystem>

#ifdef _WIN32
	#ifdef REDFORGE_EXPORTS
		#define REDFORGE_API __declspec(dllexport)
	#else
		#define REDFORGE_API __declspec(dllimport)
	#endif
#else
	#define REDFORGE_API
#endif

REDFORGE_API bool LoadGameLibrary(const std::filesystem::path& gamePath);
REDFORGE_API void Run();