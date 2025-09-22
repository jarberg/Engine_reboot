#include "remote_client.h"
#include <core/world.h>
#include <core/components.h>

#include <parser.h>


RemoteClient::RemoteClient(int clientid, Entity ent){
	events = EventDispatcher();
	this->target = ent;
	this->clientid = clientid;

    auto& netComp = World::current()->add_component<NetworkComponent>(this->target, this->target, this->clientid);

    events.Subscribe<RClientMSGEvent>(
        [this](std::shared_ptr<RClientMSGEvent> ev) {
            try {
                PositionComponent& comp = World::current()->get_component<PositionComponent>(this->target);
                Vec3 val = parse_vec(ev->msg);
                comp.x = val.X;
				comp.y = val.Y;
				comp.z = val.Z;
            }
            catch (const std::exception& ex) {
                std::cerr << "RClientMSGEvent parse error: " << ex.what() << std::endl;
            }
        }
    );
}
