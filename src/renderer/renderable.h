#ifndef RENDERER_RENDERABLE_H_
#define RENDERER_RENDERABLE_H_

struct Renderable {
    virtual ~Renderable() {};
    virtual void beforeRender() = 0;
    virtual void afterRender() = 0;
};

#endif