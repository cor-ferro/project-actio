#ifndef BASE_RENDERER_H_
#define BASE_RENDERER_H_

#include <vector>
#include <functional>
#include <algorithm>
#include <boost/signals2.hpp>
#include "../game/components/model.h"
#include "../game/components/transform.h"
#include "../scenes/scene.h"
#include "../lib/path.h"
#include "stats.h"
#include "renderer_types.h"
#include "entityx/entityx.h"

namespace renderer {
    enum RenderType {
        RenderForward,
        RenderDeferred
    };

    struct Renderer {
        Renderer();

        Renderer(renderer::Params);

        virtual ~Renderer() = 0;

        virtual bool init() = 0;

        virtual void draw(Scene *scene) = 0;

        virtual void draw(entityx::EntityManager &es) = 0;

        virtual void setupMesh(Mesh *mesh) = 0;

        virtual void destroyMesh(Mesh *mesh) = 0;

        const renderer::Params &getParams();

        void setViewSize(renderer::ScreenSize, renderer::ScreenSize);

        void setType(RenderType newType);

        void setShadersFolder(Path path);

        void preRender();

        void postRender();

    protected:
        RenderType type;
        Path shadersFolder;
        Stats stats;

        double elaspsedTime = 0.0;
        double time = 1.0;

        boost::signals2::signal<void()> onPreRender;
        boost::signals2::signal<void()> onPostRender;

    private:
        renderer::Params params_;

    };
}


#endif /* BASE_RENDERER_H_ */
