#include "world_object.h"

namespace game {
    WorldObject::WorldObject(entityx::Entity &fromEntity) {
        assert(fromEntity.valid());

        entity_ = fromEntity;
        transform = entity_.assign<c::Transform>();
    }

    ex::Entity WorldObject::getEntity() const {
        return entity_;
    }

    void WorldObject::rotate(vec3 vector, float angle) {
        transform->rotate(vector, angle);
    }

    void WorldObject::setPosition(glm::vec3 position) {
        transform->setPosition(position);
    }

    void WorldObject::setRotation(glm::quat quat) {
        transform->rotate(quat);
    }

    void WorldObject::setScale(glm::vec3 scale) {
        transform->setScale(scale);
    }

    void WorldObject::setQuaternion(glm::quat quat) {
        transform->setQuaternion(quat);
    }
}