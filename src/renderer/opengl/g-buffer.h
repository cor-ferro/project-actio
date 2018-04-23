#ifndef G_BUFFER_H_
#define G_BUFFER_H_

#include <vector>
#include <glad/glad.h>
#include "../renderer_types.h"
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

			bool init(renderer::ScreenSize width, renderer::ScreenSize height);

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

			renderer::ScreenSize getWidth();
			renderer::ScreenSize getHeight();

			void setWidth(renderer::ScreenSize newWidth);
			void setHeight(renderer::ScreenSize newHeight);

			GLuint fbo;

		private:
			renderer::ScreenSize width;
			renderer::ScreenSize height;

			std::vector<GLuint> textures;
			GLuint rboDepth;
			GLuint depthTexture = 0;
			GLuint finalTexture = 0;

		};
	}

}



#endif /* G_BUFFER_H_ */
