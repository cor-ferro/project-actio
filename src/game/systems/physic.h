//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_SYSTEMS_PHYSIC_H
#define ACTIO_SYSTEMS_PHYSIC_H

#include <entityx/entityx/entityx.h>
#include <stack>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/System.h>

#include "../components/base.h"
#include "../components/physic_actor.h"
#include "../components/transform.h"
#include "../components/controlled.h"
#include "../components/model.h"
#include "../components/mesh.h"
#include "../components/renderable.h"
#include "../components/character.h"
#include "../components/user_control.h"
#include "../components/physic_actor.h"
#include "../events/physic_force.h"
#include "../events/setup_controlled.h"
#include "../events/key_press.h"
#include "../events/render_update_mesh.h"
#include "../events/physic_contact.h"
#include "../events/object_create.h"
#include "../../core/mesh.h"
#include "../../core/geometry.h"
#include "../../core/vertex.h"
#include "../../lib/console.h"
#include "../../lib/image_data.h"
#include "../context.h"
#include "../world_object.h"
#include "base.h"

#define PX_REST_VEC(vec) px::PxVec3(vec.x, vec.y, vec.z)

namespace game {
    class World;

    struct HeightMap {
        explicit HeightMap(const physx::PxU32 &cols, const physx::PxU32 &rows)
            : cols(cols)
            , rows(rows)
            , size(cols * rows)
            , values(new physx::PxI16[cols * rows])
        {}

        ~HeightMap() {
            delete []values;
        }

        physx::PxI16 *values = nullptr;
        physx::PxU32 cols;
        physx::PxU32 rows;
        size_t size;
    };

    namespace systems {
        namespace ex = entityx;
        namespace px = physx;
        namespace c = components;

        class Physic
                : public systems::BaseSystem
                  , public entityx::System<Physic>
                  , public ex::Receiver<Physic> {
        public:
            enum SceneParam {
                Scene_Gravity
            };

            enum class GeometryType {
                Box,
                Capsule,
                Sphere,
                TriangleMesh
            };

            explicit Physic(Context& context);

            ~Physic() override;

            void configure(ex::EntityManager &entities, entityx::EventManager &event_manager) override;

            void start(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void updateDebugBuffer(ex::EventManager& events);

            void computeBoundingBox(game::WorldObject *object);

            void receive(const ex::EntityCreatedEvent &event);

            void receive(const ex::EntityDestroyedEvent &event);

            void receive(const events::ObjectCreate &event);

            void receive(const events::PhysicForce &event);

            void receive(const events::MakeControlled &event);

            void receive(const events::MakeUnControlled &event);

            HeightMap *createHeightMap(const std::shared_ptr<ImageData> &image);

            px::PxRigidStatic *generateTerrain(const HeightMap &heightmap, const px::PxReal &width, const px::PxReal &height);

            std::shared_ptr<Mesh> generateTerrainMesh(px::PxRigidStatic *actor, const HeightMap &heightmap);
        private:
            ex::Entity debugLinesEntity;
            ex::Entity debugTrianglesEntity;
            ex::Entity debugPointsEntity;

            std::stack<std::pair<px::PxActor *, px::PxActor *>> contactedActors;

            bool isDisplayDebug = false;
        };
    }
}

#endif //ACTIO_SYSTEMS_PHYSIC_H
