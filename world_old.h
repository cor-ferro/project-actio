#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <glm/glm.hpp>
#include <entityx/entityx.h>
#include <boost/lockfree/queue.hpp>
#include "context.h"
#include "character.h"
#include "script.h"
#include "../lib/console.h"
#include "../lib/watch.h"
#include "../lib/input_manager.h"
#include "../lib/ini_loader.h"

#include "../lib/assets.h"
#include "../resources/file_resource.h"
#include "../renderer/renderer.h"
#include "../core/model_builder.h"
#include "../core/skin_builder.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "systems/input.h"
#include "systems/weapons.h"
#include "systems/render.h"
#include "components/weapon.h"
#include "components/weapon_strategy.h"
#include "desc/light_point.h"
#include "desc/light_directional.h"
#include "desc/light_spot.h"
#include "desc/weapon.h"
#include "weapon_handler.h"
#include "world_object.h"
#include "world_tasks.h"

/**
 * \defgroup World
 * \brief World namespace
 */

namespace game {
    namespace ex = entityx;
    namespace c = components;

    class World {
        /**
         * Base weapon of world
         */
        class Weapon : public WorldObject {
        public:
            Weapon(ex::Entity& fromEntity, desc::Weapon description, strategy::WeaponStrategy *weaponStrategy)
                    : WorldObject(fromEntity) {
                weapon = entity_.assign<c::Weapon>(description);
                strategy = entity_.assign<c::WeaponStrategy>(weaponStrategy);
            }

            Weapon(const Weapon& other) = default;

            Weapon& operator=(const Weapon& other) = default;

            ex::ComponentHandle<c::Weapon> weapon;
            ex::ComponentHandle<c::WeaponStrategy> strategy;
        private:
            explicit Weapon() = default;
        };

        /**
         * World constructor
         */
        World();

        void setup();

        void destroy();

        void update(const ex::TimeDelta& dt);

        void render(const ex::TimeDelta& dt);

        void setRenderer(renderer::Renderer *renderer);

        bool registerWeapon(strategy::WeaponStrategy *system);

        void addLight(desc::LightPointDesc);

        void addLight(desc::LightDirectionalDesc);

        void addLight(desc::LightSpotDesc);

        void setPhysicsDebug(bool value);

        void setLightDebug(bool value);

        void setCameraSettings(float fov, float aspect, float near, float far);

        void setGravity(const vec3& dir);

        void cameraLookAt(const vec3& target);

        void setRenderSize(renderer::Dimension width, renderer::Dimension height);

        void setInput(int place, InputManager *ih);

        void removeInput(systems::Input::InputPlace place);

        std::shared_ptr<Material> findMaterial(const std::string& name);

        game::WorldObject *getUserControlCharacter();

        game::Context& getContext();

        void importAssets(std::shared_ptr<Assets>& newAssets);

        std::shared_ptr<Assets> getAssets();

        void load();

        void unload();

        const profiling::ProfileTimings& getSystemProfiler() const;

        void generateBaseTerrain();


        void asyncLoad(assets::Model *asset);

        void asyncLoadModel(assets::Model *asset);

        bool hasTasks();

        WorldTask *popTaskToPerform();

        void performTask(WorldTask *task);

        void flush();

        void setupRenderMesh(ex::Entity& entity);

        /* ----- API v2 ----- */

        game::WorldObject *createObject();

        game::WorldObject *createObject(const glm::vec3& pos);

        game::WorldObject *createStaticObject();

        game::WorldObject *createDynamicObject();

        game::WorldObject *createStaticObject(const glm::vec3& pos);

        game::WorldObject *createStaticObject(const game::WorldObject::Description& description);

        game::WorldObject *createDynamicObject(const glm::vec3& pos);

        game::WorldObject *createDynamicObject(const game::WorldObject::Description& description);

        game::WorldObject *createCharacterObject();

        game::WorldObject *createLightObject(const desc::LightDirectionalDesc& description);

        game::WorldObject *createLightObject(const game::desc::LightPointDesc& description);

        game::WorldObject *createLightObject(const game::desc::LightSpotDesc& description);

        void setObjectMesh(game::WorldObject *object, std::shared_ptr<Mesh>& mesh);

        void setObjectModel(game::WorldObject *object, assets::Model *);

        void destroyObject(game::WorldObject *object);

        ex::Entity createTerrain(const std::shared_ptr<ImageData>& image);

        void hideObject(ex::Entity& entity);

        void showObject(ex::Entity& entity);

        void hideObject(game::WorldObject *object);

        void showObject(game::WorldObject *object);

        void spawn(game::WorldObject *object);

        void spawn(game::WorldObject *object, const vec3& position);

        void makeControlled(game::WorldObject *object);

        void makeUnControlled(game::WorldObject *object);

        void createInventory(game::WorldObject *object);

        void assignDefaultWeapon(game::WorldObject *object);

        void forcePush(game::WorldObject *object, const vec3& direction, const float& force);

        assets::Model *findAssetModel(const std::string& name);

    private:
        std::string name;

        std::map<game::WorldObject::Id, game::WorldObject *> objects;

        InputManager *input1 = nullptr;
        InputManager *input2 = nullptr;

        Context context;

        std::vector<game::Script *> scripts;

        std::shared_ptr<Assets> assets;

        boost::lockfree::queue<WorldTask *> tasks;
        boost::lockfree::queue<WorldTask *> completedTasks;

        void reloadAssetsScripts();

        void reloadRenderAssets();

        void unloadScripts();

        void unloadTextures();

        void applyObjectAsset(game::WorldObject *object, assets::Model *asset);
    };
}

#endif
