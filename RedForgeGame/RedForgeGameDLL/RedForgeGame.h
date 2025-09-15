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

//class MyGameModule : public IGameModule
//{
//public:
//    MyGameModule() {}
//    ~MyGameModule() {}
//    virtual void RegisterComponents() override {}
//    virtual void Initialize() override {}
//    virtual void Shutdown() override {}
//};
//
//REDFORGEGAME_API IGameModule* CreateGameModule();
//IGameModule* CreateGameModule()
//{
//    return new MyGameModule();
//}