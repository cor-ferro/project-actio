#ifndef G_BUFFER_H_
#define G_BUFFER_H_

#include <GL/glew.h>
#include "../renderer_types.h"
#include "../../lib/console.h"
#include "../../lib/utils.h"

namespace Renderer {

class GBuffer {
public:
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();

	bool init(Renderer::ScreenSize width, Renderer::ScreenSize height);

	void bindForWriting();
	void bindForReading();
	void setReadBuffer(GBUFFER_TEXTURE_TYPE);

private:
	GLuint fbo;
	GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthTexture;
};

}



#endif /* G_BUFFER_H_ */
