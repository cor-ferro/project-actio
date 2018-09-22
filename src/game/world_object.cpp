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
        if (entity_.valid()) {
            entity_.destroy();
        }
    }

    const WorldObject::Id WorldObject::getId() {
        return entity_.id();
    }

    void WorldObject::rotate(vec3 vector, float angle) {
        transform->rotate(vector, angle);
        transform->dirty = true;
    }

    const glm::vec3 &WorldObject::getPosition() const {
        return transform->position;
    }

    void WorldObject::setPosition(const glm::vec3 &position) {
        transform->setPosition(position);
        transform->dirty = true;
    }

    void WorldObject::setRotation(const glm::quat &quat) {
        transform->rotate(quat);
        transform->dirty = true;
    }

    void WorldObject::setScale(const glm::vec3 &scale) {
        transform->setScale(scale);
        transform->dirty = true;
    }

    void WorldObject::setScale(const float &x, const float &y, const float &z) {
        transform->setScale(x, y, z);
        transform->dirty = true;
    }

    void WorldObject::setQuaternion(const glm::quat &quat) {
        transform->setQuaternion(quat);
        transform->dirty = true;
    }

}