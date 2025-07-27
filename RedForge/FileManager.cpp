#include "FileManager.h"

#include <fstream>
#include <stack>

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