#pragma once

#include <algorithm>
#include <format>

#include "ByteStream.h"

static const uint32_t INVALID_ENTITY = 0xFFFFFFFF;

struct Entity
{
public:
	uint32_t index = INVALID_ENTITY;
	// The generation of this entity, used to determine whether the entity is out-of-date.
	// If 0, the entity is transient, i.e. it is owned by a non-authoritative source (like a command buffer) and will be added to the entity manager as a new entity with a different permanent ID in the future.
	uint32_t generation = INVALID_ENTITY;

	// Entity data should only be managed by EntityManager and LevelManager
	friend class EntityManager;
	friend class LevelManager;

	friend struct std::formatter<Entity, char>;
	friend struct std::hash<Entity>;

public:
	bool operator==(const Entity& other) const { return index == other.index && generation == other.generation; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	bool IsValid() const { return index != INVALID_ENTITY && generation != INVALID_ENTITY; }
};
template <>
struct std::formatter<Entity, char>
{
	constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const Entity& e, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "Entity(index={}, generation={})", e.index, e.generation);
	}
};
namespace std
{
	template<>
	struct hash<Entity>
	{
		size_t operator()(const Entity& e) const
		{
			std::hash<uint32_t> hasher;
			size_t seed = hasher(e.index);
			seed ^= hasher(e.generation) + 0x9e3779b9 + (seed << 6) + (seed >> 2); // 0x9e3779b9 is inverse golden ratio

			return seed;
		}
	};
}

template<>
inline void ByteStreamBase::Serialize(const Entity& src);
template<>
inline void ByteStreamBase::Deserialize(Entity& dst);