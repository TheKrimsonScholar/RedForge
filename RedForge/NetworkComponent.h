#pragma once

#include "ComponentRegistration.h"

#define NETID_INVALID -1

struct NetworkComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS(networkID)
	REPLICATED_COMPONENT_VARS()

private:
	size_t networkID = NETID_INVALID;

	friend class NetworkSystem;

public:
	size_t GetNetworkID() const { return networkID; }
};