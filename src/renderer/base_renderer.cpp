#include "base_renderer.h"

BaseRenderer::BaseRenderer()
{
	RendererParams renderParams;
	renderParams.width = 500;
	renderParams.height = 500;

	params_ = renderParams;
}

BaseRenderer::BaseRenderer(RendererParams params)
	: params_(params)
{
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
