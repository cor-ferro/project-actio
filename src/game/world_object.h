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
        explicit WorldObject(ex::Entity &fromEntity);

        ex::Entity getEntity() const;

        void rotate(vec3 vector, float angle);

        void setPosition(const glm::vec3 &position);

        void setRotation(const glm::quat &quat);

        void setScale(const glm::vec3 &scale);

        void setScale(const float &x, const float &y, const float &z);

        void setQuaternion(const glm::quat &quat);

        ex::ComponentHandle<c::Transform> transform;
    protected:
        explicit WorldObject() = default;

        ex::Entity entity_;
    };
}

#endif //ACTIO_GAME_WORLD_OBJECT_H
