#include "FileManager.h"

#include <fstream>
#include <stack>

static std::filesystem::path activeGamePath;

std::filesystem::path GetEnginePath()
{
    static std::filesystem::path enginePath(ENGINE_PATH);

    return enginePath;
}
std::filesystem::path GetEngineAssetsPath()
{
    return GetEnginePath().append(L"Assets/Engine/");
}
std::filesystem::path GetEditorAssetsPath()
{
    return GetEnginePath().append(L"Assets/Editor/");
}
std::filesystem::path GetGamePath()
{
    return activeGamePath;
}
std::filesystem::path GetGameAssetsPath()
{
    return GetGamePath().append(L"Assets/");
}

void SetGamePath(const std::filesystem::path& gamePath)
{
    activeGamePath = gamePath;
}

void FileManager::Startup()
{
	Instance = this;
}
void FileManager::Shutdown()
{

}

void FileManager::SaveObject(std::ostream& os, const SerializedObject& object, uint32_t tabDepth)
{
	for(uint32_t i = 0; i < tabDepth; i++)
        os << '\t';
    os << '$' << object.typeName << std::endl;

    for(const std::pair<std::string, std::string>& keyValue : object.parameters)
    {
        for(uint32_t i = 0; i < tabDepth + 1; i++)
            os << '\t';
        os << keyValue.first << "=" << keyValue.second << std::endl;
    }
    for(const SerializedObject& child : object.children)
        SaveObject(os, child, tabDepth + 1);

    for(uint32_t i = 0; i < tabDepth; i++)
        os << '\t';
    os << "$$" << std::endl;
}
SerializedObject FileManager::LoadObject(std::istream& is)
{
	static const std::string IGNORED_CHARS = " \t";

    SerializedObject root;

    std::stack<SerializedObject*> objectsStack;
    objectsStack.push(&root);
    while(is.good())
    {
        std::string line;
        std::getline(is, line);

        size_t firstValidCharPosition = line.find_first_not_of(IGNORED_CHARS);
        size_t lastValidCharPosition = line.find_last_not_of(IGNORED_CHARS);

        // If there are no valid characters, skip this line
        if(firstValidCharPosition == std::string::npos)
            continue;

        // Trim whitespace on the left and right
        line = line.substr(firstValidCharPosition, lastValidCharPosition - (firstValidCharPosition - 1));

        // If the first valid character is $, it's starting or finishing an object
        if(line[0] == '$')
        {
            // If next char is also $, we're ending an object
            if(line[1] == '$')
            {
                // Move down the stack to the most recent parent and set it as the new current object if it exists
                objectsStack.pop();
            }
            // Otherwise we start a new object with the given type name
            else
            {
                size_t nextInvalidCharPosition = line.find_first_of(IGNORED_CHARS);
                std::string typeName = line.substr(1, nextInvalidCharPosition);

                // Add a new object as a child and add it to the stack
                objectsStack.top()->children.emplace_back();
                objectsStack.push(&objectsStack.top()->children.back());
                objectsStack.top()->typeName = typeName;
            }
        }
        // Otherwise, read key-value pair for current object
        else
        {
            size_t centerCharPosition = line.find('=');
            if(centerCharPosition != std::string::npos)
            {
                std::string key = line.substr(0, centerCharPosition);
                std::string value = line.substr(centerCharPosition + 1);
            
                if(objectsStack.top())
                    objectsStack.top()->parameters.emplace(key, value);
            }
        }
    }

    // If there is only one child, the root is redundant; return the child's hierarchy
    // Otherwise, return the entire hierarchy
    // This is done because objects are intended to be wrapped entirely in a single root, but empty files and files with multiple roots are still supported.
    return root.children.size() == 1 ? root.children[0] : root;
}

std::vector<char> FileManager::ReadFile(const std::filesystem::path& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if(!file.is_open())
        throw std::runtime_error("Failed to open file! \"" + filePath.string() + "\"");

    // Determine size of the file to allocate a buffer
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // Seek back to the beginning and read all
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

std::unordered_map<std::wstring, std::wstring> FileManager::GetAllFilesInDirectory(const std::filesystem::path& directory, std::vector<std::wstring> extensions)
{
    std::unordered_map<std::wstring, std::wstring> identifiers;

    for(const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if(std::filesystem::is_regular_file(entry))
        {
            std::wstring filePath = entry.path().wstring();
            size_t directoryEnd = filePath.find(directory.wstring()) + directory.wstring().length();
            size_t extensionStart = filePath.find_last_of('.');

            std::wstring localPath = filePath.substr(directoryEnd);
            std::wstring identifier = filePath.substr(directoryEnd, extensionStart - directoryEnd);
            std::wstring extension = filePath.substr(extensionStart);

            if(std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                identifiers.emplace(localPath, identifier);
        }
    }

    return identifiers;
}