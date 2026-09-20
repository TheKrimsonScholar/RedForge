#pragma once

#include "System.h"

#include "SystemRegistration.h"

#include "ReplicationState.h"

#include "NetworkComponent.h"

#include "Exports.h"

#include <enet/enet.h>

enum class ENetworkPacketType : uint8_t
{
	CreateEntity,
	DestroyEntity,
	ComponentUpdate
};

struct NetworkPacket
{
	ENetworkPacketType packetType;
	void* data;
};
struct NetworkPacket_CreateEntity
{
	Entity parent;
	//std::string name;
};

class NetworkSystem : public System<ReplicationState, IReplicatedComponent, NetworkComponent>
{
private:
	static inline NetworkSystem* Instance;

	ENetHost* host = nullptr;
	ENetPeer* server = nullptr;
	bool isConnected = false;

	std::unordered_map<uint32_t, Entity> networkEntities;

	ByteStream<IPacketStreamFormat> packetByteStream;

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

	void InitializeConnectedClient(const World& world);

	ENetPacket* CreatePacket_CreateEntity();

	void ReceivePacket_CreateEntity(LocalSystemContext& ctx, const NetworkPacket_CreateEntity* packetData);
};
REGISTER_SYSTEM_TYPE(NetworkSystem, System<ReplicationState, IReplicatedComponent, NetworkComponent>)