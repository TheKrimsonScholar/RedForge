#pragma once

#include "TypeRegistry.h"

class RegisterTypeBase
{
protected:
	std::string typeName;

protected:
	RegisterTypeBase(const std::string& typeName) : typeName(typeName) {};

	template<typename T, typename... Bases>
	void AddToRegistry();

	virtual void Register() = 0;
	template<typename T, typename... Bases>
	void RegisterType();
	
	static bool Compare(const RegisterTypeBase& a, const RegisterTypeBase& b) { return a.typeName < b.typeName; }

	REDFORGE_API TypeRegistry& GetRegistryInstance();

	friend class TypeRegistry;
};

template<typename T, typename ...Bases>
inline void RegisterTypeBase::AddToRegistry()
{
	GetRegistryInstance().AddTypeToRegistry<T, Bases...>(this);
}

template<typename T, typename... Bases>
inline void RegisterTypeBase::RegisterType()
{
	std::vector<TypeInfo>& registeredTypesList = GetRegistryInstance().registeredTypesList;

	// If this type was already registered, don't register it again
	if(registeredTypesList.size() > TYPEID(T).id)
		return;
		
	TypeInfo typeInfo = {};
	std::string typeName = typeid(T).name();
	typeInfo.name = typeName.substr(typeName.find_first_of(' ') + 1); // Remove the "class " or "struct " prefix from the type name
	typeInfo.size = sizeof(T);

	registeredTypesList.push_back(typeInfo);
}

#define REGISTER_TYPE(Type, ...)\
struct RegisterType_##Type : public RegisterTypeBase\
{\
	RegisterType_##Type() : RegisterTypeBase(#Type)\
	{\
		AddToRegistry<Type, __VA_ARGS__>();\
	}\
	void Register() override\
	{\
		RegisterType<Type, __VA_ARGS__>();\
	}\
};\
inline RegisterType_##Type RegisterType_##Type##_Instance;
#define TYPE_HAS_TAG(TagType) RegisterComponentWithTag(typeID, TypeRegistry::GetTypeID<TagType>());