#pragma once

#ifdef _WIN32
	#ifdef REDFORGEGAMEDLL_EXPORTS
		#define REDFORGEGAME_API __declspec(dllexport)
	#else
		#define REDFORGEGAME_API __declspec(dllimport)
	#endif
#else
	#define REDFORGEGAME_API
#endif

#include <string>

#include "GameModule.h"

REDFORGEGAME_API const std::string GAME_NAME = "RedForgeGame";