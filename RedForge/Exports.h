#pragma once

//#ifdef _WIN32
	//#define _CRTDBG_MAP_ALLOC
	//#include <crtdbg.h>
//#endif

#include <chrono>

#ifdef _WIN32
	#ifdef REDFORGE_EXPORTS
		#define REDFORGE_API __declspec(dllexport)
	#else
		#define REDFORGE_API __declspec(dllimport)
	#endif
#else
	#define REDFORGE_API
#endif

REDFORGE_API int Get();

REDFORGE_API void Run();