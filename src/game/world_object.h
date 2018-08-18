//
// Created by demitriy on 8/1/18.
//

#ifndef ACTIO_GAME_WORLD_OBJECT_H
#define ACTIO_GAME_WORLD_OBJECT_H

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include "components/base.h"
#include "components/transform.h"

namespace game {
    namespace ex = entityx;
    namespace c = components;

    class WorldObject {
    public:
        typedef ex::Entity::Id Id;

        explicit WorldObject(ex::Entity &fromEntity);

        ~WorldObject();

        ex::Entity getEntity() const;

        void rotate(vec3 vector, float angle);

        void setPosition(const glm::vec3 &position);

        void setRotation(const glm::quat &quat);

        void setScale(const glm::vec3 &scale);

        void setScale(const float &x, const float &y, const float &z);

        void setQuaternion(const glm::quat &quat);

        const Id getId();

        template <class C>
        bool hasComponent() {
            return entity_.has_component<C>();
        }

        template <class C>
        inline entityx::ComponentHandle<C> getComponent() {
            return entity_.component<C>();
        }

        ex::ComponentHandle<c::Transform> transform;
    protected:
        explicit WorldObject() = default;

        ex::Entity entity_;
    };
}

#endif //ACTIO_GAME_WORLD_OBJECT_H
