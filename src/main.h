#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <memory>

class App;

namespace game {
    class Engine;
}

struct ProgramContext {
    explicit ProgramContext(App& app, std::shared_ptr<game::Engine>& engine) : m_engine(engine), m_app(app) {}

    App& getApp() {
        return m_app;
    };

    std::shared_ptr<game::Engine> &getEngine() {
        return m_engine;
    }

    bool shouldWork = true;

private:
    App& m_app;
    std::shared_ptr<game::Engine> m_engine;
};

int main(int argc, char **argv);

void renderHandler(ProgramContext& context);

void appHandler(ProgramContext& context);
