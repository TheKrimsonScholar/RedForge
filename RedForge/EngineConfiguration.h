#pragma once

#include <filesystem>

struct EngineConfiguration
{
	static EngineConfiguration LoadFromFile();
};
struct GameConfiguration
{
	std::filesystem::path startupLevelPath;

	static GameConfiguration LoadFromFile();
};