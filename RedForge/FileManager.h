#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Exports.h"

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
};