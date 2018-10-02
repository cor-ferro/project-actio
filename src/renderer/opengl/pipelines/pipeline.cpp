#include "pipeline.h"
#include "../handle.h"

namespace renderer {
    namespace Opengl {
        Pipeline::Pipeline() : object(initialObject) {};

        Pipeline::Pipeline(Program *newProgram)
            : object(initialObject)
            , program(newProgram) {}

        Pipeline::~Pipeline() = default;

        void Pipeline::use() {
            program->use();
        }

        void Pipeline::setProgram(Program *newProgram) {
            program = newProgram;
        }

        Program *Pipeline::getProgram() {
            return program;
        }

        void Pipeline::setTransform(const Object3D &newObject) {
            object = newObject;
        }

        void Pipeline::setInitialTransform() {
            object = Object3D();
        }

    }
}
