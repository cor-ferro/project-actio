//
// Created by demitriy on 5/2/18.
//

#ifndef ACTIO_EVENTS_PHYSIC_CONTACT_H
#define ACTIO_EVENTS_PHYSIC_CONTACT_H

namespace game {
    namespace events {
        namespace px = physx;

        struct PhysicContact {
            explicit PhysicContact(px::PxActor *actor1, px::PxActor *actor2)
                    : actor1(actor1)
                    , actor2(actor2) {};

            px::PxActor *actor1;
            px::PxActor *actor2;
        };
    }
}

#endif //ACTIO_EVENTS_PHYSIC_CONTACT_H
