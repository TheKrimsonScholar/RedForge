#pragma once

#include <vector>
#include <typeindex>

#include "SystemContext.h"
#include "EngineParams.h"

#include "SystemRegistrationMacros.h"

class SystemBase
{
protected:
	virtual void Startup(const EngineStartupParams& params, World& world) = 0;
	virtual void PostStartup(const EngineStartupParams& params, World& world) = 0;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) = 0;

	virtual void Update(SystemContextBase* ctx, float deltaTime) = 0;
	
public:
	virtual SystemContextBase* CreateContext(World* world, CommandBuffer* commandBuffer) = 0;
	
	friend class Scheduler;
};
template<typename... Components>
class System : public SystemBase
{
protected:
	typedef SystemContext<Components...> LocalSystemContext;

	void Update(SystemContextBase* ctx, float deltaTime) override;
	virtual void Update(LocalSystemContext& ctx, float deltaTime) = 0;

public:
	SystemContextBase* CreateContext(World* world, CommandBuffer* commandBuffer) override
	{
		return new LocalSystemContext(world, commandBuffer);
	}

	static void GetComponentLists(std::vector<std::type_index>& out_writeList, std::vector<std::type_index>& out_readList)
	{
		auto lambda = [&]<typename T>()
		{
			if(std::is_const<T>())
				out_readList.push_back(typeid(T));
			else
				out_writeList.push_back(typeid(T));
		};
		(lambda.template operator()<Components>(), ...);
	}
};

template<typename ...Components>
inline void System<Components...>::Update(SystemContextBase* ctx, float deltaTime)
{
	Update(*static_cast<LocalSystemContext*>(ctx), deltaTime);
}