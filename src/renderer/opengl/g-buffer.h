#ifndef G_BUFFER_H_
#define G_BUFFER_H_

#include <vector>
#include <glad/glad.h>
#include "../params.h"
#include "../../lib/console.h"
#include "../../lib/utils.h"

namespace renderer {
	namespace Opengl {
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

			bool init(renderer::Dimension width, renderer::Dimension height);

			const std::vector<GLuint>& getTextures();

			void bindForWriting();
			void bindForReading();
			void setReadBuffer(GBUFFER_TEXTURE_TYPE);

			void startFrame();
			void geometryPass();
			void lightPass();
			void stencilPass();
			void finalPass();

			void generateTextures();

			renderer::Dimension getWidth();
			renderer::Dimension getHeight();

			void setWidth(renderer::Dimension newWidth);
			void setHeight(renderer::Dimension newHeight);

			void setFinalTexture(const GBUFFER_TEXTURE_TYPE& type);

			GLuint fbo;

		private:
			void copyToDefaultBuffer();

			renderer::Dimension width;
			renderer::Dimension height;

			std::vector<GLuint> textures;
			GLuint rboDepth;
			GLuint depthTexture = 0;
			GLuint finalTexture = 0;

			GLenum finalTextureType = GL_COLOR_ATTACHMENT4;

		};
	}

}



#endif /* G_BUFFER_H_ */
