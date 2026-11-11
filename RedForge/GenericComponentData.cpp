#include "GenericComponentData.h"

#include "TypeRegistration.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

REGISTER_TYPE(GenericComponentData)

GenericComponentData::GenericComponentData()
{

}
GenericComponentData::GenericComponentData(const GenericComponentData& other)
	: componentType(other.componentType)
{
	componentPtr = CONSTRUCT_COPY(componentType)(other.componentPtr);
}
GenericComponentData::GenericComponentData(GenericComponentData&& other) noexcept
	: componentType(other.componentType), componentPtr(other.componentPtr)
{
	other.componentType = INVALID_TYPE;
	other.componentPtr = nullptr;
}
GenericComponentData::GenericComponentData(TypeID componentType, const void* component)
    : componentType(componentType)
{
    componentPtr = CONSTRUCT_COPY(componentType)(component);
}
GenericComponentData::~GenericComponentData()
{
	if(componentPtr)
    {
        DESTROY(componentType)(componentPtr);
        componentPtr = nullptr;
    }
}

void GenericComponentData::operator=(const GenericComponentData& other)
{
    if(componentPtr)
    {
        DESTROY(componentType)(componentPtr);
        componentPtr = nullptr;
    }

    componentType = other.componentType;
    componentPtr = CONSTRUCT_COPY(componentType)(other.componentPtr);
}

TypeID GenericComponentData::GetComponentType() const
{
    return componentType;
}
void* GenericComponentData::GetComponentPtr() const
{
    return componentPtr;
}

template<>
void ByteStreamBase::Serialize(const GenericComponentData& src)
{
	*this << src.componentType;

	SERIALIZE(src.componentType)(*this, src.componentPtr);
}
template<>
void ByteStreamBase::Deserialize(GenericComponentData& dst)
{
	*this >> dst.componentType;

	// Reallocate the component data pointer
	if(dst.componentPtr)
	{
		DESTROY(dst.componentType)(dst.componentPtr);
		dst.componentPtr = nullptr;
	}
	dst.componentPtr = CONSTRUCT(dst.componentType)();

	DESERIALIZE(dst.componentType)(*this, dst.componentPtr);
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const GenericComponentData& src)
{
    *this << src.componentType;

    SERIALIZE_TO_PACKET(src.componentType)(*this, src.componentPtr);
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(GenericComponentData& dst)
{
    *this >> dst.componentType;

    // Reallocate the component data pointer
    if(dst.componentPtr)
    {
        DESTROY(dst.componentType)(dst.componentPtr);
        dst.componentPtr = nullptr;
    }
    dst.componentPtr = CONSTRUCT(dst.componentType)();

    DESERIALIZE_FROM_PACKET(dst.componentType)(*this, dst.componentPtr);
}