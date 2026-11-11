#include "ByteStream.h"

#include <type_traits>

void ByteStreamBase::CopyBytesIntoBuffer(const void* data, size_t size)
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);

	buffer.insert(buffer.end(), bytes, bytes + size);
}
void ByteStreamBase::Clear()
{
	buffer.clear();
	readOffset = 0;
}

std::vector<uint8_t> ByteStreamBase::GetBytes()
{
	std::vector<uint8_t> bytesToEnd(buffer.begin() + readOffset, buffer.end());
	readOffset = buffer.size();

	return bytesToEnd;
}
std::vector<uint8_t> ByteStreamBase::GetAllBytes() const
{
	return buffer;
}
size_t ByteStreamBase::GetSize() const
{
	return buffer.size();
}
bool ByteStreamBase::HasNext() const
{
	return readOffset < buffer.size();
}

template<>
void ByteStreamBase::Serialize(const std::vector<uint8_t>& src)
{
	*this << src.size();
	buffer.insert(buffer.end(), src.begin(), src.end());
}
template<>
void ByteStreamBase::Deserialize(std::vector<uint8_t>& dst)
{
	size_t size;
	*this >> size;
	dst = std::vector<uint8_t>(buffer.begin() + readOffset, buffer.begin() + (readOffset + size));
}

template<>
void ByteStreamBase::Serialize(const std::string& src)
{
	*this << src.size();
	for(size_t i = 0; i < src.size(); i++)
		*this << src[i];
}
template<>
void ByteStreamBase::Deserialize(std::string& dst)
{
	size_t size;
	*this >> size;

	dst.resize(size);
	for(size_t i = 0; i < size; i++)
		*this >> dst[i];
}
template<>
void ByteStreamBase::Serialize(const std::filesystem::path& src)
{
	*this << src.string();
}
template<>
void ByteStreamBase::Deserialize(std::filesystem::path& dst)
{
	std::string pathString;
	*this >> pathString;

	dst = std::filesystem::path(pathString);
}