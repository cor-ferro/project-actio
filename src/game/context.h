//
// Created by demitriy on 4/20/18.
//

#ifndef ACTIO_CONTEXT_H
#define ACTIO_CONTEXT_H

#include <entityx/entityx.h>

#include "world_object.h"
#include "input_output.h"
#include "components/transform.h"
#include "../lib/types.h"
#include "../lib/input_manager.h"
#include "../lib/assets.h"
#include "objects_manager.h"
#include "physic_manager.h"
#include "../lib/mesh_manager.h"

namespace game {
    namespace ex = entityx;

    class Context {
    public:
        explicit Context(InputOutput& io) : m_io(io) {}

        struct Data {
            float screenWidth = 0.0f;
            float screenHeight = 0.0f;
            float windowWidth = 0.0f;
            float windowHeight = 0.0f;
            vec2 mousePosition;
            vec3 mouseWorldPosition;
        };

        ex::Entity createObject(ex::EntityManager& es);

        inline ObjectsManager& objects() { return objectsManager; };

        inline PhysicManager& physic() { return m_physicManager; } ;

        inline MeshManager& meshes() { return m_meshManager; };

        inline InputManager& input() { return m_io.input; };

        inline WindowContext& window() { return *m_io.windowContext; };

        void getCharacter(ex::EntityManager& es, const size_t& id);

        void getCamera(ex::EntityManager& es);

        std::shared_ptr<Assets>& getAssets();

        Data data;
    private:
        std::shared_ptr<Assets> assets;
        InputOutput &m_io;

        ObjectsManager objectsManager;
        PhysicManager m_physicManager;
        MeshManager m_meshManager;
    };
}

#endif //ACTIO_CONTEXT_H
