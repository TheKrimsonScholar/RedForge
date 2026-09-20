#include "EditorConfiguration.h"

#include <cassert>
#include <fstream>

#include "FileManager.h"

#define EDITORCONFIG_FILENAME "EditorConfig.txt"

#ifdef NETMODE_CLIENT
    #define EDITORCONFIG_FILENAME "ClientEditorConfig.txt"
#endif
#ifdef NETMODE_SERVER
    #define EDITORCONFIG_FILENAME "ServerEditorConfig.txt"
#endif

EditorConfiguration EditorConfiguration::LoadFromFile()
{
    std::ifstream editorConfigStream(GetGameConfigPath().append(EDITORCONFIG_FILENAME).c_str());
    assert(editorConfigStream.good() && "Editor configuration file does not exist.");
    const SerializedObject editorConfigObj = File::LoadObject(editorConfigStream);

    EditorConfiguration editorConfig = {};
    editorConfig.startupLevelPath = std::filesystem::path(editorConfigObj.parameters.at("DefaultLevel"));

    return editorConfig;
}