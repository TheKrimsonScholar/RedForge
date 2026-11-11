#pragma once

#include "TypeRegistry.h"

#include <stdexcept>

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
	typeInfo.constructInstance = []() -> void*
		{
			if constexpr(std::is_default_constructible_v<T>)
				return new T();
			else
				assert(false, "Type is not default constructible.");

			return nullptr;
		};
	typeInfo.copyConstructInstance = [](const void* src) -> void*
		{
			if constexpr(std::is_copy_constructible_v<T>)
			{
				const T* srcPtr = reinterpret_cast<const T*>(src);
				
				T* newObj = new T(*srcPtr);

				return newObj;
			}
			else
				assert(false, "Type is not copy constructible.");

			return nullptr;
		};
	typeInfo.copyInstance = [](const void* src, void* dst)
		{
			if constexpr(std::is_copy_assignable_v<T>)
			{
				const T* srcPtr = reinterpret_cast<const T*>(src);
				T* dstPtr = reinterpret_cast<T*>(dst);

				*dstPtr = *srcPtr;
			}
			else
				assert(false, "Type is not copy assignable.");
		};
	typeInfo.destroyInstance = [](void* ptr)
		{
			if constexpr(std::is_destructible_v<T>)
			{
				T* objPtr = reinterpret_cast<T*>(ptr);
				
				delete objPtr;
			}
			else
				assert(false, "Type is not destructible.");
		};
	typeInfo.serialize = [](ByteStreamBase& str, void* src)
		{
			if constexpr(IsTypeSerializable<T>)
			{
				T* srcObj = reinterpret_cast<T*>(src);

				str << *srcObj;
			}
			else
				assert(false, "Type is not serializable.");
		};
	typeInfo.deserialize = [](ByteStreamBase& str, void* dst)
		{
			if constexpr(IsTypeDeserializable<T>)
			{
				T* dstObj = reinterpret_cast<T*>(dst);
					
				str >> *dstObj;
			}
			else
				assert(false, "Type is not deserializable.");
		};
	typeInfo.serializePacket = [](ByteStream<IPacketStreamFormat>& str, void* src)
		{
			if constexpr(IsTypeSerializableInFormat<IPacketStreamFormat, T>)
			{
				T* srcObj = reinterpret_cast<T*>(src);
					
				str << *srcObj;
			}
			else
				assert(false, "Type is not serializable in packet format.");
		};
	typeInfo.deserializePacket = [](ByteStream<IPacketStreamFormat>& str, void* dst)
		{
			if constexpr (IsTypeDeserializableInFormat<IPacketStreamFormat, T>)
			{
				T* dstObj = reinterpret_cast<T*>(dst);

				str >> *dstObj;
			}
			else
				assert(false, "Type is not deserializable in packet format.");
		};

	registeredTypesList.push_back(typeInfo);
}

#define REGISTER_TYPE(Type, ...)						\
struct RegisterType_##Type : public RegisterTypeBase	\
{														\
	RegisterType_##Type() : RegisterTypeBase(#Type)		\
	{													\
		AddToRegistry<Type, __VA_ARGS__>();				\
	}													\
	void Register() override							\
	{													\
		RegisterType<Type, __VA_ARGS__>();				\
	}													\
};														\
inline RegisterType_##Type RegisterType_##Type##_Instance;
#define TYPE_HAS_TAG(TagType) RegisterComponentWithTag(typeID, TypeRegistry::GetTypeID<TagType>());