#include "NetworkSystem.h"

#include <iostream>

void NetworkSystem::Startup()
{
    Connect();
}
void NetworkSystem::Shutdown()
{
    Disconnect();
}

void NetworkSystem::Update()
{
    if(!isConnected)
        return;

    #ifdef NETMODE_SERVER
    ENetEvent event;
    while(enet_host_service(host, &event, 0) > 0) // 0ms timeout; non-blocking
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Client connected from " << event.peer->address.host << "\n";
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
                std::cout << "Server says: " << (char*) event.packet->data << "\n";
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server.\n";
                break;
            default: break;
        }
    }
    #endif // NETMODE_CLIENT
}

bool NetworkSystem::Connect()
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
    address.host = ENET_HOST_ANY;   // listen on all interfaces
    address.port = 1234;

    host = enet_host_create(&address, 32, 2, 0, 0);
    if(!host)
    {
        std::cerr << "Failed to create ENet server.\n";
        return false;
    }

    std::cout << "Server started on port 1234...\n";
    #endif // NETMODE_SERVER
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
    enet_address_set_host(&address, "127.0.0.1"); // server IP
    address.port = 1234;

    ENetPeer* peer = enet_host_connect(host, &address, 2, 0);
    if(!peer)
    {
        std::cerr << "No available peers.\n";
        return false;
    }

    ENetEvent event;
    if(enet_host_service(host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connected to server!\n";

        std::string msg = "Hello from client!";
        ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(host);
    }
    else
    {
        std::cerr << "Connection to server failed.\n";
        enet_peer_reset(peer);
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