#pragma once

#include <filesystem>

struct EditorConfiguration
{
	std::filesystem::path startupLevelPath;

	static EditorConfiguration LoadFromFile();
};