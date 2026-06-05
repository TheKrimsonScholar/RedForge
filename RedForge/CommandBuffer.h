#pragma once

#include <vector>
#include <iostream>

#include "World.h"
#include "EntityManager.h"
#include "EventQueue.h"

class CommandBuffer
{
	enum class ECommandType : uint8_t
	{
		CreateEntity,
		DestroyEntity,
		RenameEntity,
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
		Entity parent;
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
		std::type_index componentType;
		//void* component;
	};
	struct RemoveComponentCommand
	{
		Entity entity;
		std::type_index componentType;
	};
	struct QueueEventCommand
	{
		std::type_index eventType;
		//void* event;
	};

private:
	World* world;

	std::vector<std::byte> buffer;
	
	size_t writePosition = 0;
	size_t readPosition = 0;

	size_t entitiesCreated = 0;
	std::vector<Entity> createdEntities;

public:
	CommandBuffer(World* world) : world(world) {}

	Entity Command_CreateEntity(const std::string& name, const Entity& parent, const std::filesystem::path& prefabPath)
	{
		CreateEntityCommand command = { name, parent, prefabPath };
		WriteCommand(ECommandType::CreateEntity, &command);

		Entity tempEntity;
		tempEntity.index = entitiesCreated++;
		tempEntity.generation = 0; // Set generation to 0 to indicate a transient entity

		return tempEntity;
	}
	void Command_DestroyEntity(const Entity& entity)
	{
		DestroyEntityCommand command = { entity };
		WriteCommand(ECommandType::DestroyEntity, &command);
	}
	void Command_RenameEntity(const Entity& entity, const std::string& name)
	{
		RenameEntityCommand command = { entity, name };
		WriteCommand(ECommandType::RenameEntity, &command);
	}
	void Command_ReparentEntity(const Entity& entity, const Entity& parent)
	{
		ReparentEntityCommand command = { entity, parent };
		WriteCommand(ECommandType::ReparentEntity, &command);
	}
	void Command_MoveEntityBefore(const Entity& entity, const Entity& next)
	{
		MoveEntityBeforeCommand command = { entity, next };
		WriteCommand(ECommandType::MoveEntityBefore, &command);
	}
	void Command_MoveEntityAfter(const Entity& entity, const Entity& previous)
	{
		MoveEntityAfterCommand command = { entity, previous };
		WriteCommand(ECommandType::MoveEntityAfter, &command);
	}
	void Command_AddComponent(const Entity& entity, std::type_index componentType, void* component)
	{
		AddComponentCommand command = { entity, componentType };

		// Write command with the component as extra data
		WriteCommand(ECommandType::AddComponent, &command, GET_COMPONENT_SIZE(componentType), component);
	}
	void Command_RemoveComponent(const Entity& entity, std::type_index componentType)
	{
		RemoveComponentCommand command = { entity, componentType };
		WriteCommand(ECommandType::RemoveComponent, &command);
	}
	void Command_QueueEvent(std::type_index eventType, const void* event)
	{
		QueueEventCommand command = { eventType };

		// Write command with the event as extra data
		WriteCommand(ECommandType::QueueEvent, &command, GET_EVENT_SIZE(eventType), event);
	}

	void Flush()
	{
		ECommandType commandType;
		void* data;
		size_t extraBytes;
		void* extra;
		while(ReadCommand(commandType, data, extraBytes, extra))
		{
			switch(commandType)
			{
				case ECommandType::CreateEntity:
				{
					CreateEntityCommand* command = reinterpret_cast<CreateEntityCommand*>(data);

					// Create the entity and add it to internal list for future reference
					createdEntities.push_back(world->GetEntityManager().CreateEntity(command->parent, command->name, command->prefabPath));
				}
				break;
				case ECommandType::DestroyEntity:
				{
					DestroyEntityCommand* command = reinterpret_cast<DestroyEntityCommand*>(data);
					
					world->GetEntityManager().DestroyEntity(ResolveTransientEntity(command->entity));
				}
				break;
				case ECommandType::RenameEntity:
				{
					RenameEntityCommand* command = reinterpret_cast<RenameEntityCommand*>(data);
					
					world->GetEntityManager().SetEntityName(ResolveTransientEntity(command->entity), command->name);
				}
				break;
				case ECommandType::ReparentEntity:
				{
					ReparentEntityCommand* command = reinterpret_cast<ReparentEntityCommand*>(data);
					
					world->GetEntityManager().ReparentEntity(
						ResolveTransientEntity(command->entity), 
						ResolveTransientEntity(command->parent));
				}
				break;
				case ECommandType::MoveEntityBefore:
				{
					MoveEntityBeforeCommand* command = reinterpret_cast<MoveEntityBeforeCommand*>(data);
					
					world->GetEntityManager().MoveEntityBefore(
						ResolveTransientEntity(command->entity), 
						ResolveTransientEntity(command->next));
				}
				break;
				case ECommandType::MoveEntityAfter:
				{
					MoveEntityAfterCommand* command = reinterpret_cast<MoveEntityAfterCommand*>(data);

					world->GetEntityManager().MoveEntityAfter(
						ResolveTransientEntity(command->entity), 
						ResolveTransientEntity(command->previous));
				}
				break;
				case ECommandType::AddComponent:
				{
					AddComponentCommand* command = reinterpret_cast<AddComponentCommand*>(data);
					void* component = extra;

					world->GetEntityManager().AddComponentOfType(ResolveTransientEntity(command->entity), command->componentType, component);
				}
				break;
				case ECommandType::RemoveComponent:
				{
					RemoveComponentCommand* command = reinterpret_cast<RemoveComponentCommand*>(data);

					world->GetEntityManager().RemoveComponentOfType(ResolveTransientEntity(command->entity), command->componentType);
				}
				break;
				case ECommandType::QueueEvent:
				{
					QueueEventCommand* command = reinterpret_cast<QueueEventCommand*>(data);
					void* event = extra;

					/* Use event reflection to manually resolve any transient entities found in event arguments */

					std::vector<std::pair<void*, EventVariableInfo>> eventVariables = GET_EVENT_VARS(command->eventType, event);
					for(const std::pair<void*, EventVariableInfo>& eventVariable : eventVariables)
					{
						// If the event variable is of type Entity, cast it to Entity and resolve its permanent ID in-place
						if(eventVariable.second.variableType == typeid(Entity))
						{
							Entity& entityVariable = *static_cast<Entity*>(eventVariable.first);
							entityVariable = ResolveTransientEntity(entityVariable);
						}
					}

					world->QueueEventOfType(command->eventType, event);
				}
				break;
				default: break;
			}
		}

		writePosition = 0;
		readPosition = 0;
		buffer.clear();
	}

private:
	void WriteCommand(ECommandType commandType, const void* data, size_t extraBytes = 0, const void* extra = nullptr)
	{
		size_t dataSize = GetCommandSize(commandType);

		size_t totalCommandSize = sizeof(ECommandType) + dataSize + sizeof(size_t) + extraBytes;
		if(buffer.size() < writePosition + totalCommandSize)
			buffer.resize(writePosition + totalCommandSize);

		// Write command type
		memcpy(buffer.data() + writePosition, &commandType, sizeof(ECommandType));
		writePosition += sizeof(ECommandType);
		// Write command data
		memcpy(buffer.data() + writePosition, data, dataSize);
		writePosition += dataSize;
		// Write extra data size
		memcpy(buffer.data() + writePosition, &extraBytes, sizeof(size_t));
		writePosition += sizeof(size_t);
		// Write extra data (optional)
		if(extraBytes > 0 && extra)
		{
			memcpy(buffer.data() + writePosition, extra, extraBytes);
			writePosition += extraBytes;
		}
	}
	bool ReadCommand(ECommandType& out_type, void*& out_data, size_t& out_extraBytes, void*& out_extra)
	{
		if(readPosition >= writePosition)
			return false;

		// Read command type
		out_type = *reinterpret_cast<ECommandType*>(buffer.data() + readPosition);
		readPosition += sizeof(ECommandType);
		
		size_t dataSize = GetCommandSize(out_type);
		// Read command data
		out_data = buffer.data() + readPosition;
		readPosition += dataSize;

		// Read extra data size
		out_extraBytes = *reinterpret_cast<size_t*>(buffer.data() + readPosition);
		readPosition += sizeof(size_t);
		// Read extra data (if applicable)
		if(out_extraBytes > 0)
		{
			out_extra = buffer.data() + readPosition;
			readPosition += out_extraBytes;
		}
		else
			out_extra = nullptr;

		return true;
	}

	Entity ResolveTransientEntity(const Entity& entity)
	{
		// If the given entity is transient (internal reference), find the permanent ID and use that instead
		if(entity.generation == 0)
			return createdEntities[entity.index];
		else
			return entity;
	}

	size_t GetCommandSize(ECommandType commandType) const
	{
		switch(commandType)
		{
			case ECommandType::CreateEntity: return sizeof(CreateEntityCommand);
			case ECommandType::DestroyEntity: return sizeof(DestroyEntityCommand);
			case ECommandType::RenameEntity: return sizeof(RenameEntityCommand);
			case ECommandType::ReparentEntity: return sizeof(ReparentEntityCommand);
			case ECommandType::MoveEntityBefore: return sizeof(MoveEntityBeforeCommand);
			case ECommandType::MoveEntityAfter: return sizeof(MoveEntityAfterCommand);
			case ECommandType::AddComponent: return sizeof(AddComponentCommand);
			case ECommandType::RemoveComponent: return sizeof(RemoveComponentCommand);
			case ECommandType::QueueEvent: return sizeof(QueueEventCommand);
			default: return 0;
		}
	}
};