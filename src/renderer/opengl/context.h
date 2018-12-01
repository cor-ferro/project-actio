//
// Created by demitriy on 10/11/18.
//

#ifndef ACTIO_RENDERER_OPENGL_CONTEXT_H
#define ACTIO_RENDERER_OPENGL_CONTEXT_H

#include <string>
#include <set>
#include "program.h"
#include "handle.h"

namespace renderer {
    namespace Opengl {
        class Context {
        public:
            Context() = default;

            ~Context();

            Opengl::Program& getProgram(const std::string& name);

            Opengl::Program *findProgram(const std::string& name);

            bool hasProgram(const std::string& name);

            void createNewProgram(const std::string& name);

            void addHandle(Opengl::GeometryHandle *);

            void addHandle(Opengl::TextureHandle *);

        private:

            std::set<Opengl::GeometryHandle *> geometryHandles;
            std::set<Opengl::TextureHandle *> textureHandles;

            std::map<std::string, Opengl::Program> programs;
        };
    }
}

#endif //ACTIO_RENDERER_OPENGL_CONTEXT_H
