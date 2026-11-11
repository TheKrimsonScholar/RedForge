#pragma once

#include "TypeRegistry.h"

struct GenericEventData
{
private:
	TypeID eventType = INVALID_TYPE;
	void* eventPtr = nullptr;

public:
	GenericEventData();
	GenericEventData(const GenericEventData& other);
	GenericEventData(GenericEventData&& other) noexcept;
	GenericEventData(TypeID eventType, const void* eventPtr);
	~GenericEventData();

	GenericEventData& operator=(const GenericEventData& other);

	TypeID GetEventType() const;
	void* GetEventPtr() const;

	friend class ByteStreamBase;
};

template<>
void ByteStreamBase::Serialize(const GenericEventData& src);
template<>
void ByteStreamBase::Deserialize(GenericEventData& dst);