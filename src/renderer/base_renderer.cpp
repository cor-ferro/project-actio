#include "base_renderer.h"

BaseRenderer::BaseRenderer()
{
	RendererParams renderParams;
	renderParams.width = 500;
	renderParams.height = 500;

	params_ = renderParams;
	scene_ = 0;
}

BaseRenderer::BaseRenderer(RendererParams params)
: params_(params)
{
	scene_ = 0;
}

const RendererParams& BaseRenderer::getParams()
{
	return params_;
}

void BaseRenderer::setViewSize(Renderer::ScreenSize width, Renderer::ScreenSize height)
{
	params_.width = width;
	params_.height = height;
	params_.calcAspectRatio();
}

Scene * BaseRenderer::getScene()
{
	return scene_;
}

void BaseRenderer::setScene(Scene * scene)
{
	scene_ = scene;
}

void BaseRenderer::setupScene()
{
	if (!scene_) {
		console::warn("Cannot setup scene. Pointer is null");
		return;
	}

	std::vector<Model*>* models = scene_->getModels();

	for (auto model = models->begin(); model != models->end(); model++) {
		std::vector<Mesh*>* meshes = (*model)->getMeshes();

		for(auto mesh = meshes->begin(); mesh != meshes->end(); mesh++)
		{
			// (*mesh)->setup();
		}
	}
}

void BaseRenderer::addFrameHandler(callback cb)
{
	frameHandlers_.push_back(cb);
}

void BaseRenderer::doFrameHandlers()
{
	std::for_each(frameHandlers_.begin(), frameHandlers_.end(), [](callback cb) {
		cb();
	});
}

void RendererParams::calcAspectRatio()
{
	aspectRatio = (float)width / (float)height;
}
