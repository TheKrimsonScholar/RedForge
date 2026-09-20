#include "EngineConfiguration.h"

#include <cassert>
#include <fstream>

#include "FileManager.h"

#define ENGINECONFIG_FILENAME "EngineConfig.txt"
#define GAMECONFIG_FILENAME "GameConfig.txt"

#ifdef NETMODE_CLIENT
    #define ENGINECONFIG_FILENAME "ClientEngineConfig.txt"
    #define GAMECONFIG_FILENAME "ClientGameConfig.txt"
#endif
#ifdef NETMODE_SERVER
    #define ENGINECONFIG_FILENAME "ServerEngineConfig.txt"
    #define GAMECONFIG_FILENAME "ServerGameConfig.txt"
#endif

EngineConfiguration EngineConfiguration::LoadFromFile()
{
    std::ifstream engineConfigStream(GetGameConfigPath().append(ENGINECONFIG_FILENAME).c_str());
    assert(engineConfigStream.good() && "Engine configuration file does not exist.");
    const SerializedObject engineConfigObj = File::LoadObject(engineConfigStream);

    EngineConfiguration engineConfig = {};

    return engineConfig;
}

GameConfiguration GameConfiguration::LoadFromFile()
{
    std::ifstream gameConfigStream(GetGameConfigPath().append(GAMECONFIG_FILENAME).c_str());
    assert(gameConfigStream.good() && "Game configuration file does not exist.");
    const SerializedObject gameConfigObj = File::LoadObject(gameConfigStream);

    GameConfiguration gameConfig = {};
    gameConfig.startupLevelPath = std::filesystem::path(gameConfigObj.parameters.at("DefaultLevel"));

    return gameConfig;
}