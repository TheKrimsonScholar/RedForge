#include "NetworkSystem.h"

#include <iostream>

#include "NetworkDirtyTracker.h"

#include "NetworkComponent.h"

#include "DebugMacros.h"

REGISTER_SYSTEM_TYPE(NetworkSystem, System<ReplicationState, IReplicatedComponent, NetworkComponent>)

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

    //TransformComponent transform = {};
    //transform.location = { 1, 2, 3 };
    //transform.rotation = { 9, 10, 11, 12.6f };
    //transform.scale = { 4, 5, 6 };
    //ENetPacket* packet = CreatePacket_UpdateComponent({ 7, 4 }, TYPEID(TransformComponent), &transform);

    // Iterate through all replicated component types and query for entities with that component and network component
    //for(TypeID replicatedComponentType : GET_TYPES_DERIVED_FROM(TYPEID(IReplicatedComponent)))
    //{
    //    ctx.ForEachComponentOfType({ TYPEID(NetworkComponent), replicatedComponentType },
    //        [this, replicatedComponentType](const Entity& entity, std::span<void*> components) -> void
    //        {
    //            //std::string msg = "awd";
				////ENetPacket* replicationPacket = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    //            //enet_host_broadcast(host, 0, replicationPacket);
    //            //enet_host_flush(host);
    //            
    //            //int k = 0;
    //            //REPLICATE_COMPONENT(replicatedComponentType);
    //        });
    //}

    SendHierarchyEventsPackets(ctx);
    SendReplicationPackets(ctx);
    #endif // NETMODE_SERVER

    #ifdef NETMODE_SERVER
    ENetEvent event;
    while(enet_host_service(host, &event, 0) > 0) // 0ms timeout; non-blocking
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Client connected from " << event.peer->address.host << "\n";
                ServerAddClient(event.peer);
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
                    //ENetPacket* packet = CreatePacket_CreateEntity();
                    //enet_host_broadcast(host, 0, packet);
                    //enet_host_flush(host);
                }
                
                //ctx.CreateEntity({}, "Hello");
                //InitializeConnectedClient();
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                //std::cout << "Message from client: " << (char*) event.packet->data << "\n";

                ReceivePacket(ctx, event.peer, event.packet->data);
                
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected.\n";
                ServerRemoveClient(event.peer);
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
                //std::cout << "Server says: " << (char*) event.packet->data << "\n";

                ReceivePacket(ctx, event.peer, event.packet->data);

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

    currentNetworkTick++;
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

        //std::string msg = "Hello from client!";
        //ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        //enet_peer_send(server, 0, packet);
        //enet_host_flush(host);

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

void NetworkSystem::ServerAddClient(ENetPeer* client)
{
    clients.insert(client);
    clientLastTickAcknowledged.emplace(client, 0);
    pendingClientResourceChanges.emplace(client, std::unordered_set<TypeID>());
    pendingClientComponentChanges.emplace(client, std::unordered_set<ComponentNetworkRef>());
}
void NetworkSystem::ServerRemoveClient(ENetPeer* client)
{
    clients.erase(client);
    clientLastTickAcknowledged.erase(client);
    pendingClientResourceChanges.erase(client);
    pendingClientComponentChanges.erase(client);
}

void NetworkSystem::InitializeConnectedClient(const World& world)
{
    auto k = 1;
}

#ifdef NETMODE_SERVER
void NetworkSystem::SendHierarchyEventsPackets(LocalSystemContext& ctx)
{
    packetByteStream.Clear();
    
    std::vector<GenericEventData> hierarchyEvents;
    ctx.PollEvents<
        EntityLevelChangedEvent, EntityReparentedEvent, 
        EntityMovedBeforeEvent, EntityMovedAfterEvent, 
        ComponentAddedEvent, ComponentRemovedEvent>(hierarchyEvents);
    for(const GenericEventData& event : hierarchyEvents)
    {
        if(event.GetEventType() == TYPEID(ComponentAddedEvent))
        {
            const ComponentAddedEvent& componentAddedEvent = *reinterpret_cast<const ComponentAddedEvent*>(event.GetEventPtr());

            // Case 1: NetworkComponent added to an entity -> Add the entity to the network
			if(componentAddedEvent.componentType == TYPEID(NetworkComponent))
			{
                size_t networkID = ServerAddEntityToNetwork(ctx, componentAddedEvent.entity);
                if(networkID == NETID_INVALID)
                    continue;

				HierarchyEventData_EntityAddedToNetwork addEntityEvent = {};
                addEntityEvent.entity = networkID;
				addEntityEvent.level = ctx.GetEntityLevel(componentAddedEvent.entity);
				addEntityEvent.parent = GetNetworkIDOfEntity(ctx.GetEntityParent(componentAddedEvent.entity));
                addEntityEvent.previous = GetNetworkIDOfEntity(ctx.GetEntityLastSibling(componentAddedEvent.entity));
                addEntityEvent.next = GetNetworkIDOfEntity(ctx.GetEntityNextSibling(componentAddedEvent.entity));
				addEntityEvent.name = ctx.GetEntityName(componentAddedEvent.entity);

                // Copy all of the entity's components to the component data list
                for(TypeID componentType : TypeRegistry::GetRegisteredComponentsList())
                    if(ctx.HasComponentOfType(componentAddedEvent.entity, componentType))
                        addEntityEvent.componentData.emplace_back(componentType, ctx.GetComponentOfType_Const(componentAddedEvent.entity, componentType));

                // Write the hierarchy event to the stream
                packetByteStream << EHierarchyEventType::EntityAddedToNetwork;
				packetByteStream << addEntityEvent;
			}
            // Case 2: Other component added to a network entity -> Tell clients to add the component
            else if(GetNetworkIDOfEntity(componentAddedEvent.entity) != NETID_INVALID)
            {
                HierarchyEventData_ComponentAdded addComponentEvent = {};
                addComponentEvent.entity = GetNetworkIDOfEntity(componentAddedEvent.entity);
                addComponentEvent.componentData = 
                    GenericComponentData(componentAddedEvent.componentType, ctx.GetComponentOfType_Const(componentAddedEvent.entity, componentAddedEvent.componentType));

                // Write the hierarchy event to the stream
                packetByteStream << EHierarchyEventType::ComponentAdded;
                packetByteStream << addComponentEvent;
            }
        }
        else if(event.GetEventType() == TYPEID(ComponentRemovedEvent))
        {
            const ComponentRemovedEvent& componentRemovedEvent = *reinterpret_cast<const ComponentRemovedEvent*>(event.GetEventPtr());

			// Case 3: NetworkComponent removed from an entity -> Remove the entity from the network
			if(componentRemovedEvent.componentType == TYPEID(NetworkComponent))
			{
				HierarchyEventData_EntityRemovedFromNetwork removeEntityEvent = {};
                removeEntityEvent.entity = GetNetworkIDOfEntity(componentRemovedEvent.entity);

                ServerRemoveEntityFromNetwork(ctx, componentRemovedEvent.entity);

                // Write the hierarchy event to the stream
                packetByteStream << EHierarchyEventType::EntityRemovedFromNetwork;
				packetByteStream << removeEntityEvent;
			}
			// Case 4: Other component removed from a network entity -> Tell clients to remove the component
            else if(GetNetworkIDOfEntity(componentRemovedEvent.entity) != NETID_INVALID)
            {
                HierarchyEventData_ComponentRemoved removeComponentEvent = {};
                removeComponentEvent.entity = GetNetworkIDOfEntity(componentRemovedEvent.entity);
                removeComponentEvent.componentType = componentRemovedEvent.componentType;

                // Write the hierarchy event to the stream
                packetByteStream << EHierarchyEventType::ComponentRemoved;
                packetByteStream << removeComponentEvent;
            }
        }
        else if(event.GetEventType() == TYPEID(EntityLevelChangedEvent)
            || event.GetEventType() == TYPEID(EntityReparentedEvent)
            || event.GetEventType() == TYPEID(EntityMovedBeforeEvent)
            || event.GetEventType() == TYPEID(EntityMovedAfterEvent))
        {
			Entity entity;
            if(event.GetEventType() == TYPEID(EntityLevelChangedEvent))
				entity = reinterpret_cast<const EntityLevelChangedEvent*>(event.GetEventPtr())->entity;
            else if(event.GetEventType() == TYPEID(EntityReparentedEvent))
				entity = reinterpret_cast<const EntityReparentedEvent*>(event.GetEventPtr())->entity;
            else if(event.GetEventType() == TYPEID(EntityMovedBeforeEvent))
				entity = reinterpret_cast<const EntityMovedBeforeEvent*>(event.GetEventPtr())->entity;
            else if(event.GetEventType() == TYPEID(EntityMovedAfterEvent))
				entity = reinterpret_cast<const EntityMovedAfterEvent*>(event.GetEventPtr())->entity;

            // Case 5: A network entity was moved in the hierarchy -> Tell clients where to move it
            if(GetNetworkIDOfEntity(entity) != NETID_INVALID)
            {
				HierarchyEventData_EntityMoved moveEntityEvent = {};
				moveEntityEvent.entity = GetNetworkIDOfEntity(entity);
				moveEntityEvent.level = ctx.GetEntityLevel(entity);
				moveEntityEvent.parent = GetNetworkIDOfEntity(ctx.GetEntityParent(entity));
				moveEntityEvent.previous = GetNetworkIDOfEntity(ctx.GetEntityLastSibling(entity));
				moveEntityEvent.next = GetNetworkIDOfEntity(ctx.GetEntityNextSibling(entity));

				// Write the hierarchy event to the stream
				packetByteStream << EHierarchyEventType::EntityMoved;
				packetByteStream << moveEntityEvent;
            }
        }
    }

    if(packetByteStream.GetSize() == 0)
        return;

    // Send all structural events to all clients
    // Every client gets the same events and must receive every structural update
    for(ENetPeer* client : clients)
        SendPacket(client, ENetworkPacketType::HierarchyEvents, packetByteStream.GetSize(), packetByteStream.buffer.data(), ENET_PACKET_FLAG_RELIABLE);

    LOG(ctx, "Sent hierarchy events packet for tick {}", currentNetworkTick);
}
void NetworkSystem::SendReplicationPackets(LocalSystemContext& ctx)
{
    const std::unordered_set<TypeID>& dirtyResources = NetworkDirtyTracker::GetDirtyResources();
    std::unordered_set<ComponentNetworkRef> dirtyComponents;
    for(const ComponentRef& component : NetworkDirtyTracker::GetDirtyComponents())
        dirtyComponents.emplace(GetNetworkIDOfEntity(component.entity), component.componentType);

    // Don't send packet if there's nothing to replicate
    if(dirtyResources.empty() && dirtyComponents.empty())
        return;

    for(TypeID resource : dirtyResources)
        resourceTickLastChanged[resource] = currentNetworkTick;
    for(const ComponentNetworkRef& component : dirtyComponents)
        componentTickLastChanged[component] = currentNetworkTick;

    for(ENetPeer* client : clients)
    {
        pendingClientResourceChanges.at(client).insert(dirtyResources.begin(), dirtyResources.end());
        pendingClientComponentChanges.at(client).insert(dirtyComponents.begin(), dirtyComponents.end());

        // Don't send any changes issued at a tick before or at the last tick the client received and acknowledged
        for(TypeID resource : pendingClientResourceChanges.at(client))
        {
            if(resourceTickLastChanged.at(resource) <= clientLastTickAcknowledged.at(client))
                pendingClientResourceChanges.at(client).erase(resource);
        }
        for(const ComponentNetworkRef& component : pendingClientComponentChanges.at(client))
        {
            if(componentTickLastChanged.at(component) <= clientLastTickAcknowledged.at(client))
                pendingClientComponentChanges.at(client).erase(component);
        }

        /* Create the replication packet from this client's changelist */

        packetByteStream.Clear();

        NetworkPacketData_StateReplication packetData = {};
        packetData.currentNetworkTick = currentNetworkTick;
        for(TypeID resource : pendingClientResourceChanges.at(client))
        {
            SERIALIZE_TO_PACKET(resource)(packetByteStream, ctx.GetResourceOfType(resource));

            ResourceReplicationData replicationData = {};
            replicationData.resource = resource;
            replicationData.resourceData = packetByteStream.GetBytes();

            packetData.resourceReplicationData.push_back(replicationData);
        }
        for(const ComponentNetworkRef& component : pendingClientComponentChanges.at(client))
        {
            SERIALIZE_TO_PACKET(component.componentType)(packetByteStream, ctx.GetComponentOfType(GetEntityByNetworkID(component.networkID), component.componentType));

            ComponentReplicationData replicationData = {};
            replicationData.componentRef = component;
            replicationData.componentData = packetByteStream.GetBytes();

            packetData.componentReplicationData.push_back(replicationData);
        }

        packetByteStream.Clear();
        
        packetByteStream << packetData;
        SendPacket(client, ENetworkPacketType::StateReplication, packetByteStream.GetSize(), packetByteStream.buffer.data());

        LOG(ctx, "Sent replication packet for tick {}", packetData.currentNetworkTick)
    }

    // Clear all dirty objects after all replication packets have been sent
    NetworkDirtyTracker::ClearDirtyResources();
    NetworkDirtyTracker::ClearDirtyComponents();
}
#endif // NETMODE_SERVER

void NetworkSystem::SendPacket_StateReplicationACK(ENetPeer* recipient, const NetworkPacketData_StateReplicationACK& packetData)
{
#ifdef NETMODE_CLIENT
    packetByteStream.Clear();

    packetByteStream << packetData;

    SendPacket(server, ENetworkPacketType::StateReplicationACK, packetByteStream.GetSize(), packetByteStream.buffer.data(), ENET_PACKET_FLAG_RELIABLE);
#endif // NETMODE_CLIENT
}
void NetworkSystem::SendPacket(ENetPeer* recipient, ENetworkPacketType packetType, uint32_t packetDataSize, const void* packetData, enet_uint32 flags)
{
    NetworkPacketHeader packetHeader = {};
    packetHeader.packetType = packetType;
    packetHeader.dataSize = packetDataSize;
    char* packet = (char*) malloc(sizeof(NetworkPacketHeader) + packetDataSize);
    if(!packet)
        return;

    memcpy(packet, &packetHeader, sizeof(NetworkPacketHeader));
    memcpy(packet + sizeof(NetworkPacketHeader), packetData, packetDataSize);

    ENetPacket* packetToSend = enet_packet_create(packet, sizeof(NetworkPacketHeader) + packetDataSize, flags);

    free(packet);

    enet_peer_send(recipient, 0, packetToSend);
    enet_host_flush(host);
}

void NetworkSystem::ReceivePacket(LocalSystemContext& ctx, ENetPeer* sender, const void* packet)
{
	const uint8_t* packetBytes = reinterpret_cast<const uint8_t*>(packet);

    const NetworkPacketHeader* header = reinterpret_cast<const NetworkPacketHeader*>(packet);

    packetByteStream.Clear();
	packetByteStream.CopyBytesIntoBuffer(packetBytes + sizeof(NetworkPacketHeader), header->dataSize);

    switch(header->packetType)
    {
        case ENetworkPacketType::LoadLevel:
            {
                NetworkPacketData_LoadLevel packetData;
                packetByteStream >> packetData;

                ReceivePacket_LoadLevel(ctx, sender, packetData);
            }
            break;
        case ENetworkPacketType::HierarchyEvents:
            ReceivePacket_HierarchyEvents(ctx, sender);
            break;
        case ENetworkPacketType::StateReplication:
            {
                NetworkPacketData_StateReplication packetData;
                packetByteStream >> packetData;

                ReceivePacket_StateReplication(ctx, sender, packetData);
            }
            break;
        case ENetworkPacketType::StateReplicationACK:
        {
            NetworkPacketData_StateReplicationACK packetData;
            packetByteStream >> packetData;

            ReceivePacket_StateReplicationACK(ctx, sender, packetData);
        }
        break;
        default: break;
    }
}
void NetworkSystem::ReceivePacket_LoadLevel(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_LoadLevel& packetData)
{

}
void NetworkSystem::ReceivePacket_HierarchyEvents(LocalSystemContext& ctx, ENetPeer* sender)
{
#ifdef NETMODE_CLIENT
    LOG(ctx, "Received Hierarchy Events Packet");
    
    while(packetByteStream.HasNext())
    {
		EHierarchyEventType eventType;
		packetByteStream >> eventType;

        switch(eventType)
        {
		    case EHierarchyEventType::EntityAddedToNetwork:
		        {
			        HierarchyEventData_EntityAddedToNetwork event;
			        packetByteStream >> event;

			        const Entity& parent = GetEntityByNetworkID(event.parent);
			        const Entity& previous = GetEntityByNetworkID(event.previous);
			        const Entity& next = GetEntityByNetworkID(event.next);

			        Entity entity;
			        if(event.previous != NETID_INVALID || event.next != NETID_INVALID)
				        entity = ctx.CreateEntity(previous, next, event.name);
			        else if(event.parent != NETID_INVALID)
				        entity = ctx.CreateEntity(parent, event.name);
			        else
				        entity = ctx.CreateEntity(event.name, event.level);

			        for(const GenericComponentData& componentData : event.componentData)
				        ctx.AddComponentOfType(entity, componentData.GetComponentType(), componentData.GetComponentPtr());

			        networkEntities.emplace(event.entity, entity);
                    networkIDs.emplace(entity, event.entity);
		        }
		        break;
		    case EHierarchyEventType::EntityRemovedFromNetwork:
		        {
			        HierarchyEventData_EntityRemovedFromNetwork event;
			        packetByteStream >> event;

					const Entity& entity = GetEntityByNetworkID(event.entity);
			        ctx.DestroyEntity(entity);

                    networkEntities.erase(event.entity);
                    networkIDs.erase(entity);
		        }
		        break;
		    case EHierarchyEventType::EntityMoved:
		        {
			        HierarchyEventData_EntityMoved event;
			        packetByteStream >> event;

                    if(event.previous != NETID_INVALID)
                        ctx.MoveEntityAfter(GetEntityByNetworkID(event.entity), GetEntityByNetworkID(event.previous));
                    else if(event.next != NETID_INVALID)
                        ctx.MoveEntityBefore(GetEntityByNetworkID(event.entity), GetEntityByNetworkID(event.next));
                    else if(event.parent != NETID_INVALID)
                        ctx.ReparentEntity(GetEntityByNetworkID(event.entity), GetEntityByNetworkID(event.parent));
                    else
                        ctx.MoveEntityToLevel(GetEntityByNetworkID(event.entity), event.level);
                }
                break;
            case EHierarchyEventType::ComponentAdded:
		        {
			        HierarchyEventData_ComponentAdded event;
			        packetByteStream >> event;

			        ctx.AddComponentOfType(GetEntityByNetworkID(event.entity), event.componentData.GetComponentType(), event.componentData.GetComponentPtr());
		        }
		        break;
            case EHierarchyEventType::ComponentRemoved:
		        {
			        HierarchyEventData_ComponentRemoved event;
			        packetByteStream >> event;

			        ctx.RemoveComponentOfType(GetEntityByNetworkID(event.entity), event.componentType);
		        }
		        break;
            default: break;
        }
    }
#endif // NETMODE_CLIENT
}
void NetworkSystem::ReceivePacket_StateReplication(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_StateReplication& packetData)
{
#ifdef NETMODE_CLIENT
    LOG(ctx, "Received Replication Packet for tick {}", packetData.currentNetworkTick);

    packetByteStream.Clear();
    
    for(const ResourceReplicationData& resourceRep : packetData.resourceReplicationData)
    {
        packetByteStream.CopyBytesIntoBuffer(resourceRep.resourceData.data(), resourceRep.resourceData.size());

        // Deserialize replication data into the existing resource instance
        DESERIALIZE_FROM_PACKET(resourceRep.resource)(packetByteStream, ctx.GetResourceOfType(resourceRep.resource));
    }
    for(const ComponentReplicationData& componentRep : packetData.componentReplicationData)
    {
        packetByteStream.CopyBytesIntoBuffer(componentRep.componentData.data(), componentRep.componentData.size());

        // Deserialize replication data into the existing component instance
        DESERIALIZE_FROM_PACKET(componentRep.componentRef.componentType)(packetByteStream, ctx.GetComponentOfType(GetEntityByNetworkID(componentRep.componentRef.networkID), componentRep.componentRef.componentType));
    }

    NetworkPacketData_StateReplicationACK ackPacketData = {};
    ackPacketData.acknowledgedTick = packetData.currentNetworkTick;

    SendPacket_StateReplicationACK(server, ackPacketData);
#endif // NETMODE_CLIENT
}
void NetworkSystem::ReceivePacket_StateReplicationACK(LocalSystemContext& ctx, ENetPeer* sender, const NetworkPacketData_StateReplicationACK& packetData)
{
#ifdef NETMODE_SERVER
    LOG(ctx, "Received Replication Packet ACK for tick {}", packetData.acknowledgedTick);

    clientLastTickAcknowledged.at(sender) = packetData.acknowledgedTick;
#endif // NETMODE_SERVER
}

Entity NetworkSystem::GetEntityByNetworkID(size_t networkID)
{
    if(networkEntities.find(networkID) != networkEntities.end())
        return networkEntities.at(networkID);

    return Entity();
}
size_t NetworkSystem::GetNetworkIDOfEntity(const Entity& entity)
{
    if(networkIDs.find(entity) != networkIDs.end())
        return networkIDs.at(entity);

    return NETID_INVALID;
}

#ifdef NETMODE_SERVER
size_t NetworkSystem::ServerAddEntityToNetwork(LocalSystemContext& ctx, const Entity& entity)
{
	// Ensure that all ancestors of entity have a NetworkComponent
	// If any ancestor does not have a NetworkComponent, return NETID_INVALID to indicate that the entity cannot be added to the network
	Entity ancestor = ctx.GetEntityParent(entity);
    while(ancestor.IsValid())
    {
        if(!ctx.HasComponent<NetworkComponent>(ancestor))
			return NETID_INVALID;
		
        ancestor = ctx.GetEntityParent(ancestor);
    }

	size_t networkID;
	// If there are any available network IDs that were previously used and then freed, reuse one of those IDs
    if(!availableNetworkIDs.empty())
    {
        networkID = availableNetworkIDs.top();
        availableNetworkIDs.pop();
    }
	// Otherwise, assign a new network ID
    else
        networkID = nextNetworkID++;

	ctx.GetComponent<NetworkComponent>(entity).networkID = networkID;
    networkIDs.emplace(entity, networkID);
    networkEntities.emplace(networkID, entity);

	// Also add all descendant branches which have NetworkComponents
    Entity descendant = ctx.GetEntityFirstChild(entity);
	while(descendant.IsValid())
	{
		if(ctx.HasComponent<NetworkComponent>(descendant))
			ServerAddEntityToNetwork(ctx, descendant);

		descendant = ctx.GetEntityNextSibling(descendant);
	}

    return networkID;
}
void NetworkSystem::ServerRemoveEntityFromNetwork(LocalSystemContext& ctx, const Entity& entity)
{
	size_t networkID = GetNetworkIDOfEntity(entity);

    networkIDs.erase(entity);
    networkEntities.erase(networkID);
    
    // If entity has a NetworkComponent, invalidate its networkID but don't remove the component
	// This is done to keep the entity considered for the network if its parent is re-added to the network later
    if(ctx.HasComponent<NetworkComponent>(entity))
		ctx.GetComponent<NetworkComponent>(entity).networkID = NETID_INVALID;

	// If the entity has the highest network ID, "shrink" the valid range
	if(networkID == nextNetworkID - 1)
	{
        nextNetworkID--;
		// Keep moving nextNetworkID down until we hit a non-empty index
		while(!availableNetworkIDs.empty() && nextNetworkID - 1 == availableNetworkIDs.top())
		{
            availableNetworkIDs.pop();
			nextNetworkID--;
		}
	}
	// Otherwise the networkID is in the valid range, so add it to free queue
	else
        availableNetworkIDs.push(networkID);

	// Also remove all descendants of the entity from the network
    ctx.ForEachEntity([this, &ctx](const Entity& descendant)
        {
            ServerRemoveEntityFromNetwork(ctx, descendant);
        }, entity);
}
#endif // NETMODE_SERVER

#pragma region PacketSerialization
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityAddedToNetwork& src)
{
    *this << src.entity;
    *this << src.level;
    *this << src.parent;
    *this << src.previous;
    *this << src.next;
    *this << src.name;

    *this << src.componentData.size();
    for(const GenericComponentData& componentData : src.componentData)
        *this << componentData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityAddedToNetwork& dst)
{
    *this >> dst.entity;
    *this >> dst.level;
    *this >> dst.parent;
    *this >> dst.previous;
    *this >> dst.next;
    *this >> dst.name;

	size_t componentCount;
    *this >> componentCount;
    for(size_t i = 0; i < componentCount; i++)
    {
        GenericComponentData componentData = {};
        *this >> componentData;

        dst.componentData.push_back(componentData);
    }
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityRemovedFromNetwork& src)
{
    *this << src.entity;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityRemovedFromNetwork& dst)
{
    *this >> dst.entity;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_EntityMoved& src)
{
    *this << src.entity;
	*this << src.level;
	*this << src.parent;
	*this << src.previous;
	*this << src.next;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_EntityMoved& dst)
{
    *this >> dst.entity;
    *this >> dst.level;
    *this >> dst.parent;
    *this >> dst.previous;
    *this >> dst.next;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_ComponentAdded& src)
{
    *this << src.entity;
	*this << src.componentData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_ComponentAdded& dst)
{
    *this >> dst.entity;
    *this >> dst.componentData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const HierarchyEventData_ComponentRemoved& src)
{
    *this << src.entity;
	*this << src.componentType;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(HierarchyEventData_ComponentRemoved& dst)
{
    *this >> dst.entity;
    *this >> dst.componentType;
}

template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const NetworkPacketData_StateReplication& src)
{
    *this << src.currentNetworkTick;

    *this << src.resourceReplicationData.size();
    for(const ResourceReplicationData& resourceRep : src.resourceReplicationData)
        *this << resourceRep;

    *this << src.componentReplicationData.size();
    for(const ComponentReplicationData& componentRep : src.componentReplicationData)
        *this << componentRep;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(NetworkPacketData_StateReplication& dst)
{
    *this >> dst.currentNetworkTick;

    dst.resourceReplicationData.clear();
    dst.componentReplicationData.clear();

    size_t resourceCount;
    *this >> resourceCount;
    for(size_t i = 0; i < resourceCount; i++)
    {
        ResourceReplicationData resourceData = {};
        *this >> resourceData;

        dst.resourceReplicationData.push_back(resourceData);
    }
    
    size_t componentCount;
    *this >> componentCount;
    for(size_t i = 0; i < componentCount; i++)
    {
        ComponentReplicationData componentData = {};
        *this >> componentData;

        dst.componentReplicationData.push_back(componentData);
    }
}

template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const ResourceReplicationData& src)
{
    *this << src.resource;
    *this << src.resourceData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(ResourceReplicationData& dst)
{
    *this >> dst.resource;
    *this >> dst.resourceData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Serialize(const ComponentReplicationData& src)
{
    *this << src.componentRef;
    *this << src.componentData;
}
template<>
template<>
void ByteStream<IPacketStreamFormat>::Deserialize(ComponentReplicationData& dst)
{
    *this >> dst.componentRef;
    *this >> dst.componentData;
}
#pragma endregion