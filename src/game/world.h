#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
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
#include "../core/skin_builder.h"
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
#include "world_object.h"

namespace game {
    namespace ex = entityx;
    namespace c = components;

    typedef glm::vec3 vec3;
    typedef glm::vec2 vec2;
    typedef glm::mat4 mat4;

    struct World : ex::EntityX {
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

        void setup();

        void destroy();

        void update(ex::TimeDelta dt);

        void render(ex::TimeDelta dt);

        bool registerWeapon(strategy::WeaponsBase *system);

        World::Weapon createWeapon();

        void addLight(desc::LightPointDesc);

        void addLight(desc::LightDirectionalDesc);

        void addLight(desc::LightSpotDesc);

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

        std::shared_ptr<Material> findMaterial(const std::string &name);

        game::WorldObject *getUserControlCharacter();

        game::Context &getContext();

        void importAssets(std::shared_ptr<Assets> &newAssets);

        void load();

        void unload();

        const profiling::ProfileTimings &getSystemProfiler() const;

        void generateBaseTerrain();

        /* ----- API v2 ----- */

        game::WorldObject *createObject();

        game::WorldObject *createObject(const glm::vec3 &pos);

        game::WorldObject *createStaticObject();

        game::WorldObject *createDynamicObject();

        game::WorldObject *createStaticObject(const glm::vec3 &pos);

        game::WorldObject *createDynamicObject(const glm::vec3 &pos);

        game::WorldObject *createCharacterObject();

        void setObjectMesh(game::WorldObject *object, std::shared_ptr<Mesh> &mesh);

        void setObjectModel(game::WorldObject *object, assets::Model *);

        void destroyObject(game::WorldObject *object);

        ex::Entity createTerrain(const std::shared_ptr<ImageData> &image);

        void hideObject(ex::Entity &entity);

        void showObject(ex::Entity &entity);

        void hideObject(game::WorldObject *object);

        void showObject(game::WorldObject *object);

        void spawn(game::WorldObject *object);

        void spawn(game::WorldObject *object, const vec3 &position);

        void makeControlled(game::WorldObject *object);

        void makeUnControlled(game::WorldObject *object);

        assets::Model *findAssetModel(const std::string &name);

    private:
        std::string name;

        std::shared_ptr<game::systems::Physic> physic = nullptr;
        std::shared_ptr<game::systems::Camera> camera = nullptr;
        std::shared_ptr<game::systems::Weapons> weapons = nullptr;
        std::shared_ptr<game::systems::Render> renderer = nullptr;

        std::map<game::WorldObject::Id, game::WorldObject*> objects;

        InputHandler *input1 = nullptr;
        InputHandler *input2 = nullptr;

        Context context;

        profiling::ProfileTimings systemProfiler;

        std::vector<game::Script *> scripts;

        std::shared_ptr<Assets> assets;

        void reloadAssetsScripts();

        void reloadRenderAssets();

        void unloadScripts();

        void unloadTextures();
    };
}

#endif
