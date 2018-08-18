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

    WorldObject::~WorldObject() {
        entity_.destroy();
    }

    const WorldObject::Id WorldObject::getId() {
        return entity_.id();
    }

    void WorldObject::rotate(vec3 vector, float angle) {
        transform->rotate(vector, angle);
    }

    void WorldObject::setPosition(const glm::vec3 &position) {
        transform->setPosition(position);
    }

    void WorldObject::setRotation(const glm::quat &quat) {
        transform->rotate(quat);
    }

    void WorldObject::setScale(const glm::vec3 &scale) {
        transform->setScale(scale);
    }

    void WorldObject::setScale(const float &x, const float &y, const float &z) {
        transform->setScale(x, y, z);
    }

    void WorldObject::setQuaternion(const glm::quat &quat) {
        transform->setQuaternion(quat);
    }

}