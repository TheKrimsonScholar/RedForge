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
REDFORGE_API std::filesystem::path GetGameConfigPath();
REDFORGE_API std::filesystem::path GetGameAssetsPath();

void SetGamePath(const std::filesystem::path& gamePath);

struct SerializedObject
{
	std::string typeName;
	std::unordered_map<std::string, std::string> parameters;
	std::vector<SerializedObject> children;
};

class File
{
public:
	REDFORGE_API static void SaveObject(std::ostream& os, const SerializedObject& object, uint32_t tabDepth = 0);
	REDFORGE_API static SerializedObject LoadObject(std::istream& is);

	static std::vector<char> ReadFile(const std::filesystem::path& filePath);

	// Returns all relative file paths at all levels below the specified directory, optionally limited to specific extensions.
	REDFORGE_API static std::vector<std::filesystem::path> GetAllFilesInDirectory(const std::filesystem::path& directory, const std::vector<std::wstring>& extensions = {});
	// Returns the names of all top-level items (files and folders) directly under the specified directory.
	REDFORGE_API static std::vector<std::filesystem::path> GetAllTopLevelItemsInDirectory(const std::filesystem::path& directory);

	template<typename... Ts>
	static char* Serialize(Ts... objs, size_t& out_size);
	template<typename T>
	static char* Serialize(const T& obj, size_t& out_size);

	template<typename T>
	static size_t GetSerializedDataSize(const T& obj);

	template<typename... Ts>
	static void Serialize(const Ts&... objs, std::stringstream& str);
	template<typename T>
	static void Serialize(const T& obj, std::stringstream& str);
	template<typename T>
	static T Deserialize(std::stringstream& str);
};

template<typename... Ts>
inline char* File::Serialize(Ts... objs, size_t& out_size)
{
	const size_t totalBufferSize = (GetSerializedDataSize(objs) + ...);
	char* buffer = new char[totalBufferSize];

	size_t offset = 0;
	//std::apply(
		//[](Ts... objs)
		//{
			([buffer, &offset](auto obj)
				{
					//buffer[offset] = Serialize(obj);
					size_t bufferSize;
					const void* serializedObj = Serialize(obj, bufferSize);
					memcpy(&buffer[offset], serializedObj, bufferSize);
					offset += bufferSize;
				}(std::forward<Ts>(objs)), ...);
		//}, objs);

	return buffer;
}

template<>
inline char* File::Serialize<std::string>(const std::string& obj, size_t& out_size)
{
	out_size = GetSerializedDataSize(obj);

	char* buffer = new char[out_size];
	size_t stringLength = obj.size();
	memcpy(buffer, &stringLength, sizeof(size_t));
	memcpy(buffer + sizeof(size_t), obj.c_str(), obj.size());
	return buffer;
}
//template<>
//inline size_t File::GetSerializedDataSize<std::string>(const std::string& obj)
//{
//	// String size + string data
//	return sizeof(size_t) + sizeof(char) * obj.size();
//}

template<typename... Ts>
inline void File::Serialize(const Ts&... objs, std::stringstream& str)
{
	(Serialize(objs, str), ...);
}
template<>
inline void File::Serialize<std::string>(const std::string& obj, std::stringstream& str)
{
	str << obj.size() << ' ' << obj;

	//size_t stringLength = obj.size();
	//str.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
	//str.write(obj.c_str(), obj.size());
}