#include "GenericEventData.h"

#include "TypeRegistration.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

REGISTER_TYPE(GenericEventData)

GenericEventData::GenericEventData()
{

}
GenericEventData::GenericEventData(const GenericEventData& other)
    : eventType(other.eventType)
{
    eventPtr = CONSTRUCT_COPY(eventType)(other.eventPtr);
}
GenericEventData::GenericEventData(GenericEventData&& other) noexcept
	: eventType(other.eventType), eventPtr(other.eventPtr)
{
	other.eventType = INVALID_TYPE;
	other.eventPtr = nullptr;
}
GenericEventData::GenericEventData(TypeID eventType, const void* event)
    : eventType(eventType)
{
    eventPtr = CONSTRUCT_COPY(eventType)(event);
}
GenericEventData::~GenericEventData()
{
    if(eventPtr)
    {
        DESTROY(eventType)(eventPtr);
        eventPtr = nullptr;
    }
}

GenericEventData& GenericEventData::operator=(const GenericEventData& other)
{
    if(eventPtr)
    {
        DESTROY(eventType)(eventPtr);
        eventPtr = nullptr;
    }

    eventType = other.eventType;
    eventPtr = CONSTRUCT_COPY(eventType)(other.eventPtr);

    return *this;
}

TypeID GenericEventData::GetEventType() const
{
    return eventType;
}
void* GenericEventData::GetEventPtr() const
{
    return eventPtr;
}

template<>
void ByteStreamBase::Serialize(const GenericEventData& src)
{
    *this << src.eventType;

    SERIALIZE(src.eventType)(*this, src.eventPtr);
}
template<>
void ByteStreamBase::Deserialize(GenericEventData& dst)
{
	*this >> dst.eventType;

    // Reallocate the event pointer
    if(dst.eventPtr)
    {
        DESTROY(dst.eventType)(dst.eventPtr);
        dst.eventPtr = nullptr;
    }
    dst.eventPtr = CONSTRUCT(dst.eventType)();

    DESERIALIZE(dst.eventType)(*this, dst.eventPtr);
}