#pragma once

#include <typeindex>

#include "TypeRegistration.h"

struct GenericComponentData
{
	TypeID componentType;
	size_t dataSize;
	void* componentData;
};

template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(GenericComponentData& dst)
{
	TypeID componentType;
	*this >> componentType;

	DESERIALIZE_COMPONENT(componentType)(*this, dst.componentData);
}

//// Provide a non-template overload for operator>> so we don't need to specialize
//// the member function template (which is difficult to declare for operator>>).
//inline ByteStream<IPacketStreamFormat>& operator>>(ByteStream<IPacketStreamFormat>& bs, GenericComponentData& dst)
//{
//	RFComponentTypeID componentType;
//	bs >> componentType;
//
//	DESERIALIZE_COMPONENT(componentType)(bs, dst.componentData);
//
//	return bs;
//}