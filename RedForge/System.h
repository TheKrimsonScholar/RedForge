#pragma once

class System
{
protected:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

	virtual void Update() = 0;

	friend class Engine;
};