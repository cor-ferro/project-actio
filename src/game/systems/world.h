#ifndef ACTIO_WORLD_H
#define ACTIO_WORLD_H

#include <stack>
#include <deque>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/System.h>

#include "base.h"
#include "../events/load_story_complete.h"
#include "extensions/PxRevoluteJoint.h"

namespace game {
    namespace systems {
        namespace px = physx;
        namespace ex = entityx;

        class World : public systems::BaseSystem, public entityx::System<World>, public ex::Receiver<World> {
        public:
            explicit World(Context& context);

            ~World() override;

            void configure(ex::EntityManager &entities, entityx::EventManager &event_manager) override;

            void start(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::LoadStoryComplete& event);

        private:
            struct Story {};

            class Aircraft {
            public:
                Aircraft() = default;

                Aircraft(Aircraft && other) noexcept;

                ~Aircraft();

                ex::Entity entity;

                struct Wing {
                    ex::Entity entity;
                    float angle = 0.0f;
                };

                class Thruster {
                public:
                    void increaseRate(float value);

                    void decreaseRate(float value);

                    void decreaseRate();

                    const float getPower();

                    const float& getMaxPower();

                private:
                    float overheat = 0.0f; // должно влиять на общую мощность
                    float angle = 0.0f;
                    const float maxPower = 1000.0f;
                    float rate = 0.0f; // 0.0f - 1.0f
                };

                struct Shield {
                    float angle = 3.14f;
                    float power = 1.0f;
                };

                struct Weapon {
                    float overheat = 0.0f;
                };

                struct Energy {
                    float amount = 0.0f;
                };

                // Thrusters
                struct Thrusters {
                    Thruster back;

                    Thruster front;

                    Thruster left;

                    Thruster right;

                    Thruster vertical;
                };

                // Wings
                struct Wings {
                    // Wings
                    Wing left;

                    Wing right;
                };

                struct Shields {
                    Shield front;
                };

                struct Weapons {
                    Weapon main;
                };

                Wings wings;

                Thrusters thrusters;

                Shields shields;

                Energy energy;

                std::vector<px::PxJoint*> joints;

                float getMass();
            };

            void buildAircraft(ex::EntityManager &es, ex::EventManager &events);

            void buildHall(ex::EntityManager &es, ex::EventManager &events);

            std::stack<Story> loadedStory;

            std::deque<Aircraft> aircrafts;
        };
    }
}

#endif //ACTIO_WORLD_H
