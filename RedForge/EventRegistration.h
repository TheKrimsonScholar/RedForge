#pragma once

#include "TypeRegistration.h"

#include "EventQueue.h"

class RegisterEventTypeBase : public RegisterTypeBase
{
protected:
	RegisterEventTypeBase(const std::string& typeName) : RegisterTypeBase(typeName) {};

	template<typename T, typename... Bases>
	void RegisterEventType();
	REDFORGE_API void RegisterEventVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, EventVariableTypeInfo>>(void*)>& getVariables);
};

template<typename T, typename ...Bases>
inline void RegisterEventTypeBase::RegisterEventType()
{
	std::vector<TypeID>& registeredEventsList = GetRegistryInstance().registeredEventsList;

	// If this type was already registered, don't register it again
	if(std::find(registeredEventsList.begin(), registeredEventsList.end(), TYPEID(T)) != registeredEventsList.end())
		return;
	
	RegisterType<T, Bases...>();
	registeredEventsList.push_back(TYPEID(T));

	EventTypeInfo eventTypeInfo = {};
	eventTypeInfo.createQueue = []() { return new EventQueue<T>(); };

	GetRegistryInstance().registeredEventInfoMap.emplace(TYPEID(T), eventTypeInfo);
}

#define SERIALIZED_EVENT_VARS(...)																													\
void SerializeEvent(ByteStreamBase& str) const																										\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeEvent(ByteStreamBase& str)																											\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}
#define REPLICATED_EVENT_VARS(...)																													\
void SerializeEvent(ByteStream<IPacketStreamFormat>& str) const																						\
{																																						\
	str.SerializeAll(__VA_ARGS__);																														\
}																																						\
void DeserializeEvent(ByteStream<IPacketStreamFormat>& str)																							\
{																																						\
	str.DeserializeAll(__VA_ARGS__);																													\
}

#define REGISTER_EVENT_TYPE(Type, ...) REGISTER_EVENT_TYPE_BEGIN(Type, __VA_ARGS__) REGISTER_EVENT_TYPE_END(Type)
#define REGISTER_EVENT_TYPE_BEGIN(Type, ...)																											\
struct RegisterEventType_##Type : public RegisterEventTypeBase																							\
{																																						\
	RegisterEventType_##Type() : RegisterEventTypeBase(#Type)																							\
	{																																					\
		AddToRegistry<Type, __VA_ARGS__>();																												\
	}																																					\
	void Register() override																															\
	{																																					\
		if(TypeRegistry::IsEventTypeRegistered(typeid(Type)))																							\
			return;																																		\
																																						\
		auto castEventPtr = [](void* rawEvent) -> Type*																									\
			{																																			\
				return static_cast<Type*>(rawEvent);																									\
			};																																			\
																																						\
		TypeID typeID = TYPEID(Type);																													\
		RegisterEventType<Type, __VA_ARGS__>();
#define EVENT_VARS_BEGIN																																\
		RegisterEventVariables(typeID, [castEventPtr](void* rawEvent) -> std::vector<std::pair<void*, EventVariableTypeInfo>>							\
			{																																			\
				auto event = castEventPtr(rawEvent);																									\
																																						\
				return																																	\
				{
#define EVENT_VAR(variableType, variableName)																											\
					{																																	\
						&event->variableName,																											\
						{ typeid(variableType), #variableName }																							\
					},
#define EVENT_VARS_END																																	\
				};																																		\
			});
#define REGISTER_EVENT_TYPE_END(Type)																													\
	}																																					\
};																																						\
inline RegisterEventType_##Type RegisterEventType_##Type##_Instance;																					\
																																						\
template<>																																				\
inline void ByteStreamBase::Serialize(const Type& src)																									\
{																																						\
	src.SerializeEvent(*this);																															\
}																																						\
template<>																																				\
inline void ByteStreamBase::Deserialize(Type& dst)																										\
{																																						\
	dst.DeserializeEvent(*this);																														\
}																																						\
																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Serialize(const Type& src)																					\
{																																						\
	src.SerializeEvent(*this);																															\
}																																						\
template<>																																				\
template<>																																				\
inline void ByteStream<IPacketStreamFormat>::Deserialize(Type& dst)																						\
{																																						\
	dst.DeserializeEvent(*this);																														\
}