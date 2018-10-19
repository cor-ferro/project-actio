#include "frame_context.h"
#include "../game/components/camera.h"
#include "../game/components/transform.h"
#include "../game/components/target.h"

namespace renderer {
    FrameContext::FrameContext(entityx::EntityManager &es) : es(es) {};

    const Dimension &FrameContext::getWidth() const {
        return width;
    }

    void FrameContext::setWidth(Dimension width) {
        FrameContext::width = width;
    }

    const Dimension &FrameContext::getHeight() const {
        return height;
    }

    void FrameContext::setHeight(Dimension height) {
        FrameContext::height = height;
    }

    void FrameContext::setView(const mat4 &value) {
        view = value;
    }

    void FrameContext::setProjection(const mat4 &value) {
        projection = value;
    };

    void FrameContext::setCameraPosition(const vec3 &value) {
        cameraPosition = value;
    }

    void FrameContext::setCameraRotation(const vec3 &value) {
        cameraRotation = value;
    }

    void FrameContext::setCameraRotationMat(const mat4 &value) {
        cameraRotationMat = value;
    };

    const mat4 &FrameContext::getView() const {
        return view;
    }

    const mat4 &FrameContext::getProjection() const {
        return projection;
    }

    const vec3 &FrameContext::getCameraPosition() const {
        return cameraPosition;
    }

    const vec3 &FrameContext::getCameraRotation() const {
        return cameraRotation;
    }

    const mat4 &FrameContext::getCameraRotationMat() const {
        return cameraRotationMat;
    }

    entityx::EntityManager &FrameContext::getEntityManager() const {
        return es;
    }

    bool FrameContext::isShadowsEnabled() const {
        return shadowsEnabled;
    }

    void FrameContext::setShadowsEnabled(bool shadowsEnabled) {
        FrameContext::shadowsEnabled = shadowsEnabled;
    }
}