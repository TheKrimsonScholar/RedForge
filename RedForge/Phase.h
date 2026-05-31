#pragma once

#include <vector>
#include <typeindex>

#include "System.h"
#include "SystemRegistrationMacros.h"

struct Phase
{
private:
	std::vector<CommandBuffer*> commandBuffers;
	std::vector<SystemBase*> systems;
	std::vector<SystemContextBase*> systemContexts;

	std::vector<std::type_index> writeList;
	std::vector<std::type_index> readList;

public:
	Phase();
	~Phase();

	void AddSystem(World* world, std::type_index systemType, SystemBase* system);
	
	void ClearSystems();

	size_t GetSystemCount() const { return systems.size(); };

	SystemBase* GetSystem(size_t systemIndex) const { return systems[systemIndex]; };
	const CommandBuffer* GetCommandBuffer(size_t systemIndex) const { return commandBuffers[systemIndex]; };
	SystemContextBase* GetSystemContext(size_t systemIndex) const { return systemContexts[systemIndex]; };

	std::vector<std::type_index> GetWriteList() const { return writeList; };
	std::vector<std::type_index> GetReadList() const { return readList; };
};