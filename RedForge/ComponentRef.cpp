#include "ComponentRef.h"

ComponentRef::ComponentRef()
{

}
ComponentRef::ComponentRef(const Entity& entity, TypeID componentType)
	: entity(entity), componentType(componentType)
{

}

ComponentNetworkRef::ComponentNetworkRef()
{

}
ComponentNetworkRef::ComponentNetworkRef(size_t networkID, TypeID componentType)
	: networkID(networkID), componentType(componentType)
{

}

template<>
inline void ByteStreamBase::Serialize(const ComponentRef& src)
{
	*this << src.entity;
	*this << src.componentType;
}
template<>
inline void ByteStreamBase::Deserialize(ComponentRef& dst)
{
	*this >> dst.entity;
	*this >> dst.componentType;
}

template<>
inline void ByteStreamBase::Serialize(const ComponentNetworkRef& src)
{
	*this << src.networkID;
	*this << src.componentType;
}
template<>
inline void ByteStreamBase::Deserialize(ComponentNetworkRef& dst)
{
	*this >> dst.networkID;
	*this >> dst.componentType;
}