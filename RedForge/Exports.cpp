#include "Exports.h"

#include "Engine.h"

bool LoadGameLibrary(const std::filesystem::path& gamePath)
{
    std::wstring gamePathString = gamePath.wstring();
    std::replace(gamePathString.begin(), gamePathString.end(), '\\', '/'); // Replace back-slashes with forward-slashes

    // Trim any trailing slashes in the path
    while(gamePathString.ends_with(L'/'))
        gamePathString = gamePathString.substr(0, gamePathString.find_last_of(L'/'));
    // The name of the project is determined by the name of the specified folder.
    std::wstring gameName = gamePathString.substr(gamePathString.find_last_of(L'/') + 1);
    
    SetGamePath(std::filesystem::path(gamePathString));

    /* Construct the path to the game DLL, based on configuration and platform */

    std::wstring gameDLLPathString = gamePathString;

    // Platform (just 64-bit)
    #ifdef _M_X64
    gameDLLPathString += L"/x64";
    #endif

    // Configuration
    #ifdef _DEBUG
    gameDLLPathString += L"/Debug";
    #else
    gameDLLPathString += L"/Release";
    #endif

    // Network configuration
    #ifdef NETMODE_CLIENT
    gameDLLPathString += L" Client/" + gameName + L"DLLClient.dll";
    #elif NETMODE_SERVER
    gameDLLPathString += L" Server/" + gameName + L"DLLServer.dll";
    #else
    gameDLLPathString += L"/" + gameName + L"DLL.dll";
    #endif

    std::filesystem::path gameDLLPath = std::filesystem::path(gameDLLPathString);

    std::cout << "Game Path: " << GetGamePath() << std::endl;
    std::cout << "Game DLL Path: " << gameDLLPath << std::endl;

    // Load the DLL using the provided path
    HMODULE handle = LoadLibraryA(gameDLLPath.string().c_str());
    if(handle == NULL)
    {
        std::cerr << "Failed to load DLL at path: " << gameDLLPath << std::endl;
        return false;
    }

    return true;
}
void Run()
{
    #ifdef _WIN32
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
    
    Engine* engine = new Engine();
    engine->Run();
    
    delete engine;
}