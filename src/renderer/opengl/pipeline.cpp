#include "pipeline.h"

namespace renderer {
    namespace Opengl {
        Pipeline::Pipeline(Program *newProgram)
                : program(newProgram) {}

        Pipeline::~Pipeline() {

        }

        void Pipeline::setProgram(Program *newProgram) {
            program = newProgram;
        }

        Program *Pipeline::getProgram() {
            return program;
        }

        void Pipeline::setObject(Object3D &newObject) {
            object = newObject;
        }


        MeshDrawPipeline::MeshDrawPipeline()
                : Pipeline() {};

        MeshDrawPipeline::MeshDrawPipeline(Program *newProgram)
                : Pipeline(newProgram) {};

        void MeshDrawPipeline::use() {
            program->use();
        }

        void MeshDrawPipeline::draw(Mesh &mesh, uint flags) {
            draw(program, mesh, flags);
        }

        void MeshDrawPipeline::draw(Program *drawProgram, Mesh &mesh, uint flags) {
            drawProgram->use();
            assert(!drawProgram->isSuccess() && "cannot set program");

            if ((flags & Mesh_Draw_Textures) != 0) {
                unsigned int textureIndex = 0;
                const MaterialTextures &textures = mesh.material.getTextures();
                for (const Texture &texture : textures) {
                    OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, texture);
                    drawProgram->setInt(texture.name, textureIndex);
                    textureIndex++;
                }
            }

            if ((flags & Mesh_Draw_Material) != 0) {
                drawProgram->setVec(renderer::Opengl::Uniform::MaterialAmbient, mesh.material.ambient);
                drawProgram->setVec(renderer::Opengl::Uniform::MaterialDiffuse, mesh.material.diffuse);
                drawProgram->setVec(renderer::Opengl::Uniform::MaterialSpecular, mesh.material.specular);
                drawProgram->setFloat(renderer::Opengl::Uniform::MaterialShininess, mesh.material.shininess);
            }

            if ((flags & Mesh_Draw_Bones) != 0) {
                drawProgram->enableVertexSubroutine("BoneTransformEnabled");
                drawProgram->setMat("bones[]", &mesh.boneTransforms);
            } else {
                drawProgram->enableVertexSubroutine("BoneTransformDisabled");
            }

            if ((flags & Mesh_Draw_Base) != 0) {
                glBindVertexArray(mesh.geometry.VAO);

                object.updateModelMatrix(false);
                drawProgram->setMat("model", object.getModelMatrix());

                GeometryVertices *vertices = mesh.geometry.getVertices();
                GeometryIndices *indices = mesh.geometry.getIndices();

                GLenum primitiveType;
                switch (mesh.getDrawType()) {
                    case Mesh_Draw_Line:
                        primitiveType = GL_LINES;
                        break;
                    case Mesh_Draw_Line_Loop:
                        primitiveType = GL_LINE_LOOP;
                        break;
                    case Mesh_Draw_Triangle:
                        primitiveType = GL_TRIANGLES;
                        break;
                    default:
                        primitiveType = GL_TRIANGLES;
                }

                MeshDrawMode drawMode = indices->empty() ? Mesh_Draw_Arrays : Mesh_Draw_Elements;

                if (mesh.material.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                switch (drawMode) {
                    case Mesh_Draw_Arrays:
                        glDrawArrays(primitiveType, 0, static_cast<int>(vertices->size()));
                        break;
                    case Mesh_Draw_Elements:
                        glDrawElements(primitiveType, static_cast<int>(indices->size()), GL_UNSIGNED_INT, nullptr);
                        break;
                    default:
                        console::warn("unknown draw mode: %i", drawMode);
                }

                if (mesh.material.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glBindVertexArray(0);
            }
        };

        void SkyboxPipeline::setTexture() {
//            OpenglUtils::bindTexture(GL_TEXTURE0 + maxTextureUnits - 1, texture);
//            skyboxDeferredProgram.setInt("cubeTexture", maxTextureUnits - 1);
        }
    }
}
