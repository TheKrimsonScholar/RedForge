#pragma once

#include "TypeRegistration.h"

#include "EntityManager.h"

class RegisterComponentTypeBase : public RegisterTypeBase
{
protected:
	RegisterComponentTypeBase(const std::string& typeName) : RegisterTypeBase(typeName) {};

	template<typename T, typename... Bases>
	void RegisterComponentType();
	REDFORGE_API void RegisterComponentDependency(TypeID typeID, TypeID dependency);
	REDFORGE_API void RegisterComponentVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)>& getVariables);
};

template<typename T, typename... Bases>
inline void RegisterComponentTypeBase::RegisterComponentType()
{
	std::vector<TypeID>& registeredComponentsList = GetRegistryInstance().registeredComponentsList;

	// If this type was already registered, don't register it again
	if(std::find(registeredComponentsList.begin(), registeredComponentsList.end(), TYPEID(T)) != registeredComponentsList.end())
		return;

	RegisterType<T, Bases...>();
	registeredComponentsList.push_back(TYPEID(T));
	GetRegistryInstance().componentDependencyHierarchy.AddNode(TYPEID(T));

	ComponentTypeInfo componentTypeInfo = {};
	componentTypeInfo.registerComponent = [](EntityManager& entityManager) { entityManager.RegisterComponent<T>(); };
	componentTypeInfo.getVariables = [](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableTypeInfo>> { return {}; };
	
	GetRegistryInstance().registeredComponentInfoMap.emplace(TYPEID(T), componentTypeInfo);
}

#define SERIALIZED_COMPONENT_VARS(...)																													\
void SerializeComponent(ByteStreamBase& str) const																										\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeComponent(ByteStreamBase& str)																											\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}
#define REPLICATED_COMPONENT_VARS(...)																													\
void SerializeComponent(ByteStream<IPacketStreamFormat>& str) const																						\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeComponent(ByteStream<IPacketStreamFormat>& str)																							\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}

#define REGISTER_COMPONENT_TYPE(Type, ...) REGISTER_COMPONENT_TYPE_BEGIN(Type, __VA_ARGS__) REGISTER_COMPONENT_TYPE_END(Type)
#define REGISTER_COMPONENT_TYPE_BEGIN(Type, ...)																										\
struct RegisterComponentType_##Type : public RegisterComponentTypeBase																					\
{																																						\
	RegisterComponentType_##Type() : RegisterComponentTypeBase(#Type)																					\
	{																																					\
		AddToRegistry<Type, __VA_ARGS__>();																												\
	}																																					\
	void Register() override																															\
	{																																					\
		if(TypeRegistry::IsComponentTypeRegistered(typeid(Type)))																						\
			return;																																		\
																																						\
		auto castComponentPtr = [](void* rawComponent) -> Type*																							\
			{																																			\
				return static_cast<Type*>(rawComponent);																								\
			};																																			\
																																						\
		TypeID typeID = TYPEID(Type);																													\
		RegisterComponentType<Type, __VA_ARGS__>();
#define COMPONENT_REQUIRES(DependencyType) RegisterComponentDependency(typeID, TYPEID(DependencyType));
#define COMPONENT_VARS_BEGIN																															\
		RegisterComponentVariables(typeID, [castComponentPtr](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableTypeInfo>>			\
			{																																			\
				auto component = castComponentPtr(rawComponent);																						\
																																						\
				return																																	\
				{
#define COMPONENT_VAR(variableType, variableName)																										\
					{																																	\
						&component->variableName,																										\
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
#define COMPONENT_VAR_REPLICATED(variableType, variableName)																							\
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
#define COMPONENT_VARS_END																																\
				};																																		\
			});
#define REGISTER_COMPONENT_TYPE_END(Type)																												\
	}																																					\
};																																						\
inline RegisterComponentType_##Type RegisterComponentType_##Type##_Instance;																			\
																																						\
template<>																																				\
inline void ByteStreamBase::Serialize(const Type& src)																									\
{																																						\
	src.SerializeComponent(*this);																														\
}																																						\
template<>																																				\
inline void ByteStreamBase::Deserialize(Type& dst)																										\
{																																						\
	dst.DeserializeComponent(*this);																													\
}																																						\
																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Serialize(const Type& src)																					\
{																																						\
	src.SerializeComponent(*this);																														\
}																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Deserialize(Type& dst)																						\
{																																						\
	dst.DeserializeComponent(*this);																													\
}