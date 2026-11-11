#pragma once

#include "Entity.h"
#include "TypeRegistry.h"

#include "AlgorithmUtils.h"

struct ComponentRef
{
	Entity entity;
	TypeID componentType;

	ComponentRef();
	ComponentRef(const Entity& entity, TypeID componentType);
	~ComponentRef() = default;

	bool operator==(const ComponentRef& other) const { return entity == other.entity && componentType == other.componentType; }
};
namespace std
{
	template<>
	struct hash<ComponentRef>
	{
		size_t operator()(const ComponentRef& c) const
		{
			return CombineHashes(std::hash<Entity>()(c.entity), std::hash<TypeID>()(c.componentType));
		}
	};
}

struct ComponentNetworkRef
{
	size_t networkID;
	TypeID componentType;

	ComponentNetworkRef();
	ComponentNetworkRef(size_t networkID, TypeID componentType);
	~ComponentNetworkRef() = default;

	bool operator==(const ComponentNetworkRef& other) const { return networkID == other.networkID && componentType == other.componentType; }
};
namespace std
{
	template<>
	struct hash<ComponentNetworkRef>
	{
		size_t operator()(const ComponentNetworkRef& c) const
		{
			return CombineHashes(c.networkID, std::hash<TypeID>()(c.componentType));
		}
	};
}

template<>
inline void ByteStreamBase::Serialize(const ComponentRef& src);
template<>
inline void ByteStreamBase::Deserialize(ComponentRef& dst);

template<>
inline void ByteStreamBase::Serialize(const ComponentNetworkRef& src);
template<>
inline void ByteStreamBase::Deserialize(ComponentNetworkRef& dst);