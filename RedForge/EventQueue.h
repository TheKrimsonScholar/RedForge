#pragma once

#include <vector>

#include "EventMacros.h"

class IEventQueue
{
public:
	virtual void PushEvent(const void* event) = 0;

private:
	virtual void SwapBuffers() = 0;

	friend class Scheduler;
};
template<typename T>
class EventQueue : public IEventQueue
{
	std::vector<T> buffers[2];
	uint8_t writeBufferIndex = 0;
	uint8_t readBufferIndex = 1;

public:
	void PushEvent(const void* event)
	{
		PushEvent(*static_cast<const T*>(event));
	}
	void PushEvent(const T& event)
	{
		buffers[writeBufferIndex].push_back(event);
	}
	const std::vector<T>& PollEvents() const
	{
		return buffers[readBufferIndex];
	}

private:
	void SwapBuffers() override
	{
		std::swap(writeBufferIndex, readBufferIndex);

		buffers[writeBufferIndex].clear();
	}

	friend class Scheduler;
};