#include "Phase.h"

Phase::Phase()
{

}
Phase::~Phase()
{
	
}

void Phase::AddSystem(World* world, std::type_index systemType, SystemBase* system)
{
	//CommandBuffer& commandBuffer = commandBuffers.emplace_back();
	CommandBuffer* commandBuffer = new CommandBuffer();
	commandBuffers.push_back(commandBuffer);
	systems.push_back(system);
	systemContexts.push_back(system->CreateContext(world, commandBuffer));

	std::vector<std::type_index> systemWriteList = GET_SYSTEM_WRITELIST(systemType);
	std::vector<std::type_index> systemReadList = GET_SYSTEM_READLIST(systemType);
	writeList.insert(writeList.end(), systemWriteList.begin(), systemWriteList.end());
	readList.insert(readList.end(), systemReadList.begin(), systemReadList.end());
}

void Phase::ClearSystems()
{
	// Destroy command buffers
	for(const CommandBuffer* commandBuffer : commandBuffers)
		delete commandBuffer;

	// Destroy system contexts
	for(const SystemContextBase* ctx : systemContexts)
		delete ctx;
	
	commandBuffers.clear();
	systems.clear();
	systemContexts.clear();
}