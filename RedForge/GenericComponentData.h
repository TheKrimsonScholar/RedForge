#pragma once

#include "TypeRegistry.h"

struct GenericComponentData
{
private:
	TypeID componentType = INVALID_TYPE;
	void* componentPtr = nullptr;

public:
	GenericComponentData();
	GenericComponentData(const GenericComponentData& other);
	GenericComponentData(GenericComponentData&& other) noexcept;
	GenericComponentData(TypeID componentType, const void* component);
	~GenericComponentData();

	void operator=(const GenericComponentData& other);

	TypeID GetComponentType() const;
	void* GetComponentPtr() const;

	friend class ByteStreamBase;
	friend class ByteStream<IPacketStreamFormat>;
};

template<>
void ByteStreamBase::Serialize(const GenericComponentData& src);
template<>
void ByteStreamBase::Deserialize(GenericComponentData& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const GenericComponentData& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(GenericComponentData& dst);