//
// Created by demitriy on 10/6/18.
//

#ifndef ACTIO_FRAME_CONTEXT_H
#define ACTIO_FRAME_CONTEXT_H

#include <entityx/Entity.h>
#include "params.h"
#include "../lib/types.h"


namespace renderer {
    struct FrameContext {
        explicit FrameContext(entityx::EntityManager &es);

        mat4 view;
        mat4 projection;
        vec3 cameraPosition;
        vec3 cameraRotation;
        mat4 cameraRotationMat;
        entityx::EntityManager &es;
        Dimension height = 0;
        Dimension width = 0;

        bool shadowsEnabled = false;

        bool isShadowsEnabled() const;

        void setShadowsEnabled(bool shadowsEnabled);

        size_t drawCalls = 0;

        const Dimension &getWidth() const;

        void setWidth(Dimension screenWidth);

        const Dimension &getHeight() const;

        void setHeight(Dimension screenHeight);

        void setView(const mat4 &value);

        void setProjection(const mat4 &value);

        void setCameraPosition(const vec3 &value);

        void setCameraRotation(const vec3 &value);

        void setCameraRotationMat(const mat4 &value);

        const mat4 &getView() const;

        const mat4 &getProjection() const;

        const vec3 &getCameraPosition() const;

        const vec3 &getCameraRotation() const;

        const mat4 &getCameraRotationMat() const;

        entityx::EntityManager &getEntityManager() const;
    };
}

#endif //ACTIO_FRAME_CONTEXT_H
