#pragma once

class IGameModule
{
public:
    virtual ~IGameModule() = default;
    virtual void RegisterComponents() = 0;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
};

// type of the factory function every game DLL must export
using CreateGameModuleFunc = IGameModule* (*)();