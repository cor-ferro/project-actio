#ifndef RENDERER_OPENGL_H_
#define RENDERER_OPENGL_H_

#define SECOND 1000.0
#define FPS 60.0

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
#include "../../core/geometry.h"
#include "../../core/mesh.h"
#include "shader.h"
#include "program.h"
#include "g-buffer.h"
#include "u-buffer.h"
#include "../stats.h"
#include "uniforms.h"
#include "utils.h"

#define UBO_MATRICES_POINT_INDEX 0
#define UBO_LIGHTS_POINT_INDEX 1

#define DEFAULT_FRAME_BUFFER 0

namespace Renderer {
	typedef std::function<void()> callback;

	struct OpenglRenderer : BaseRenderer {
		OpenglRenderer(RendererParams);
		~OpenglRenderer();
		bool init(int, char **);
		void start();
		void draw(Scene * scene);
		void drawModels(Scene * scene, Opengl::Program& program);
		void updateTime(int);

		void forwardRender(Scene * scene);
		void defferedRender(Scene * scene);

		void renderPointLights(Scene& scene);
		void renderDirLights(Scene& scene);
		void renderSpotLights(Scene& scene);

		void preRender();
		void postRender();

		void addPreRenderHandler(callback handler);
		void addPostRenderHandler(callback handler);

		double elaspsedTime = 0.0;
		double time = 1.0;

		GLuint depthMapFBO;
		GLuint depthMap;

		GBuffer gbuffer;
		UBuffer matricesBuffer;
		UBuffer lightBuffer;

		Renderer::Stats stats;

		boost::signals2::signal<void ()> onPreRender;
		boost::signals2::signal<void ()> onPostRender;
	private:
		void initMatricesBuffer();

		int timerId;

		Opengl::Program forwardProgram;
		Opengl::Program skyboxProgram;
		Opengl::Program skyboxDeferredProgram;
		Opengl::Program geometryPassProgram;
		Opengl::Program lightPassProgram;
		Opengl::Program nullProgram;

		Mesh * lightQuad;
		Mesh * lightSphere;
		Mesh * lightCylinder;
	};

}

#endif
