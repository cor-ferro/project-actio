//
// Created by demitriy on 3/26/18.
//

#ifndef ACTIO_PIPELINE_H
#define ACTIO_PIPELINE_H

#include "../../../core/mesh.h"
#include "../../../core/object3D.h"
#include "../../../core/material.h"
#include "../program.h"
#include "../shader.h"
#include "../utils.h"

namespace renderer {
    namespace Opengl {
        struct Pipeline {
            Pipeline();

            virtual ~Pipeline() = 0;

            explicit Pipeline(Program *newProgram);

            void use();

            void setProgram(Program *newProgram);

            Program *getProgram();

            void setTransform(const Object3D &object);

            void setInitialTransform();

            virtual void draw(Mesh &mesh, uint flags) = 0;

            virtual void draw(Program *program, Mesh &mesh, uint flags) = 0;

        protected:
            renderer::Opengl::Program *program = nullptr;
            Object3D &object;
            Object3D initialObject;
        };
    }
}

#endif //ACTIO_PIPELINE_H
