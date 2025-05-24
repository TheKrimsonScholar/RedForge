#pragma once

#include <unordered_map>
#include <functional>

struct InputComponent
{
	std::unordered_map<uint32_t, std::function<void(uint32_t)>> mouseDownCallbacks;

	std::unordered_map<uint32_t, std::function<void(uint32_t)>> keyDownCallbacks;
};