#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "Exports.h"

#define ENGINE_ASSET_EXTENSION L".rfasset"

REDFORGE_API std::filesystem::path GetEnginePath();
REDFORGE_API std::filesystem::path GetEngineAssetsPath();
REDFORGE_API std::filesystem::path GetEditorAssetsPath();
REDFORGE_API std::filesystem::path GetGamePath();
REDFORGE_API std::filesystem::path GetGameAssetsPath();

void SetGamePath(const std::filesystem::path& gamePath);

struct SerializedObject
{
	std::string typeName;
	std::unordered_map<std::string, std::string> parameters;
	std::vector<SerializedObject> children;
};

class FileManager
{
private:
	static inline FileManager* Instance;

public:
	FileManager() {};
	~FileManager() {};

	void Startup();
	void Shutdown();

	static void SaveObject(std::ostream& os, const SerializedObject& object, uint32_t tabDepth = 0);
	static SerializedObject LoadObject(std::istream& is);

	static std::vector<char> ReadFile(const std::filesystem::path& filePath);

	static std::unordered_map<std::wstring, std::wstring> GetAllFilesInDirectory(const std::filesystem::path& directory, std::vector<std::wstring> extensions);
};