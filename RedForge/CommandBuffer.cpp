#include "CommandBuffer.h"

template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::CreateEntityCommand& src)
{
	*this << src.name;
	*this << src.level;
	*this << src.parent;
	*this << src.previous;
	*this << src.next;
	*this << src.prefabPath;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::CreateEntityCommand& dst)
{
	*this >> dst.name;
	*this >> dst.level;
	*this >> dst.parent;
	*this >> dst.previous;
	*this >> dst.next;
	*this >> dst.prefabPath;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::DestroyEntityCommand& src)
{
	*this << src.entity;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::DestroyEntityCommand& dst)
{
	*this >> dst.entity;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::RenameEntityCommand& src)
{
	*this << src.entity;
	*this << src.name;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::RenameEntityCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.name;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityToLevelCommand& src)
{
	*this << src.entity;
	*this << src.level;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityToLevelCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.level;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::ReparentEntityCommand& src)
{
	*this << src.entity;
	*this << src.parent;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::ReparentEntityCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.parent;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityBeforeCommand& src)
{
	*this << src.entity;
	*this << src.next;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityBeforeCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.next;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityAfterCommand& src)
{
	*this << src.entity;
	*this << src.previous;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityAfterCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.previous;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::AddComponentCommand& src)
{
	*this << src.entity;
	*this << src.component;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::AddComponentCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.component;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::RemoveComponentCommand& src)
{
	*this << src.entity;
	*this << src.componentType;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::RemoveComponentCommand& dst)
{
	*this >> dst.entity;
	*this >> dst.componentType;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::QueueEventCommand& src)
{
	*this << src.event;
}
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::QueueEventCommand& dst)
{
	*this >> dst.event;
}