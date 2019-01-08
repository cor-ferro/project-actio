#include "init.h"
#include "shader_content.h"
#include <boost/filesystem/string_file.hpp>

renderer::Renderer* renderer::create(renderer::Params &params) {
    std::map<std::string, RawProgram> programs;
    loadPrograms(programs);

    Renderer *renderer = new OpenglRenderer(params);
    renderer->addPrograms(programs);

    return renderer;
}

void renderer::destroy(renderer::Renderer *renderer) {
    if (renderer != nullptr) {
        renderer->destroy();
        delete renderer;
    }
}

void renderer::loadPrograms(std::map<std::string, RawProgram> &programs) {
    Path folder("resources/required-shaders");

    fs::directory_iterator itr_end;
    std::map<std::string, std::vector<std::string>> shaders;

    for (fs::directory_iterator itr(folder); itr != itr_end; ++itr) {
        if (fs::is_regular_file(itr->path())) {
            std::string shaderName = itr->path().stem().string();
            std::string shaderType = itr->path().extension().string();

            shaderType.erase(0, 1);

            if (shaders.find(shaderName) == shaders.end()) {
                shaders[shaderName] = { shaderType };
            } else {
                shaders[shaderName].push_back(shaderType);
            }
        }
    }

    for (const auto &it : shaders) {
        programs.insert({it.first, RawProgram()});

        auto &rawProgram = programs.find(it.first)->second;

        const std::string name = it.first;
        const std::vector<std::string> &types = it.second;

        rawProgram.setName(name);

        for (const auto &type : types) {
            Path shaderFilePath = createPath(folder, name);
            shaderFilePath.replace_extension(type.c_str());

            if (RawProgram::isValidType(type)) {
                ShaderContent &shaderContent = rawProgram.getShader(type);
                std::shared_ptr<std::string> content = shaderContent.getContent();

                boost::filesystem::load_string_file(shaderFilePath, *content);
            }
        }
    }
}