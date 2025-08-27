#pragma once

#include "Exports.h"

#include <enet/enet.h>

class NetworkSystem
{
private:
	static inline NetworkSystem* Instance;

	ENetHost* host = nullptr;
	bool isConnected = false;

public:
	NetworkSystem() {};
	~NetworkSystem() {};

	void Startup();
	void Shutdown();

	REDFORGE_API void Update();

	bool Connect();
	void Disconnect();
};