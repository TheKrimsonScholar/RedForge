#pragma once

#include "TypeRegistration.h"

class RegisterResourceTypeBase : public RegisterTypeBase
{
protected:
	RegisterResourceTypeBase(const std::string& typeName) : RegisterTypeBase(typeName) {};

	template<typename T, typename... Bases>
	void RegisterResourceType();
	REDFORGE_API void RegisterResourceDependency(TypeID typeID, TypeID dependency);
	REDFORGE_API void RegisterResourceVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)>& getVariables);
	
	template<typename T>
	T* CreateResource()
	{
		static_assert(std::is_base_of_v<IResource, T>);

		return new T();
	}
};

template<typename T, typename... Bases>
inline void RegisterResourceTypeBase::RegisterResourceType()
{
	std::vector<TypeID>& registeredResourcesList = GetRegistryInstance().registeredResourcesList;

	// If this type was already registered, don't register it again
	if(std::find(registeredResourcesList.begin(), registeredResourcesList.end(), TYPEID(T)) != registeredResourcesList.end())
		return;

	RegisterType<T, Bases...>();
	registeredResourcesList.push_back(TYPEID(T));
	GetRegistryInstance().resourceDependencyHierarchy.AddNode(TYPEID(T));

	ResourceTypeInfo resourceTypeInfo = {};
	resourceTypeInfo.createResource = [this]() { return CreateResource<T>(); };
	resourceTypeInfo.getVariables = [](void* rawResource) -> std::vector<std::pair<void*, ResourceVariableTypeInfo>> { return {}; };

	GetRegistryInstance().registeredResourceInfoMap.emplace(TYPEID(T), resourceTypeInfo);
}

#define SERIALIZED_RESOURCE_VARS(...)																													\
void SerializeResource(ByteStreamBase& str) const																										\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeResource(ByteStreamBase& str)																											\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}
#define REPLICATED_RESOURCE_VARS(...)																													\
void SerializeResource(ByteStream<IPacketStreamFormat>& str) const																						\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeResource(ByteStream<IPacketStreamFormat>& str)																							\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}

#define REGISTER_RESOURCE_TYPE(Type, ...) REGISTER_RESOURCE_TYPE_BEGIN(Type, __VA_ARGS__) REGISTER_RESOURCE_TYPE_END(Type)
#define REGISTER_RESOURCE_TYPE_BEGIN(Type, ...)																											\
struct RegisterResourceType_##Type : public RegisterResourceTypeBase																					\
{																																						\
	RegisterResourceType_##Type() : RegisterResourceTypeBase(#Type)																						\
	{																																					\
		AddToRegistry<Type, __VA_ARGS__>();																												\
	}																																					\
	void Register() override																															\
	{																																					\
		if(TypeRegistry::IsResourceTypeRegistered(typeid(Type)))																						\
			return;																																		\
																																						\
		auto castResourcePtr = [](void* rawResource) -> Type*																							\
			{																																			\
				return static_cast<Type*>(rawResource);																									\
			};																																			\
																																						\
		TypeID typeID = TYPEID(Type);																													\
		RegisterResourceType<Type, __VA_ARGS__>();
#define RESOURCE_DEPENDS_ON(DependencyType) RegisterResourceDependency(typeID, TYPEID(DependencyType));
#define RESOURCE_VARS_BEGIN																																\
		RegisterResourceVariables(typeID, [castResourcePtr](void* rawResource) -> std::vector<std::pair<void*, ResourceVariableTypeInfo>>				\
			{																																			\
				auto resource = castResourcePtr(rawResource);																							\
																																						\
				return																																	\
				{
#define RESOURCE_VAR(variableType, variableName)																										\
					{																																	\
						&resource->variableName,																										\
						{																																\
							typeid(variableType), #variableName, false, 																				\
																																						\
							[](std::istream& is, void* variablePtr) -> std::istream&																	\
								{																														\
									return is >> *static_cast<variableType*>(variablePtr);																\
								},																														\
							[](std::ostream& os, void* variablePtr) -> std::ostream&																	\
								{																														\
									return os << *static_cast<variableType*>(variablePtr) << " ";														\
								}																														\
						}																																\
					},			
#define RESOURCE_VAR_REPLICATED(variableType, variableName)																								\
					{																																	\
						&component->variableName,																										\
						{																																\
							typeid(variableType), #variableName, true, 																					\
																																						\
							[](std::istream& is, void* variablePtr) -> std::istream&																	\
								{																														\
									return is >> *static_cast<variableType*>(variablePtr);																\
								},																														\
							[](std::ostream& os, void* variablePtr) -> std::ostream&																	\
								{																														\
									return os << *static_cast<variableType*>(variablePtr) << " ";														\
								}																														\
						}																																\
					},
#define RESOURCE_VARS_END																																\
				};																																		\
			});
#define REGISTER_RESOURCE_TYPE_END(Type)																												\
	}																																					\
};																																						\
inline RegisterResourceType_##Type RegisterResourceType_##Type##_Instance;																				\
																																						\
template<>																																				\
inline void ByteStreamBase::Serialize(const Type& src)																									\
{																																						\
	src.SerializeResource(*this);																														\
}																																						\
template<>																																				\
inline void ByteStreamBase::Deserialize(Type& dst)																										\
{																																						\
	dst.DeserializeResource(*this);																														\
}																																						\
																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Serialize(const Type& src)																					\
{																																						\
	src.SerializeResource(*this);																														\
}																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Deserialize(Type& dst)																						\
{																																						\
	dst.DeserializeResource(*this);																														\
}