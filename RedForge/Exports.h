#pragma once

#ifdef REDFORGE_EXPORTS
#define REDFORGE_API __declspec(dllexport)
#else
#define REDFORGE_API __declspec(dllimport)
#endif

REDFORGE_API int Get();

REDFORGE_API void Run();