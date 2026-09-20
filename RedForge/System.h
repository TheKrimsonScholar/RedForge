#pragma once

#include <vector>
#include <typeindex>

#include "SystemContext.h"
#include "EngineParams.h"

#include "TypeRegistration.h"

class SystemBase
{
public:
	virtual ~SystemBase() = default;

protected:
	virtual void Startup(const EngineStartupParams& params, World& world) = 0;
	virtual void PostStartup(const EngineStartupParams& params, World& world) = 0;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) = 0;

	virtual void Update(SystemContextBase* ctx, float deltaTime) = 0;
	
public:
	virtual SystemContextBase* CreateContext(World* world, CommandBuffer* commandBuffer) = 0;
	
	friend class Scheduler;
};
REGISTER_TYPE(SystemBase)
template<typename... Components>
class System : public SystemBase
{
	typedef System<Components...> LocalSystemAlias;
	struct RegisterType_LocalSystemAlias : public RegisterTypeBase {
		RegisterType_LocalSystemAlias() : RegisterTypeBase("LocalSystemAlias") {
			AddToRegistry<LocalSystemAlias, SystemBase>();
		} void Register() override {
			RegisterType<LocalSystemAlias, SystemBase>();
		}
	};static inline RegisterType_LocalSystemAlias RegisterType_LocalSystemAlias_Instance;

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