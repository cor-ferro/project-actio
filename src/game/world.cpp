#include "world.h"

namespace game {
    void World::setup() {
        systems.add<systems::Render>(m_context);
        systems.add<systems::Input>(m_context);
        systems.add<systems::Camera>(m_context);
        systems.add<systems::CharControl>(m_context);
        systems.add<systems::Animations>(m_context);
        systems.add<systems::Physic>(m_context);
//        systems.add<systems::BallShot>(m_context);
//        systems.add<systems::Lights>(m_context);
//        systems.add<systems::LightHelpers>(m_context);
//        systems.add<systems::DayTime>(m_context);
//        systems.add<systems::Sky>(m_context);
//            systems.add<systems::Weapons>(m_context);

        systems.configure();
    }

    void World::start() {
        ex::TimeDelta dt = 0.16;

        systems.system<systems::Render>()->start(entities, events, dt);
        systems.system<systems::Input>()->start(entities, events, dt);
        systems.system<systems::Camera>()->start(entities, events, dt);
        systems.system<systems::CharControl>()->start(entities, events, dt);
        systems.system<systems::Animations>()->start(entities, events, dt);
        systems.system<systems::Physic>()->start(entities, events, dt);
//        systems.system<systems::BallShot>()->start(entities, events, dt);
//        systems.system<systems::Lights>()->start(entities, events, dt);
//        systems.system<systems::LightHelpers>()->start(entities, events, dt);
//        systems.system<systems::DayTime>()->start(entities, events, dt);
//        systems.system<systems::Sky>()->start(entities, events, dt);
//            systems.system<systems::Weapons>()->start(entities, events, dt);
    }

    void World::update(const ex::TimeDelta& dt) {
        // pre update
        PROFILE(systemsProfiler, "Input", systems.update<game::systems::Input>(dt));
        PROFILE(systemsProfiler, "Camera", systems.update<game::systems::Camera>(dt));
        PROFILE(systemsProfiler, "CharControl", systems.update<game::systems::CharControl>(dt));
        PROFILE(systemsProfiler, "Physic", systems.update<game::systems::Physic>(dt));
//        PROFILE(systemsProfiler, "Lights", systems.update<game::systems::Lights>(dt));
//        PROFILE(systemsProfiler, "DayTime", systems.update<game::systems::DayTime>(dt));
//        PROFILE(systemsProfiler, "Sky", systems.update<game::systems::Sky>(dt));
//
//            // main update
//            PROFILE(systemsProfiler, "Weapons", systems.update<game::systems::Weapons>(dt));
//        PROFILE(systemsProfiler, "BallShot", systems.update<game::systems::BallShot>(dt));
//
////    for (auto &script : scripts) {
////        PROFILE(systemProfiler, script->getName(), script->evalUpdate(static_cast<float>(dt)));
////    }
//
//            // post update
//        PROFILE(systemsProfiler, "Animations", systems.update<game::systems::Animations>(dt));
//        PROFILE(systemsProfiler, "LightHelpers", systems.update<game::systems::LightHelpers>(dt));
    }

    void World::render(const ex::TimeDelta& dt) {
        PROFILE(systemsProfiler, "Render", systems.update<game::systems::Render>(dt));

        bool demo = false;

        ImGui_ImplGlfwGL3_NewFrame();


        if (!demo) {
            float height = 50.0f;
            ImGui::SetNextWindowPos(ImVec2(0.0f, static_cast<float>(m_context.window().getHeight()) - height), true);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                                     | ImGuiWindowFlags_NoMove
                                     | ImGuiWindowFlags_NoResize
                                     | ImGuiWindowFlags_NoBringToFrontOnFocus;

            const auto width = static_cast<float>(m_context.window().getWidth());//400.0f;
            const float totalMs = 1000.0f * 16.6f;

            ImGui::Begin("Metrics", nullptr, flags);
            auto profilerDurations = systemsProfiler.getDurations(profiling::Microseconds);

            int columns = static_cast<int>(profilerDurations.results.size());
            ImGui::Columns(columns, nullptr, false);

            int i = 0;
            for (const auto& p : profilerDurations.results) {
                auto columnWidth = std::max(0.0f, width * static_cast<float>(p.second / profilerDurations.total));

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
                ImGui::SetColumnWidth(ImGui::GetColumnIndex(), columnWidth);

                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i/7.0f, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i/7.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i/7.0f, 0.8f, 0.8f));

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

                ImGui::Button(p.first.c_str(), ImVec2(columnWidth, 20));

                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(1);
                ImGui::PopStyleVar(1);
                ImGui::NextColumn();
                i++;
            }

            ImGui::SetWindowSize(ImVec2(width, height));

            ImGui::End();
        } else {
            ImGui::ShowDemoWindow();
        }

        ImGui::Render();
    }

    void World::setRenderer(std::shared_ptr<renderer::Renderer>& renderer) {
        systems.system<systems::Render>()->setRenderer(renderer);
    }

    ex::Entity World::createEntity() {
        return entities.create();
    }
}