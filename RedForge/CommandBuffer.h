#pragma once

#include <vector>
#include <iostream>

#include "World.h"
#include "EntityManager.h"
#include "EventQueue.h"
#include "GenericComponentData.h"
#include "GenericEventData.h"

#include "DebugLogEvent.h"

struct ICommandStreamFormat : public IByteStreamFormat {};

class CommandBuffer
{
	enum class ECommandType : uint8_t
	{
		CreateEntity,
		DestroyEntity,
		RenameEntity,
		MoveEntityToLevel,
		ReparentEntity,
		MoveEntityBefore,
		MoveEntityAfter,
		AddComponent,
		RemoveComponent,
		QueueEvent
	};

	struct CreateEntityCommand
	{
		std::string name;
		LevelID level;
		Entity parent;
		Entity previous;
		Entity next;
		std::filesystem::path prefabPath;
	};
	struct DestroyEntityCommand
	{
		Entity entity;
	};
	struct RenameEntityCommand
	{
		Entity entity;
		std::string name;
	};
	struct MoveEntityToLevelCommand
	{
		Entity entity;
		LevelID level;
	};
	struct ReparentEntityCommand
	{
		Entity entity;
		Entity parent;
	};
	struct MoveEntityBeforeCommand
	{
		Entity entity;
		Entity next;
	};
	struct MoveEntityAfterCommand
	{
		Entity entity;
		Entity previous;
	};
	struct AddComponentCommand
	{
		Entity entity;
		GenericComponentData component;
	};
	struct RemoveComponentCommand
	{
		Entity entity;
		TypeID componentType;
	};
	struct QueueEventCommand
	{
		GenericEventData event;
	};

private:
	World* world;

	ByteStream<ICommandStreamFormat> buffer;
	//std::vector<std::byte> buffer;
	
	//size_t writePosition = 0;
	//size_t readPosition = 0;

	size_t entitiesCreated = 0;
	std::vector<Entity> createdEntities;

public:
	CommandBuffer(World* world) : world(world) {}

	Entity Command_CreateEntity(
		const std::string& name, LevelID level, const Entity& parent, const Entity& previous, const Entity& next, const std::filesystem::path& prefabPath)
	{
		CreateEntityCommand command = {};
		command.name = name;
		command.level = level;
		command.parent = parent;
		command.previous = previous;
		command.next = next;
		command.prefabPath = prefabPath;

		buffer << ECommandType::CreateEntity;
		buffer << command;

		Entity tempEntity;
		tempEntity.index = entitiesCreated++;
		tempEntity.generation = 0; // Set generation to 0 to indicate a transient entity

		return tempEntity;
	}
	void Command_DestroyEntity(const Entity& entity)
	{
		DestroyEntityCommand command = {};
		command.entity = entity;

		buffer << ECommandType::DestroyEntity;
		buffer << command;
	}
	void Command_RenameEntity(const Entity& entity, const std::string& name)
	{
		RenameEntityCommand command = {};
		command.entity = entity;
		command.name = name;
		
		buffer << ECommandType::RenameEntity;
		buffer << command;
	}
	void Command_MoveEntityToLevel(const Entity& entity, LevelID level)
	{
		MoveEntityToLevelCommand command = {};
		command.entity = entity;
		command.level = level;

		buffer << ECommandType::MoveEntityToLevel;
		buffer << command;
	}
	void Command_ReparentEntity(const Entity& entity, const Entity& parent)
	{
		ReparentEntityCommand command = {};
		command.entity = entity;
		command.parent = parent;
		
		buffer << ECommandType::ReparentEntity;
		buffer << command;
	}
	void Command_MoveEntityBefore(const Entity& entity, const Entity& next)
	{
		MoveEntityBeforeCommand command = {};
		command.entity = entity;
		command.next = next;
		
		buffer << ECommandType::MoveEntityBefore;
		buffer << command;
	}
	void Command_MoveEntityAfter(const Entity& entity, const Entity& previous)
	{
		MoveEntityAfterCommand command = {};
		command.entity = entity;
		command.previous = previous;

		buffer << ECommandType::MoveEntityAfter;
		buffer << command;
	}
	void Command_AddComponent(const Entity& entity, TypeID componentType, const void* component)
	{
		AddComponentCommand command = { entity, GenericComponentData(componentType, component) };

		buffer << ECommandType::AddComponent;
		buffer << command;
	}
	void Command_RemoveComponent(const Entity& entity, TypeID componentType)
	{
		RemoveComponentCommand command = {};
		command.entity = entity;
		command.componentType = componentType;
		
		buffer << ECommandType::RemoveComponent;
		buffer << command;
	}
	void Command_QueueEvent(TypeID eventType, void* event)
	{
		QueueEventCommand command = { GenericEventData(eventType, event) };

		buffer << ECommandType::QueueEvent;
		buffer << command;
	}

	void Flush()
	{
		ECommandType commandType;
		while(buffer.HasNext())
		{
			buffer >> commandType;
			switch(commandType)
			{
				case ECommandType::CreateEntity:
				{
					CreateEntityCommand command;
					buffer >> command;

					// Create the entity and add it to internal list for future reference
					if(command.previous.IsValid() || command.next.IsValid())
						createdEntities.push_back(world->GetEntityManager().CreateEntity(command.previous, command.next, command.name, command.prefabPath));
					else if(command.parent.IsValid())
						createdEntities.push_back(world->GetEntityManager().CreateEntity(command.parent, command.name, command.prefabPath));
					else
						createdEntities.push_back(world->GetEntityManager().CreateEntity(command.name, command.level, command.prefabPath));
				}
				break;
				case ECommandType::DestroyEntity:
				{
					DestroyEntityCommand command;
					buffer >> command;

					world->GetEntityManager().DestroyEntity(ResolveTransientEntity(command.entity));
				}
				break;
				case ECommandType::RenameEntity:
				{
					RenameEntityCommand command;
					buffer >> command;
					
					world->GetEntityManager().SetEntityName(ResolveTransientEntity(command.entity), command.name);
				}
				break;
				case ECommandType::MoveEntityToLevel:
				{
					MoveEntityToLevelCommand command;
					buffer >> command;

					world->GetEntityManager().MoveEntityToLevel(
						ResolveTransientEntity(command.entity),
						command.level);
				}
				break;
				case ECommandType::ReparentEntity:
				{
					ReparentEntityCommand command;
					buffer >> command;
					
					world->GetEntityManager().ReparentEntity(
						ResolveTransientEntity(command.entity), 
						ResolveTransientEntity(command.parent));
				}
				break;
				case ECommandType::MoveEntityBefore:
				{
					MoveEntityBeforeCommand command;
					buffer >> command;
					
					world->GetEntityManager().MoveEntityBefore(
						ResolveTransientEntity(command.entity), 
						ResolveTransientEntity(command.next));
				}
				break;
				case ECommandType::MoveEntityAfter:
				{
					MoveEntityAfterCommand command;
					buffer >> command;

					world->GetEntityManager().MoveEntityAfter(
						ResolveTransientEntity(command.entity), 
						ResolveTransientEntity(command.previous));
				}
				break;
				case ECommandType::AddComponent:
				{
					AddComponentCommand command;
					buffer >> command;

					world->GetEntityManager().AddComponentOfType(ResolveTransientEntity(command.entity), command.component.GetComponentType(), command.component.GetComponentPtr());
				}
				break;
				case ECommandType::RemoveComponent:
				{
					RemoveComponentCommand command;
					buffer >> command;

					world->GetEntityManager().RemoveComponentOfType(ResolveTransientEntity(command.entity), { command.componentType });
				}
				break;
				case ECommandType::QueueEvent:
				{
					QueueEventCommand command;
					buffer >> command;

					/* Use event reflection to manually resolve any transient entities found in event arguments */

					std::vector<std::pair<void*, EventVariableTypeInfo>> eventVariables = GET_EVENT_VARS(command.event.GetEventType(), command.event.GetEventPtr());
					for(const std::pair<void*, EventVariableTypeInfo>& eventVariable : eventVariables)
					{
						// If the event variable is of type Entity, cast it to Entity and resolve its permanent ID in-place
						if(eventVariable.second.variableType == typeid(Entity))
						{
							Entity& entityVariable = *static_cast<Entity*>(eventVariable.first);
							entityVariable = ResolveTransientEntity(entityVariable);
						}
					}

					world->QueueEventOfType(command.event.GetEventType(), command.event.GetEventPtr());
				}
				break;
				default: break;
			}
		}

		buffer.Clear();
	}

	Entity ResolveTransientEntity(const Entity& entity)
	{
		// If the given entity is transient (internal reference), find the permanent ID and use that instead
		if(entity.generation == 0)
			return createdEntities[entity.index];
		else
			return entity;
	}
};

template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::CreateEntityCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::CreateEntityCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::DestroyEntityCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::DestroyEntityCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::RenameEntityCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::RenameEntityCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityToLevelCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityToLevelCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::ReparentEntityCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::ReparentEntityCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityBeforeCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityBeforeCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::MoveEntityAfterCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::MoveEntityAfterCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::AddComponentCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::AddComponentCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::RemoveComponentCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::RemoveComponentCommand& dst);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Serialize(const CommandBuffer::QueueEventCommand& src);
template<>
template<>
void ByteStream<ICommandStreamFormat>::Deserialize(CommandBuffer::QueueEventCommand& dst);