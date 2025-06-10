#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <chrono>

#ifdef REDFORGE_EXPORTS
#define REDFORGE_API __declspec(dllexport)
#else
#define REDFORGE_API __declspec(dllimport)
#endif

REDFORGE_API int Get();

REDFORGE_API void Run();