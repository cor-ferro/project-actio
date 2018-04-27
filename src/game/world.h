#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define NDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "entityx/entityx.h"
#include "character.h"
#include "components/skin.h"
#include "../lib/console.h"
#include "../lib/input_handler.h"
#include "../resources/file_resource.h"
#include "../lib/ini_loader.h"
#include "../renderer/base_renderer.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "desc/light_point.h"
#include "desc/light_directional.h"
#include "desc/light_spot.h"
#include "context.h"
#include "components/char_items.h"
#include "components/weapon.h"
#include "desc/weapon.h"

namespace game {
    namespace ex = entityx;
    namespace c = components;

    typedef glm::vec3 vec3;
    typedef glm::vec2 vec2;
    typedef glm::mat4 mat4;

    struct World : ex::EntityX {
        class WorldObject {
        public:
            ex::Entity getEntity() {
                return entity_;
            }

            ex::ComponentHandle<c::Transform> transform;
        protected:
            ex::Entity entity_;
        };

        class Character : public WorldObject {
        public:
            Character(ex::Entity &fromEntity, Resource::Assimp* resource) {
                assert(fromEntity.valid());

                entity_ = fromEntity;

                model = entity_.assign<components::Model>(resource);
                character = entity_.assign<c::Character>();
                items = entity_.assign<c::CharItems>();
                transform = entity_.assign<c::Transform>();

                if (resource->hasAnimations()) {
                    skin = entity_.assign<components::Skin>(resource);

                    auto nodeIndexes = skin->getNodeIndexes();
                    model->reindexMeshBones(nodeIndexes);
                }
            };

            Character(const Character &other) = default;

            Character &operator=(const Character &other) = default;

            static Character Restore(ex::Entity fromEntity) {
                Character character;
                character.entity_ = fromEntity;

                character.model = components::get<c::Model>(character.entity_);
                character.skin = components::get<c::Skin>(character.entity_);
                character.character = components::get<c::Character>(character.entity_);
                character.items = components::get<c::CharItems>(character.entity_);
                character.transform = components::get<c::Transform>(character.entity_);

                return character;
            }

            ex::ComponentHandle<c::Model> model;
            ex::ComponentHandle<c::Skin> skin;
            ex::ComponentHandle<c::Character> character;
            ex::ComponentHandle<c::CharItems> items;
        private:
            Character() = default;
        };

        class StaticObject : public WorldObject {
        public:
            StaticObject(ex::Entity &fromEntity, Mesh* mesh) {
                assert(fromEntity.valid());

                entity_ = fromEntity;

                model = entity_.assign<c::Model>(mesh);
                transform = entity_.assign<c::Transform>();
            }

            StaticObject(const StaticObject &other) = default;

            StaticObject &operator=(const StaticObject &other) = default;

            ex::ComponentHandle<c::Model> model;
        private:
            StaticObject() = default;
        };

        class Weapon : public WorldObject {
        public:
            Weapon(ex::Entity &fromEntity, desc::Weapon description) {
                assert(fromEntity.valid());

                entity_ = fromEntity;

                weapon = entity_.assign<c::Weapon>(description);
                transform = entity_.assign<c::Transform>();
            }

            Weapon(const Weapon &other) = default;

            Weapon &operator=(const Weapon &other) = default;

            ex::ComponentHandle<c::Weapon> weapon;
        private:
            Weapon() = default;
        };


        World();

        void setupRenderer(renderer::Renderer *);

        void setupInput(InputHandler *ih);

        void setup();

        void initGroundPlane();

        void destroy();

        inline void spawn(Character *character);

        void spawn(Character *character, glm::vec3 pos);

        World::Character createCharacter(Resource::Assimp* resource);

        void removeCharacter(World::Character character);

        World::StaticObject createStaticObject(Mesh* mesh);

        void removeStaticObject(World::StaticObject object);

        World::Weapon createWeapon();

        void add();

        void remove();

        void addLight(desc::LightPointDesc);

        void addLight(desc::LightDirectionalDesc);

        void addLight(desc::LightSpotDesc);

        void removeLight(entityx::Entity entity);

        void update(ex::TimeDelta dt);

        void render(ex::TimeDelta dt);

        void setPhysicsDebug(bool value);

        void setLightDebug(bool value);

        void setCameraSettings(float fov, float aspect, float near, float far);

        void setGravity(vec3 dir);

        void cameraLookAt(vec3 target);

        void setRenderSize(renderer::ScreenSize width, renderer::ScreenSize height);

    private:
        std::string name;

//        std::unordered_set<Character> characters;

        std::shared_ptr<game::systems::Physic> physic = nullptr;
        std::shared_ptr<game::systems::Camera> camera = nullptr;

        Context context;
    };

    struct WorldController {
        static void move(World *world, Character *character);
    };
}

#endif
