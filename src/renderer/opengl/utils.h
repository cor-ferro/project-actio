
#ifndef SRC_RENDERER_OPENGL_UTILS_H_
#define SRC_RENDERER_OPENGL_UTILS_H_

#include <glad/glad.h>
#include "../../core/texture.h"

namespace OpenglUtils {
	GLenum textureTarget(const Texture& texture);
	void bindTexture(const Texture& texture);
	void bindTexture(GLenum targetUnit, const Texture& texture);
	void checkErrors(const char * file, int line, bool silent = false);
}

#define OpenglCheckErrors() OpenglUtils::checkErrors(__FILE__, __LINE__);
#define OpenglCheckErrorsSilent() OpenglUtils::checkErrors(__FILE__, __LINE__, true);

#endif
