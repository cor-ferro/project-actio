#ifndef APP_H_
#define APP_H_

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <boost/filesystem/path.hpp>
#include "../lib/console.h"
#include "../lib/path.h"
#include "../lib/app_paths.h"
#include "../resources/file_resource.h"

#include "monitor.h"

#include "../lib/assets.h"

static void w1ErrorCallback(int error, const char *description) {
    console::err("GLFW error: %s", description);
}

class App {
public:
    typedef std::unordered_map<void *, Monitor> Monitors;

    explicit App(int argc, char **argv);

    App(App const&) = delete;

    App& operator=(App const&) = delete;

    void InitGLFW();

    void TerminateGLFW();

    const std::string getName() const;

    Path getPath();

    Path resourcePath() const;

    Path resourcePath(const std::string& fromPath) const;

    const resources::File resource(const std::string& path) const;

    void setName(const std::string& name);

    const Monitor *const getPrimaryMonitor();

    const AppPaths& getPaths() const;

    static void LoadBaseAssets(const AppPaths& appPaths);

    static void UnloadBaseAssets();

    static std::shared_ptr<Assets>& GetBaseAssets();

//    const Monitors& getMonitors();

private:
    std::string m_name;
    Path m_path;

    Monitors monitors;

    void addMonitor(GLFWmonitor *monitor);

    void removeMonitor(GLFWmonitor *monitor);

    int argc_;
    char **argv_;

    AppPaths paths;
};


#endif /* APP_H_ */
