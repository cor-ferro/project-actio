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

        void Pipeline::draw(Mesh &mesh, uint flags) {
            draw(program, mesh, flags);
        }

        void Pipeline::draw(Program *drawProgram, Mesh &mesh, uint flags) {
            assert(drawProgram->isSuccess() && "cannot set program");

            if ((flags & Mesh_Draw_Textures) != 0) {
                unsigned int textureIndex = 0;

                const std::shared_ptr<Texture> &diffuseTexture = mesh.material->getDiffuseMap();
                if (diffuseTexture && diffuseTexture->renderHandle != nullptr) {
                    const auto *handle = dynamic_cast<renderer::Opengl::TextureHandle *>(diffuseTexture->renderHandle);
                    if (handle->ready) {
                        OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, handle);
                        drawProgram->setInt(diffuseTexture->name, textureIndex);
                        textureIndex++;
                    }
                }

                const std::shared_ptr<Texture> &normalTexture = mesh.material->getNormalMap();
                if (normalTexture && normalTexture->renderHandle != nullptr) {
                    const auto *handle = dynamic_cast<renderer::Opengl::TextureHandle *>(normalTexture->renderHandle);

                    if (handle->ready) {
                        OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, handle);
                        drawProgram->setInt(normalTexture->name, textureIndex);
                        textureIndex++;
                    }
                }

                const std::shared_ptr<Texture> &specularTexture = mesh.material->getSpecularMap();
                if (specularTexture && specularTexture->renderHandle != nullptr) {
                    const auto *handle = dynamic_cast<renderer::Opengl::TextureHandle *>(specularTexture->renderHandle);

                    if (handle->ready) {
                        OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, handle);
                        drawProgram->setInt(specularTexture->name, textureIndex);
                        textureIndex++;
                    }
                }

                const std::shared_ptr<Texture> &heightTexture = mesh.material->getHeightMap();
                if (heightTexture && heightTexture->renderHandle != nullptr) {
                    const auto *handle = dynamic_cast<renderer::Opengl::TextureHandle *>(heightTexture->renderHandle);

                    if (handle->ready) {
                        OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, handle);
                        drawProgram->setInt(heightTexture->name, textureIndex);
                        textureIndex++;
                    }
                }
            }

            if ((flags & Mesh_Draw_Material) != 0) {
                drawProgram->setVec("material.ambient", mesh.material->getAmbient());
                drawProgram->setVec("material.diffuse", mesh.material->getDiffuse());
                drawProgram->setVec("material.specular", mesh.material->getSpecular());
                drawProgram->setFloat("material.shininess", mesh.material->getShininess());
            }

            if ((flags & Mesh_Draw_Bones) != 0) {
                drawProgram->enableVertexSubroutine("getBoneTransform", "BoneTransformEnabled");
                drawProgram->setMat("boneTransforms[]", &mesh.bones.transforms);
                drawProgram->setMat("boneOffsets[]", &mesh.bones.offsets);
            } else {
                drawProgram->enableVertexSubroutine("getBoneTransform", "BoneTransformDisabled");
            }

            if ((flags & Mesh_Draw_Base) != 0 && mesh.geometry.renderHandle != nullptr) {
                const auto *handle = dynamic_cast<renderer::Opengl::GeometryHandle *>(mesh.geometry.renderHandle);

                if (handle->ready) {
                    glBindVertexArray(handle->vao);

                    object.updateModelMatrix(false);
                    drawProgram->setMat("model", object.getModelMatrix());

                    GeometryVertices *vertices = mesh.geometry.getVertices();
                    GeometryIndices *indices = mesh.geometry.getIndices();

                    GLenum primitiveType;
                    switch (mesh.getPrimitiveType()) {
                        case Mesh_Primitive_Triangle:
                            primitiveType = GL_TRIANGLES;
                            break;
                        case Mesh_Primitive_Triangle_Strip:
                            primitiveType = GL_TRIANGLE_STRIP_ADJACENCY;
                            break;
                        case Mesh_Primitive_Line:
                            primitiveType = GL_LINES;
                            break;
                        case Mesh_Primitive_Line_Loop:
                            primitiveType = GL_LINE_LOOP;
                            break;
                        case Mesh_Primitive_Point:
                            primitiveType = GL_POINT;
                            break;
                        default:
                            primitiveType = GL_TRIANGLES;
                    }

                    MeshDrawMode drawMode = indices->empty() ? Mesh_Draw_Arrays : Mesh_Draw_Elements;

                    if (mesh.material->isWireframe()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                    switch (drawMode) {
                        case Mesh_Draw_Arrays:
                            if (!vertices->empty()) {
                                glDrawArrays(primitiveType, 0, static_cast<int>(vertices->size()));
                            }
                            break;
                        case Mesh_Draw_Elements:
                            glDrawElements(primitiveType, static_cast<int>(indices->size()), GL_UNSIGNED_INT, nullptr);
                            break;
                        default:
                            console::warn("unknown draw mode: %i", drawMode);
                    }

                    if (mesh.material->isWireframe()) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                    glBindVertexArray(0);
                }
            }
        }
    }
}
