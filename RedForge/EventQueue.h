#pragma once

#include <vector>

#include "TypeRegistry.h"

class IEventQueue
{
protected:
	std::vector<size_t> eventIndices[2];
	uint8_t writeBufferIndex = 0;
	uint8_t readBufferIndex = 1;

public:
	virtual ~IEventQueue() = default;

	virtual void PushEvent(const void* event, size_t absoluteEventIndex) = 0;

	const std::vector<size_t>& PollEventIndices() const;
	
	virtual const void* GetRawEventListPtr() const = 0;

	bool IsEmpty() const;

private:
	virtual void SwapBuffers() = 0;

	friend class Scheduler;
};
template<typename T>
class EventQueue : public IEventQueue
{
	static_assert(std::is_base_of_v<IEvent, T>, "EventQueue template parameter must be derived from IEvent");

	std::vector<T> buffers[2];

public:
	void PushEvent(const void* event, size_t absoluteEventIndex)
	{
		PushEvent(*static_cast<const T*>(event), absoluteEventIndex);
	}
	void PushEvent(const T& event, size_t absoluteEventIndex)
	{
		buffers[writeBufferIndex].push_back(event);
		eventIndices[writeBufferIndex].push_back(absoluteEventIndex);
	}
	const std::vector<T>& PollEvents() const
	{
		return buffers[readBufferIndex];
	}

	const void* GetRawEventListPtr() const
	{
		return buffers[readBufferIndex].data();
	}

private:
	void SwapBuffers() override
	{
		std::swap(writeBufferIndex, readBufferIndex);

		buffers[writeBufferIndex].clear();
		eventIndices[writeBufferIndex].clear();
	}

	friend class Scheduler;
};