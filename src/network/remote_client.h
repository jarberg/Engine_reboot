#pragma once

#include <core/events.h>
#include <core/components.h>

class RemoteClient {
	 
	public:
	int clientid = -1;

	EventDispatcher events;
	Entity target;

	RemoteClient(int _clientid, Entity ent);

};