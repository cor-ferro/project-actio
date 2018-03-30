//
// Created by demitriy on 3/26/18.
//

#ifndef ACTIO_PIPELINE_H
#define ACTIO_PIPELINE_H

#include "../../core/mesh.h"
#include "../../core/object3D.h"
#include "../../materials/material.h"
#include "program.h"
#include "shader.h"
#include "utils.h"

namespace renderer {
    namespace Opengl {
        struct Pipeline {
            Pipeline() = default;

            virtual ~Pipeline() = 0;

            explicit Pipeline(Program *newProgram);

            void setProgram(Program *newProgram);

            Program *getProgram();

            void setObject(Object3D &object);

            virtual void draw(Mesh &mesh, uint flags = Mesh_Draw_All) = 0;
            virtual void draw(Program *program, Mesh &mesh, uint flags = Mesh_Draw_All) = 0;

        protected:
            renderer::Opengl::Program *program;
            Object3D object;
        };

        struct FixedPipeline : Pipeline {
            explicit FixedPipeline(Program *newProgram) : Pipeline(newProgram) {};
        private:
            FixedPipeline() = default;

            void setProgram(Program *newProgram) {};
        };

        struct MeshDrawPipeline : Pipeline {
            MeshDrawPipeline();

            explicit MeshDrawPipeline(Program *newProgram);

            ~MeshDrawPipeline() = default;

            void use();

            void draw(Mesh &mesh, uint flags = Mesh_Draw_All) override;
            void draw(Program *program, Mesh &mesh, uint flags = Mesh_Draw_All) override;
        };

        struct SkyboxPipeline : Pipeline {
            void setTexture();
        };
    }
}

#endif //ACTIO_PIPELINE_H
