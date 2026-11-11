#include "EventQueue.h"

const std::vector<size_t>& IEventQueue::PollEventIndices() const
{
	return eventIndices[readBufferIndex];
}

bool IEventQueue::IsEmpty() const
{
	return eventIndices[readBufferIndex].empty();
}