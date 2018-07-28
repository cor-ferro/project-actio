#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define NDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <entityx/entityx.h>
#include "context.h"
#include "character.h"
#include "script.h"
#include "../lib/console.h"
#include "../lib/watch.h"
#include "../lib/input_handler.h"
#include "../lib/ini_loader.h"
#include "../lib/profiling.h"
#include "../lib/assets.h"
#include "../resources/file_resource.h"
#include "../renderer/base_renderer.h"
#include "../core/model_builder.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "systems/input.h"
#include "systems/weapons.h"
#include "systems/render.h"
#include "components/char_items.h"
#include "components/weapon.h"
#include "components/skin.h"
#include "components/weaponStrategy.h"
#include "desc/light_point.h"
#include "desc/light_directional.h"
#include "desc/light_spot.h"
#include "desc/weapon.h"
#include "weapon_handler.h"

namespace game {
    namespace ex = entityx;
    namespace c = components;

    typedef glm::vec3 vec3;
    typedef glm::vec2 vec2;
    typedef glm::mat4 mat4;

    struct World : ex::EntityX {
        /**
         * Base world object
         */
        class WorldObject {
        public:
            explicit WorldObject(ex::Entity &fromEntity) {
                assert(fromEntity.valid());
                entity_ = fromEntity;

                transform = entity_.assign<c::Transform>();
            }

            ex::Entity getEntity() {
                return entity_;
            }

            ex::ComponentHandle<c::Transform> transform;
        protected:
            explicit WorldObject() = default;

            ex::Entity entity_;
        };


        /**
         * Character of world
         */
        class Character : public WorldObject {
        public:
            Character(ex::Entity &fromEntity, Resource::Assimp *resource, Assets *assets) : WorldObject(fromEntity) {
                model = entity_.assign<components::Model>();
                character = entity_.assign<c::Character>();
                items = entity_.assign<c::CharItems>();

                ModelBuilder::FromAi(model.get(), resource, assets);

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
            explicit Character() = default;
        };


        /**
         * Static object of world, i.e. tree, wall...
         */
        class StaticObject : public WorldObject {
        public:
            StaticObject(ex::Entity &fromEntity, Mesh *mesh) : WorldObject(fromEntity) {
                model = entity_.assign<c::Model>(mesh);
            }

            StaticObject(const StaticObject &other) = default;

            StaticObject &operator=(const StaticObject &other) = default;

            ex::ComponentHandle<c::Model> model;
        private:
            explicit StaticObject() = default;
        };


        /**
         * Base weapon of world
         */
        class Weapon : public WorldObject {
        public:
            Weapon(ex::Entity &fromEntity, desc::Weapon description, strategy::WeaponsBase *weaponStrategy)
                    : WorldObject(fromEntity) {
                weapon = entity_.assign<c::Weapon>(description);
                strategy = entity_.assign<c::WeaponStrategy>(weaponStrategy);
            }

            Weapon(const Weapon &other) = default;

            Weapon &operator=(const Weapon &other) = default;

            ex::ComponentHandle<c::Weapon> weapon;
            ex::ComponentHandle<c::WeaponStrategy> strategy;
        private:
            explicit Weapon() = default;
        };

        /**
         * World constructor
         */
        World();

        void setupRenderer(renderer::Renderer *);

        void destroyRenderer();

        void setupInput(InputHandler *ih);

        void setup();

        void initGroundPlane();

        void destroy();

        inline void spawn(Character *character);

        void spawn(Character *character, glm::vec3 pos);

        World::Character createCharacter(Resource::Assimp *resource);

        void removeCharacter(World::Character character);

        World::StaticObject createStaticObject(Mesh *mesh);

        void removeStaticObject(World::StaticObject object);

        bool registerWeapon(strategy::WeaponsBase *system);

        World::Weapon createWeapon();

        void add();

        void remove();

        void addLight(desc::LightPointDesc);

        void addLight(desc::LightDirectionalDesc);

        void addLight(desc::LightSpotDesc);

        void removeLight(entityx::Entity entity);

        void start(ex::TimeDelta dt);

        void update(ex::TimeDelta dt);

        void render(ex::TimeDelta dt);

        void setPhysicsDebug(bool value);

        void setLightDebug(bool value);

        void setCameraSettings(float fov, float aspect, float near, float far);

        void setGravity(vec3 dir);

        void cameraLookAt(vec3 target);

        void setRenderSize(renderer::ScreenSize width, renderer::ScreenSize height);

        void setInput(int place, InputHandler *ih);

        void removeInput(systems::Input::InputPlace place);

        void forcePush(ex::Entity entity, vec3 direction, float force);

        void impactWave(vec3 position, vec3 direction);

        Character getUserControlCharacter();

        const game::Context &getContext();

        void importAssets(std::shared_ptr<Assets> &newAssets);

        void load();

        void unload();

    private:
        std::string name;

        std::shared_ptr<game::systems::Physic> physic = nullptr;
        std::shared_ptr<game::systems::Camera> camera = nullptr;
        std::shared_ptr<game::systems::Weapons> weapons = nullptr;
        std::shared_ptr<game::systems::Render> renderer = nullptr;

        InputHandler *input1 = nullptr;
        InputHandler *input2 = nullptr;

        Context context;

        profiling::ProfileTimings systemProfiler;

        std::vector<game::Script*> scripts;

        std::shared_ptr<Assets> assets;

        void reloadAssetsScripts();

        void reloadRenderAssets();

        void unloadScripts();

        void unloadTextures();
    };
}

#endif
