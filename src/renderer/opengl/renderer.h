#ifndef RENDERER_OPENGL_H_
#define RENDERER_OPENGL_H_

#define GLEW_STATIC

#define SECOND 1000.0
#define FPS 60.0

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/signals2.hpp>
#include "../../lib/console.h"
#include "../../lib/input_handler.h"
#include "../base_renderer.h"
#include "../../scenes/scene.h"
#include "../../cameras/camera.h"
#include "../../lib/types.h"
#include "shader.h"
#include "program.h"
#include "g-buffer.h"
#include "u-buffer.h"
#include "../stats.h"
#include "uniforms.h"

#define UBO_MATRICES_POINT_INDEX 0
#define UBO_LIGHTS_POINT_INDEX 1

namespace Renderer {

static int globalTimerId = 1;
static int winId;

typedef boost::chrono::system_clock Chrono;
typedef std::function<void()> callback;

struct OpenglRenderer : BaseRenderer {
	OpenglRenderer(RendererParams);
	~OpenglRenderer();
	bool init(int, char **);
	void start();
	void draw(Scene * scene);
	void updateTime(int);
	void setTitle(const char * text);

	void forwardRender(Scene * scene);
	void defferedRender(Scene * scene);

	void preRender();
	void postRender();

	void addPreRenderHandler(callback handler);
	void addPostRenderHandler(callback handler);

	size_t elaspsedTime = 0;
	size_t time = 1000;

	GBuffer gbuffer;
	UBuffer matricesBuffer;
	UBuffer lightBuffer;

	Renderer::Stats stats;

private:
	void initLightsBuffer();
	void initMatricesBuffer();

	int timerId;
	boost::signals2::signal<void ()> preFrameSignal_;
	boost::signals2::signal<void ()> postFrameSignal_;
};

static OpenglRenderer * currentRenderer;

static void drawFunction();
//static void timerFunction(int);
static void resizeFunction(int width, int height);

}

#endif
