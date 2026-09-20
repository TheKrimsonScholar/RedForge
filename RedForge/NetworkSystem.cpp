#include "NetworkSystem.h"

#include <iostream>

#include "NetworkComponent.h"

void NetworkSystem::Startup(const EngineStartupParams& params, World& world)
{
    #ifdef NETMODE_SERVER
    InitializeServer();
    #endif // NETMODE_SERVER
    #ifdef NETMODE_CLIENT
    if(params.ShouldConnectToServer())
        ConnectToHost(params.startupLevelPath.string(), world);
    #endif // NETMODE_CLIENT
}
void NetworkSystem::PostStartup(const EngineStartupParams& params, World& world)
{

}
void NetworkSystem::Shutdown(const EngineShutdownParams& params, World& world)
{
    Disconnect();
}

void NetworkSystem::Update(LocalSystemContext& ctx, float deltaTime)
{
    if(!isConnected)
        return;

    #ifdef NETMODE_SERVER
    // Replication logic

    // Iterate through all replicated component types and query for entities with that component and network component
    for(TypeID replicatedComponentType : GET_TYPES_DERIVED_FROM(TYPEID(IReplicatedComponent)))
    {
        ctx.ForEachComponentOfType({ TypeRegistry::GetTypeID<NetworkComponent>(), replicatedComponentType },
            [this, replicatedComponentType](const Entity& entity, std::span<void*> components) -> void
            {
                //std::string msg = "awd";
				//ENetPacket* replicationPacket = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
                //enet_host_broadcast(host, 0, replicationPacket);
                //enet_host_flush(host);
                
                //int k = 0;
                //REPLICATE_COMPONENT(replicatedComponentType);
            });
    }
    #endif // NETMODE_SERVER

    #ifdef NETMODE_SERVER
    ENetEvent event;
    while(enet_host_service(host, &event, 0) > 0) // 0ms timeout; non-blocking
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Client connected from " << event.peer->address.host << "\n";
				/*ctx.ForEachEntity([this](const Entity& entity)
					{
						if(!ctx.HasComponent<NetworkComponent>(entity))
							ctx.AddComponent<NetworkComponent>(entity, NetworkComponent());
					});*/
                
                /*{
                    std::string msg = "awd";
                    ENetPacket* replicationPacket = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
                    enet_host_broadcast(host, 0, replicationPacket);
                    enet_host_flush(host);
                }*/

                {
                    ENetPacket* packet = CreatePacket_CreateEntity();
                    enet_host_broadcast(host, 0, packet);
                    enet_host_flush(host);
                }
                
                ctx.CreateEntity({}, "Hello");
                //InitializeConnectedClient();
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "Message from client: " << (char*) event.packet->data << "\n";
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected.\n";
                break;
            default: break;
        }
    }
    #endif // NETMODE_SERVER
    #ifdef NETMODE_CLIENT
    ENetEvent event;
    while(enet_host_service(host, &event, 0) > 0) // 0ms timeout; non-blocking
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                const NetworkPacket* packet = reinterpret_cast<NetworkPacket*>(event.packet->data);
                switch(packet->packetType)
                {
				    case ENetworkPacketType::CreateEntity:
                        ReceivePacket_CreateEntity(ctx, reinterpret_cast<NetworkPacket_CreateEntity*>(packet->data));
					    break;
                }
                std::cout << "Server says: " << (char*) event.packet->data << "\n";
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server.\n";
                break;
            default: break;
        }
    }
    #endif // NETMODE_CLIENT
}

bool NetworkSystem::InitializeServer()
{
    if(isConnected)
        return false;

    #ifdef NETMODE_SERVER
    if(enet_initialize() != 0)
    {
        std::cerr << "Failed to initialize ENet.\n";
        return false;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY; // Listen on all interfaces
    address.port = 1234;

    host = enet_host_create(&address, 32, 2, 0, 0);
    if(!host)
    {
        std::cerr << "Failed to create ENet server.\n";
        return false;
    }

    std::cout << "Server started on port 1234...\n";
    #endif // NETMODE_SERVER

    isConnected = true;

    return true;
}
bool NetworkSystem::ConnectToHost(std::string hostAddress, World& world)
{
    if(isConnected)
        return false;

    #ifdef NETMODE_CLIENT
    if(enet_initialize() != 0)
    {
        std::cerr << "Failed to initialize ENet.\n";
        return false;
    }
    atexit(enet_deinitialize);

    host = enet_host_create(nullptr, 1, 2, 0, 0);
    if(!host)
    {
        std::cerr << "Failed to create ENet client.\n";
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, hostAddress.c_str());
    address.port = 1234;

    server = enet_host_connect(host, &address, 2, 0);
    if(!server)
    {
        std::cerr << "No available servers.\n";
        return false;
    }

    ENetEvent event;
    if(enet_host_service(host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connected to server!\n";

        std::string msg = "Hello from client!";
        ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, packet);
        enet_host_flush(host);

        // Clear all active entities so the client can synchronize with the server's current world state
        world.GetEntityManager().UnloadAllLevels();
    }
    else
    {
        std::cerr << "Connection to server failed.\n";
        enet_peer_reset(server);
    }
    #endif // NETMODE_CLIENT

    isConnected = true;

    return true;
}
void NetworkSystem::Disconnect()
{
    if(!isConnected)
        return;

    #ifdef NETMODE_SERVER
    enet_host_destroy(host);
    #endif // NETMODE_SERVER
    #ifdef NETMODE_CLIENT
    enet_host_destroy(host);
    #endif // NETMODE_CLIENT

    isConnected = false;
}

void NetworkSystem::InitializeConnectedClient(const World& world)
{
    auto k = 1;
}

ENetPacket* NetworkSystem::CreatePacket_CreateEntity()
{
    //std::string msg = "Hello from client!";
    //return enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);

    NetworkPacket_CreateEntity packetData = {};
    packetData.parent = {};
    //packetData.name = "Hello";

	/*NetworkPacket packet = {};
	packet.packetType = ENetworkPacketType::CreateEntity;
    memcpy(&packet.data, &packetData, sizeof(NetworkPacket_CreateEntity));
	packet.data = &packetData;

    return enet_packet_create(&packet, sizeof(ENetworkPacketType) + sizeof(NetworkPacket_CreateEntity), ENET_PACKET_FLAG_RELIABLE);*/

    char* packet = (char*) malloc(sizeof(ENetworkPacketType) + sizeof(NetworkPacket_CreateEntity));
    if(!packet)
        return nullptr;
    
	ENetworkPacketType packetType = ENetworkPacketType::CreateEntity;
    memcpy(packet, &packetType, sizeof(ENetworkPacketType));
    memcpy(packet + sizeof(ENetworkPacketType), &packetData, sizeof(NetworkPacket_CreateEntity));

    ENetPacket* packetToSend = enet_packet_create(packet, sizeof(ENetworkPacketType) + sizeof(NetworkPacket_CreateEntity), ENET_PACKET_FLAG_RELIABLE);

    free(packet);

    int k1 = 4;
    packetByteStream << k1;
    std::string k2 = "Hello";
    packetByteStream << k2;

	int dst1;
    packetByteStream >> dst1;
    std::string dst2;
    packetByteStream >> dst2;

    packetByteStream.Clear();

    return packetToSend;
}

void NetworkSystem::ReceivePacket_CreateEntity(LocalSystemContext& ctx, const NetworkPacket_CreateEntity* packetData)
{
	ctx.CreateEntity(packetData->parent, "K");
}