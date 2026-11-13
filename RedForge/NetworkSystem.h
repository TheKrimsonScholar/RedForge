#pragma once

#include "System.h"

#include "SystemRegistration.h"
#include "GenericComponentData.h"
#include "ComponentRef.h"

#include "ReplicationState.h"

#include "NetworkComponent.h"

#include "Exports.h"

#include <enet/enet.h>

enum class ENetworkPacketType : uint8_t
{
	LoadLevel,
	HierarchyEvents,
	StateReplication,
	StateReplicationACK,
};

struct ResourceReplicationData
{
	TypeID resource;
	std::vector<uint8_t> resourceData; // Serialized replication data for this resource
};
struct ComponentReplicationData
{
	ComponentNetworkRef componentRef;
	std::vector<uint8_t> componentData; // Serialized replication data for this component
};

struct NetworkPacketHeader
{
	ENetworkPacketType packetType;
	uint32_t dataSize;
};

enum class EHierarchyEventType : uint8_t
{
	EntityAddedToNetwork,
	EntityRemovedFromNetwork,
	EntityMoved,
	ComponentAdded,
	ComponentRemoved
};
struct HierarchyEventData_EntityAddedToNetwork
{
	size_t entity;
	size_t level;
	size_t parent;
	size_t previous;
	size_t next;
	std::string name;
	std::vector<GenericComponentData> componentData;
};
struct HierarchyEventData_EntityRemovedFromNetwork
{
	size_t entity;
};
struct HierarchyEventData_EntityMoved
{
	size_t entity;
	size_t level;
	size_t parent;
	size_t previous;
	size_t next;
};
struct HierarchyEventData_ComponentAdded
{
	size_t entity;
	GenericComponentData componentData;
};
struct HierarchyEventData_ComponentRemoved
{
	size_t entity;
	TypeID componentType;
};

struct NetworkPacketData_LoadLevel
{
	
};
struct NetworkPacketData_StateReplication
{
	size_t currentNetworkTick;
	std::vector<ResourceReplicationData> resourceReplicationData;
	std::vector<ComponentReplicationData> componentReplicationData;
};
struct NetworkPacketData_StateReplicationACK
{
	size_t acknowledgedTick;
};

class NetworkSystem : public System<const IComponent, ReplicationState, IReplicatedComponent, NetworkComponent>
{
private:
	static inline NetworkSystem* Instance;

	ENetHost* host = nullptr;
	ENetPeer* server = nullptr;
	std::unordered_set<ENetPeer*> clients;
	bool isConnected = false;

	size_t currentNetworkTick = 1;
	std::unordered_map<ENetPeer*, size_t> clientLastTickAcknowledged;

#ifdef NETMODE_SERVER
	size_t nextNetworkID = 0;
	std::priority_queue<size_t> availableNetworkIDs;
#endif // NETMODE_SERVER

	std::unordered_map<size_t, Entity> networkEntities;
	std::unordered_map<Entity, size_t> networkIDs;

	ByteStream<IPacketStreamFormat> packetByteStream;

	std::unordered_map<ENetPeer*, std::unordered_set<TypeID>> pendingClientResourceChanges;
	std::unordered_map<ENetPeer*, std::unordered_set<ComponentNetworkRef>> pendingClientComponentChanges;

	std::unordered_map<TypeID, size_t> resourceTickLastChanged;
	std::unordered_map<ComponentNetworkRef, size_t> componentTickLastChanged;

public:
	NetworkSystem() {};
	~NetworkSystem() {};

	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void PostStartup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

	REDFORGE_API void Update(LocalSystemContext& ctx, float deltaTime) override;

	bool InitializeServer();
	bool ConnectToHost(std::string hostAddress, World& world);
	void Disconnect();

	void ServerAddClient(ENetPeer* client);
	void ServerRemoveClient(ENetPeer* client);

	void InitializeConnectedClient(const World& world);

#ifdef NETMODE_SERVER
	void SendHierarchyEventsPackets(LocalSystemContext& ctx);
	void SendReplicationPackets(LocalSystemContext& ctx);

	void ServerWriteAddEntityPacket(LocalSystemContext& ctx, const Entity& entity);
#endif // NETMODE_SERVER
	
	void SendPacket_StateReplicationACK(ENetPeer* recipient, const NetworkPacketData_StateReplicationACK& packetData);
	void SendPacket(ENetPeer* recipient, ENetworkPacketType packetType, uint32_t packetDataSize, const void* packetData, enet_uint32 flags = 0);

	void ReceivePacket(LocalSystemContext& ctx, ENetPeer* sender, const void* packet);
	void ReceivePacket_LoadLevel(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_LoadLevel& packetData);
	void ReceivePacket_HierarchyEvents(LocalSystemContext& ctx, ENetPeer* sender);
	void ReceivePacket_StateReplication(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_StateReplication& packetData);
	void ReceivePacket_StateReplicationACK(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_StateReplicationACK& packetData);

	Entity GetEntityByNetworkID(size_t networkID);
	size_t GetNetworkIDOfEntity(const Entity& entity);

	bool IsEntityInNetwork(const Entity& entity);

private:
#ifdef NETMODE_SERVER
	// Server-specific function that adds an existing entity to the network. Assumes entity is valid and already has a NetworkComponent. Returns the network ID of the entity.
	size_t ServerAddEntityToNetwork(LocalSystemContext& ctx, const Entity& entity);
	void ServerRemoveEntityFromNetwork(LocalSystemContext& ctx, const Entity& entity);
#endif // NETMODE_SERVER
};

template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityAddedToNetwork& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityAddedToNetwork& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityRemovedFromNetwork& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityRemovedFromNetwork& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityMoved& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityMoved& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_ComponentAdded& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_ComponentAdded& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_ComponentRemoved& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_ComponentRemoved& dst);

template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const NetworkPacketData_StateReplication& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(NetworkPacketData_StateReplication& dst);

template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const ResourceReplicationData& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(ResourceReplicationData& dst);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const ComponentReplicationData& src);
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(ComponentReplicationData& dst);