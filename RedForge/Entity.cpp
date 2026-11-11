#include "Entity.h"

template<>
inline void ByteStreamBase::Serialize(const Entity& src)
{
	*this << src.index;
	*this << src.generation;
}
template<>
inline void ByteStreamBase::Deserialize(Entity& dst)
{
	*this >> dst.index;
	*this >> dst.generation;
}