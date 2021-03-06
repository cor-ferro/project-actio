//
// Created by demitriy on 8/1/18.
//

#ifndef ACTIO_GAME_WORLD_OBJECT_H
#define ACTIO_GAME_WORLD_OBJECT_H

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include <boost/signals2.hpp>
#include "components/base.h"
#include "components/transform.h"

namespace game {
    namespace ex = entityx;
    namespace c = components;

    class WorldObject {
    public:
        struct Description {
            Description() = default;
            explicit Description(const vec3& position);

            enum Bounding {BOUNDING_BOX, BOUNDING_SPHERE, BOUNDING_CAPSULE};

            vec3 position = vec3(0.0f);
            vec3 boundingSize = vec3(1.0f);
            Bounding boundingType = BOUNDING_BOX;
        };

        typedef ex::Entity::Id Id;

        explicit WorldObject(ex::Entity &fromEntity);

        WorldObject(const WorldObject &other);

        WorldObject(WorldObject && other) noexcept;

        ~WorldObject();

        ex::Entity getEntity() const;

        void rotate(vec3 vector, float angle);

        const glm::vec3 &getPosition() const;

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

        template <class C, typename ... Args>
        inline entityx::ComponentHandle<C> addComponent(Args && ... args) {
            return entity_.assign<C>(std::forward<Args>(args)...);
        }

        ex::ComponentHandle<c::Transform> transform;
    protected:
        explicit WorldObject() = default;

        ex::Entity entity_;

        boost::signals2::signal<void ()> onSpawn;
    };
}

#endif //ACTIO_GAME_WORLD_OBJECT_H
