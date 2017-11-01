/*
 * base_renderer.h
 *
 *  Created on: Jul 20, 2017
 *      Author: demitriy
 */

#ifndef BASE_RENDERER_H_
#define BASE_RENDERER_H_

#include <vector>
#include <functional>
#include <algorithm>
#include "../scenes/scene.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "renderer_types.h"

typedef std::function<void()> callback;

struct RendererParams {
	Renderer::ScreenSize width;
	Renderer::ScreenSize height;
	float aspectRatio;

	void calcAspectRatio();
};

struct BaseRenderer {
	BaseRenderer();
	BaseRenderer(RendererParams);

	const RendererParams& getParams();

	Scene * getScene();
	void setScene(Scene *);
	void setupScene();

	void setViewSize(Renderer::ScreenSize, Renderer::ScreenSize);

	void addFrameHandler(callback cb);
	void doFrameHandlers();

private:
	RendererParams params_;
	Scene * scene_;
	std::vector<callback> frameHandlers_;
};



#endif /* BASE_RENDERER_H_ */
