#ifndef G_BUFFER_H_
#define G_BUFFER_H_

#include <vector>
#include <glad/glad.h>
#include "../renderer_types.h"
#include "../../lib/console.h"
#include "../../lib/utils.h"

namespace Renderer {

class GBuffer {
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_ALBEDO,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();

	bool init(Renderer::ScreenSize width, Renderer::ScreenSize height);

	const std::vector<GLuint>& getTextures();

	void bindForWriting();
	void bindForReading();
	void setReadBuffer(GBUFFER_TEXTURE_TYPE);

	void startFrame();
	void geometryPass();
	void lightPass();
	void stencilPass();
	void finalPass();

	GLuint fbo;

private:
	std::vector<GLuint> textures;
	GLuint rboDepth;
	GLuint depthTexture;
	GLuint finalTexture;
	
};

}



#endif /* G_BUFFER_H_ */
