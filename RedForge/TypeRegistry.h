#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <typeindex>

#include "EngineParams.h"
#include "ByteStream.h"
#include "DirectedAcyclicGraph.h"

#include "Exports.h"

class World;
class EntityManager;
class SystemBase;

#define INVALID_TYPE TypeID(-1)

REDFORGE_API struct TypeID
{
	size_t id;

	bool operator==(const TypeID& other) const { return id == other.id; }
};
namespace std
{
	template<>
	struct hash<TypeID>
	{
		size_t operator()(const TypeID& typeID) const
		{
			return hash<uint32_t>()(typeID.id);
		}
	};
}

struct IComponent {};
struct IReplicatedComponent : public IComponent {};

struct IResource
{
public:
	virtual ~IResource() = default;

protected:
	virtual void Startup(const EngineStartupParams& params, World& world) = 0;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) = 0;

	friend World;
};

struct IEvent {};

struct TypeInfo
{
	std::string name;
	size_t size;

	std::function<void*()> constructInstance;
	std::function<void*(const void*)> copyConstructInstance;
	std::function<void(const void*, void*)> copyInstance;
	std::function<void(void*)> destroyInstance;
	std::function<void(ByteStreamBase&, void*)> serialize;
	std::function<void(ByteStreamBase&, void*)> deserialize;
	std::function<void(ByteStream<IPacketStreamFormat>&, void*)> serializePacket;
	std::function<void(ByteStream<IPacketStreamFormat>&, void*)> deserializePacket;
};
struct SystemTypeInfo
{
	std::function<SystemBase*()> createSystem;

	std::vector<std::type_index> writeList;
	std::vector<std::type_index> readList;
};
struct ComponentVariableTypeInfo
{
	std::type_index variableType;
	std::string variableName;
	bool isReplicated;

	std::function<std::istream& (std::istream&, void*)> readFromFile;
	std::function<std::ostream& (std::ostream&, void*)> writeToFile;
};
struct ComponentTypeInfo
{
	std::function<void(EntityManager&)> registerComponent;

	std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)> getVariables;

	std::vector<ComponentVariableTypeInfo> variables;

	ComponentTypeInfo() = default;
	ComponentTypeInfo(std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)> getVariables) : getVariables(getVariables) {};
	~ComponentTypeInfo() = default;
};
struct ResourceVariableTypeInfo
{
	std::type_index variableType;
	std::string variableName;
	bool isReplicated;

	std::function<std::istream& (std::istream&, void*)> readFromFile;
	std::function<std::ostream& (std::ostream&, void*)> writeToFile;
};
struct ResourceTypeInfo
{
	ResourceTypeInfo() = default;
	ResourceTypeInfo(std::function<std::vector<std::pair<void*, ResourceVariableTypeInfo>>(void*)> getVariables) :
		getVariables(getVariables) {};

	std::function<IResource*()> createResource;

	std::function<std::vector<std::pair<void*, ResourceVariableTypeInfo>>(void*)> getVariables;
};
struct EventVariableTypeInfo
{
	std::type_index variableType;
	std::string variableName;
};
struct EventTypeInfo
{
	EventTypeInfo() = default;
	EventTypeInfo(std::function<std::vector<std::pair<void*, EventVariableTypeInfo>>(void*)> getVariables) :
		getVariables(getVariables) {};

	std::function<class IEventQueue*()> createQueue;

	std::function<std::vector<std::pair<void*, EventVariableTypeInfo>>(void*)> getVariables;
};

class RegisterTypeBase;
class TypeRegistry
{
private:
	struct TypeRegistryDestructor
	{
		TypeRegistryDestructor();
		~TypeRegistryDestructor();
	};
	// Static object used to tie type registry destruction to program termination.
	// Because TypeRegistry follows the Meyer's Singleton pattern, it is created on first use and therefore must be destroyed by an external object when static objects are destroyed on shutdown, but this lifetime shouldn't be managed by the engine since the type registry may be used before the engine is initialized.
	static TypeRegistryDestructor destructor;

	REDFORGE_API static TypeRegistry* instance;

	std::unordered_map<std::type_index, RegisterTypeBase*> registrars;
	DAG<std::type_index> typeIndexHierarchy;

public:
	std::unordered_map<std::type_index, TypeID> registeredTypesMap;
	DAG<TypeID> typeHierarchy;

	// General type info
	std::vector<TypeInfo> registeredTypesList;

	// System type info
	std::vector<TypeID> registeredSystemsList;
	std::unordered_map<TypeID, SystemTypeInfo> registeredSystemInfoMap;
	DAG<TypeID> systemDependencyHierarchy;

	// Component type info
	std::vector<TypeID> registeredComponentsList;
	std::unordered_map<TypeID, ComponentTypeInfo> registeredComponentInfoMap;
	DAG<TypeID> componentDependencyHierarchy;

	// Resource type info
	std::vector<TypeID> registeredResourcesList;
	std::unordered_map<TypeID, ResourceTypeInfo> registeredResourceInfoMap;
	DAG<TypeID> resourceDependencyHierarchy;

	// Event type info
	std::vector<TypeID> registeredEventsList;
	std::unordered_map<TypeID, EventTypeInfo> registeredEventInfoMap;

private:
	template<typename T, typename... Bases>
	void AddTypeToRegistry(RegisterTypeBase* registrar);

	void RegisterAllTypes();

	static void DestroyInstance();
	
	REDFORGE_API static TypeRegistry& GetInstance();

	friend class RegisterTypeBase;
	friend class Engine;

public:
	REDFORGE_API static const DAG<TypeID>& GetTypeHierarchy() { return GetInstance().typeHierarchy; }
	
	REDFORGE_API static const std::vector<TypeInfo>& GetRegisteredTypesList() { return GetInstance().registeredTypesList; }
	REDFORGE_API static const std::unordered_map<std::type_index, TypeID>& GetRegisteredTypesMap() { return GetInstance().registeredTypesMap; }
	static inline bool IsTypeRegistered(const std::type_index& type)
	{
		return GetRegisteredTypesMap().find(type) != GetRegisteredTypesMap().end();
	}

	REDFORGE_API static const std::vector<TypeID>& GetRegisteredSystemsList() { return GetInstance().registeredSystemsList; }
	REDFORGE_API static const std::unordered_map<TypeID, SystemTypeInfo>& GetRegisteredSystemInfoMap() { return GetInstance().registeredSystemInfoMap; }
	REDFORGE_API static const DAG<TypeID>& GetSystemDependencyHierarchy() { return GetInstance().systemDependencyHierarchy; }
	static inline bool IsSystemTypeRegistered(const std::type_index& type)
	{
		if(!IsTypeRegistered(type))
			return false;

		return std::find(GetRegisteredSystemsList().begin(), GetRegisteredSystemsList().end(), GetTypeID(type)) != GetRegisteredSystemsList().end();
	}

	REDFORGE_API static const std::vector<TypeID>& GetRegisteredComponentsList() { return GetInstance().registeredComponentsList; }
	REDFORGE_API static const std::unordered_map<TypeID, ComponentTypeInfo>& GetRegisteredComponentInfoMap() { return GetInstance().registeredComponentInfoMap; }
	REDFORGE_API static const DAG<TypeID>& GetComponentDependencyHierarchy() { return GetInstance().componentDependencyHierarchy; }
	static inline bool IsComponentTypeRegistered(const std::type_index& type)
	{
		if(!IsTypeRegistered(type))
			return false;

		return std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), GetTypeID(type)) != GetRegisteredComponentsList().end();
	}

	REDFORGE_API static const std::vector<TypeID>& GetRegisteredResourcesList() { return GetInstance().registeredResourcesList; }
	REDFORGE_API static const std::unordered_map<TypeID, ResourceTypeInfo>& GetRegisteredResourceInfoMap() { return GetInstance().registeredResourceInfoMap; }
	REDFORGE_API static const DAG<TypeID>& GetResourceDependencyHierarchy() { return GetInstance().resourceDependencyHierarchy; }
	static inline bool IsResourceTypeRegistered(const std::type_index& type)
	{
		if(!IsTypeRegistered(type))
			return false;

		return std::find(GetRegisteredResourcesList().begin(), GetRegisteredResourcesList().end(), GetTypeID(type)) != GetRegisteredResourcesList().end();
	}

	REDFORGE_API static const std::vector<TypeID>& GetRegisteredEventsList() { return GetInstance().registeredEventsList; }
	REDFORGE_API static const std::unordered_map<TypeID, EventTypeInfo>& GetRegisteredEventInfoMap() { return GetInstance().registeredEventInfoMap; }
	static inline bool IsEventTypeRegistered(const std::type_index& type)
	{
		if(!IsTypeRegistered(type))
			return false;

		return std::find(GetRegisteredEventsList().begin(), GetRegisteredEventsList().end(), GetTypeID(type)) != GetRegisteredEventsList().end();
	}

	template<typename T>
	static inline TypeID GetTypeID()
	{
		return GetTypeID(typeid(T));
	}
	static inline TypeID GetTypeID(const std::type_index& type)
	{
		return GetRegisteredTypesMap().at(type);
	}
};

#define TYPEID(Type) ((TypeID) TypeRegistry::GetTypeID<Type>())
#define GET_TYPE_NAME(typeID) ((std::string) TypeRegistry::GetRegisteredTypesList()[typeID.id].name)
#define GET_TYPE_SIZE(typeID) ((size_t) TypeRegistry::GetRegisteredTypesList()[typeID.id].size)
#define CONSTRUCT(typeID) ((std::function<void*()>) TypeRegistry::GetRegisteredTypesList()[typeID.id].constructInstance)
#define CONSTRUCT_COPY(typeID) ((std::function<void*(const void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].copyConstructInstance)
#define COPY(typeID) ((std::function<void(const void*, void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].copyInstance)
#define DESTROY(typeID) ((std::function<void(void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].destroyInstance)
#define SERIALIZE(typeID) ((std::function<void(ByteStreamBase&, void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].serialize)
#define DESERIALIZE(typeID) ((std::function<void(ByteStreamBase&, void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].deserialize)
#define SERIALIZE_TO_PACKET(typeID) ((std::function<void(ByteStream<IPacketStreamFormat>&, void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].serializePacket)
#define DESERIALIZE_FROM_PACKET(typeID) ((std::function<void(ByteStream<IPacketStreamFormat>&, void*)>) TypeRegistry::GetRegisteredTypesList()[typeID.id].deserializePacket)
#define IS_TYPE_DERIVED_FROM(typeID, baseTypeID) TypeRegistry::GetTypeHierarchy().IsNodeDownstreamFrom(typeID, baseTypeID)
#define GET_TYPES_DERIVED_FROM(typeID) ((std::unordered_set<TypeID>) TypeRegistry::GetTypeHierarchy().GetAllDescendants(typeID))
#define GET_ALL_TYPES ((std::vector<TypeID>) TypeRegistry::GetTypeHierarchy().GetTopologicallySortedListBFS())

#define GET_SYSTEM_DEPENDENCIES(systemTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetSystemDependencyHierarchy().GetInValuesOfNode(systemTypeID))
#define GET_SYSTEM_DEPENDENTS(systemTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetSystemDependencyHierarchy().GetOutValuesOfNode(systemTypeID))
#define GET_SYSTEM_WRITELIST(systemTypeID) ((std::vector<std::type_index>) TypeRegistry::GetRegisteredSystemInfoMap().at(systemTypeID).writeList)
#define GET_SYSTEM_READLIST(systemTypeID) ((std::vector<std::type_index>) TypeRegistry::GetRegisteredSystemInfoMap().at(systemTypeID).readList)
#define GET_ALL_SYSTEM_TYPES ((std::vector<TypeID>) TypeRegistry::GetSystemDependencyHierarchy().GetTopologicallySortedListBFS())

#define GET_COMPONENT_DEPENDENCIES(componentTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetComponentDependencyHierarchy().GetInValuesOfNode(componentTypeID))
#define GET_COMPONENT_DEPENDENTS(componentTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetComponentDependencyHierarchy().GetOutValuesOfNode(componentTypeID))
// Returns a vector of void*-ComponentVariableInfo pairs indicating the location of variables and their type/name data
#define GET_COMPONENT_VARS(componentTypeID, componentPtr) ((std::vector<std::pair<void*, ComponentVariableTypeInfo>>) TypeRegistry::GetRegisteredComponentInfoMap().at(componentTypeID).getVariables(componentPtr))
#define GET_COMPONENT_VAR_INFO(componentTypeID) ((std::vector<ComponentVariableTypeInfo>) TypeRegistry::GetRegisteredComponentInfoMap().at(componentTypeID).variables)
#define GET_ALL_COMPONENT_TYPES ((std::vector<TypeID>) TypeRegistry::GetComponentDependencyHierarchy().GetTopologicallySortedListBFS())

#define GET_RESOURCE_DEPENDENCIES(resourceTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetResourceDependencyHierarchy().GetInValuesOfNode(resourceTypeID))
#define GET_RESOURCE_DEPENDENTS(resourceTypeID) ((std::unordered_set<TypeID>) TypeRegistry::GetResourceDependencyHierarchy().GetOutValuesOfNode(resourceTypeID))
// Returns a vector of void*-ResourceVariableTypeInfo pairs indicating the location of variables and their type/name data
#define GET_RESOURCE_VARS(resourceTypeID, resourcePtr) ((std::vector<std::pair<void*, ResourceVariableTypeInfo>>) TypeRegistry::GetRegisteredResourceInfoMap().at(resourceTypeID).getVariables(resourcePtr))
#define GET_ALL_RESOURCE_TYPES ((std::vector<TypeID>) TypeRegistry::GetResourceDependencyHierarchy().GetTopologicallySortedListBFS())

// Returns a vector of void*-EventVariableInfo pairs indicating the location of variables and their type/name data
#define GET_EVENT_VARS(eventTypeID, eventPtr) ((std::vector<std::pair<void*, EventVariableTypeInfo>>) TypeRegistry::GetRegisteredEventInfoMap().at(eventTypeID).getVariables(eventPtr))

template<typename T, typename ...Bases>
inline void TypeRegistry::AddTypeToRegistry(RegisterTypeBase* registrar)
{
	std::vector<std::type_index> bases = { typeid(Bases)... };

	registrars.emplace(typeid(T), registrar);
	typeIndexHierarchy.AddNode(typeid(T));
	for(const std::type_index& base : bases)
		typeIndexHierarchy.AddConnection(base, typeid(T));
}