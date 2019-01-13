#ifndef GAME_COMPONENTS_PHYSIC_H_
#define GAME_COMPONENTS_PHYSIC_H_

#include <glm/glm.hpp>
#include <entityx/entityx/Entity.h>
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxMaterial.h"
#include "../../lib/types.h"

namespace game {
    namespace components {
        namespace px = physx;

        class PhysicActor {
        public:
            enum Type {
                Character,
                Weapon,
                WeaponProjectile,
                StaticObject,
                DynamicObject,
                Other
            };

            explicit PhysicActor(px::PxRigidActor *actor, entityx::Entity entity) : actor(actor), entity(entity) {
                actor->userData = static_cast<void*>(&this->entity);
            }

            ~PhysicActor() {
                if (actor != nullptr) {
//                    actor->userData = nullptr;

                    auto *scene = actor->getScene();

                    if (scene == nullptr) {
                        return;
                    }

                    scene->removeActor(*actor);
                }
            }

            const px::PxVec3 getPosition() const {
                return actor->getGlobalPose().p;
            }

            const px::PxQuat getRotation() const {
                return actor->getGlobalPose().q;
            }

            const px::PxTransform getTransform() const {
                return actor->getGlobalPose();
            }

            void setPosition(const vec3& pos) {
                px::PxTransform transform = actor->getGlobalPose();
                transform.p.x = pos.x;
                transform.p.y = pos.y;
                transform.p.z = pos.z;

                actor->setGlobalPose(transform);
            }

            void setPosition(const px::PxVec3& pos) {
                px::PxTransform transform = actor->getGlobalPose();
                transform.p = pos;

                actor->setGlobalPose(transform);
            }

            void setPosition(const px::PxTransform& transform) {
                actor->setGlobalPose(transform);
            }

            px::PxRigidActor *const getActor() const {
                return actor;
            }

            px::PxRigidDynamic *const getDynamicActor() const {
                return static_cast<px::PxRigidDynamic *>(actor);
            }

            px::PxRigidStatic *const getStaticActor() const {
                return static_cast<px::PxRigidStatic *>(actor);
            }

            bool isDynamic() const {
                return actor->is<px::PxRigidDynamic>() != NULL;
            }

            bool isStatic() const {
                return actor->is<px::PxRigidStatic>() != NULL;
            }

            entityx::Entity *getEntity() {
                return static_cast<entityx::Entity *>(actor->userData);
            }

        private:
            px::PxRigidActor *actor = nullptr;
            entityx::Entity entity;
            Type type = Other;
        };
    }
}

#endif
