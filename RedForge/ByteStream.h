#pragma once

#include <vector>
#include <string>
#include <type_traits>
#include <concepts>

struct IByteStreamFormat {};
struct ILogStreamFormat : public IByteStreamFormat {};
struct IFileStreamFormat : public IByteStreamFormat {};
struct IPacketStreamFormat : public IByteStreamFormat {};

struct ByteStreamBase
{
	std::vector<uint8_t> buffer;
	size_t readOffset = 0;

	template<typename T>
	ByteStreamBase& operator<<(const T& src);
	template<typename T>
	ByteStreamBase& operator>>(T& dst);

	template<typename T>
	void Serialize(const T& src) = delete;
	template<typename T>
	void Deserialize(T& dst) = delete;

	template<typename... Args>
	void SerializeAll(const Args&... srcs)
	{
		(*this << ... << srcs);
	}
	template<typename... Args>
	void DeserializeAll(Args&... dsts)
	{
		(*this >> ... >> dsts);
	}

	void Clear()
	{
		buffer.clear();
		readOffset = 0;
	}
};

template<typename Format>
struct ByteStream;

template<typename T>
concept IsTypeSerializable = requires(ByteStreamBase str, T src) { str.Serialize(src); };
template<typename T>
concept IsTypeDeserializable = requires(ByteStreamBase str, T dst) { str.Deserialize(dst); };
template<typename Format, typename T>
concept IsTypeSerializableInFormat = requires(ByteStream<Format> str, T src) { str.Serialize(src); };
template<typename Format, typename T>
concept IsTypeDeserializableInFormat = requires(ByteStream<Format> str, T dst) { str.Deserialize(dst); };

template<typename Format>
struct ByteStream : public ByteStreamBase
{
	static_assert(std::is_base_of_v<IByteStreamFormat, Format>, "Byte stream format must derive from IByteStreamFormat.");

	ByteStream() {}
	~ByteStream() {}

	template<typename T>
	ByteStream<Format>& operator<<(const T& src)
	{
		// Use the specialized function for this format if it has been defined
		if constexpr(IsTypeSerializableInFormat<Format, T>)
			Serialize(src);
		// Otherwise call the base class implmentation
		else
			*static_cast<ByteStreamBase*>(this) << src;
		
		return *this;
	}
	template<typename T>
	ByteStream<Format>& operator>>(T& dst)
	{
		// Use the specialized function for this format if it has been defined
		if constexpr(IsTypeDeserializableInFormat<Format, T>)
			Deserialize(dst);
		// Otherwise call the base class implementation
		else
			*static_cast<ByteStreamBase*>(this) >> dst;
		
		return *this;
	}

	template<typename T>
	void Serialize(const T& src) = delete;
	template<typename T>
	void Deserialize(T& dst) = delete;

	template<typename... Args>
	void SerializeAll(const Args&... srcs)
	{
		(*this << ... << srcs);
	}
	template<typename... Args>
	void DeserializeAll(Args&... dsts)
	{
		(*this >> ... >> dsts);
	}
};

template<typename T>
ByteStreamBase& ByteStreamBase::operator<<(const T& src)
{
	// For primitive types, just copy the bytes directly
	if constexpr(std::is_trivially_copyable_v<T>)
	{
		size_t oldSize = buffer.size();

		buffer.resize(oldSize + sizeof(T));
		memcpy(&buffer[oldSize], &src, sizeof(T));
	}
	// Otherwise, use the custom serialization function (must be defined via template specialization)
	else
	{
		if constexpr(IsTypeSerializable<T>)
			Serialize(src);
		else static_assert(false, "Non-primitive serialized type must define a serialization function by specializing ByteStreamBase::Serialize<T>(const T& src)");
	}
	
	return *this;
}
template<typename T>
ByteStreamBase& ByteStreamBase::operator>>(T& dst)
{
	// For primitive types, just copy the bytes directly
	if constexpr(std::is_trivially_copyable_v<T>)
	{
		memcpy(&dst, &buffer[readOffset], sizeof(T));
		readOffset += sizeof(T);
	}
	// Otherwise, use the custom deserialization function (must be defined via template specialization)
	else
	{
		if constexpr(IsTypeDeserializable<T>)
			Deserialize(dst);
		else static_assert(false, "Non-primitive serialized type must define a deserialization function by specializing ByteStreamBase::Deserialize<T>(T& dst)");
	}

	return *this;
}

/* Default specializations for standard non-primitive data types */

template<>
inline void ByteStreamBase::Serialize(const std::string& src)
{
	*this << src.size();
	for(size_t i = 0; i < src.size(); i++)
		*this << src[i];
}
template<>
inline void ByteStreamBase::Deserialize(std::string& dst)
{
	size_t size;
	*this >> size;

	dst.resize(size);
	for(size_t i = 0; i < size; i++)
		*this >> dst[i];
}