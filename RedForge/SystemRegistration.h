#pragma once

#include "TypeRegistration.h"

class RegisterSystemTypeBase : public RegisterTypeBase
{
protected:
	RegisterSystemTypeBase(const std::string& typeName) : RegisterTypeBase(typeName) {};

	template<typename T, typename... Bases>
	void RegisterSystemType();
	REDFORGE_API void RegisterSystemDependency(TypeID typeID, TypeID dependency);
};

template<typename T, typename ...Bases>
inline void RegisterSystemTypeBase::RegisterSystemType()
{
	std::vector<TypeID>& registeredSystemsList = GetRegistryInstance().registeredSystemsList;

	// If this type was already registered, don't register it again
	if(std::find(registeredSystemsList.begin(), registeredSystemsList.end(), TYPEID(T)) != registeredSystemsList.end())
		return;
	
	RegisterType<T, Bases...>();
	registeredSystemsList.push_back(TYPEID(T));
	GetRegistryInstance().systemDependencyHierarchy.AddNode(TYPEID(T));

	SystemTypeInfo systemTypeInfo = {};
	systemTypeInfo.createSystem = []() { return new T(); };
	T::GetComponentLists(systemTypeInfo.writeList, systemTypeInfo.readList);

	GetRegistryInstance().registeredSystemInfoMap.emplace(TYPEID(T), systemTypeInfo);
}

#define REGISTER_SYSTEM_TYPE(Type, ...) REGISTER_SYSTEM_TYPE_BEGIN(Type, __VA_ARGS__) REGISTER_SYSTEM_TYPE_END(Type)
#define REGISTER_SYSTEM_TYPE_BEGIN(Type, ...)																											\
struct RegisterSystemType_##Type : public RegisterSystemTypeBase																						\
{																																						\
	RegisterSystemType_##Type() : RegisterSystemTypeBase(#Type)																							\
	{																																					\
		AddToRegistry<Type, __VA_ARGS__>();																												\
	}																																					\
	void Register() override																															\
	{																																					\
		if(TypeRegistry::IsSystemTypeRegistered(typeid(Type)))																							\
			return;																																		\
																																						\
		TypeID typeID = TYPEID(Type);																													\
		RegisterSystemType<Type, __VA_ARGS__>();
#define SYSTEM_REQUIRES(DependencyType)	RegisterSystemDependency(typeID, TYPEID(DependencyType));
#define REGISTER_SYSTEM_TYPE_END(Type)																													\
	}																																					\
};																																						\
inline RegisterSystemType_##Type RegisterSystemType_##Type##_Instance;