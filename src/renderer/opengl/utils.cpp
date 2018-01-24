#include "utils.h"

namespace OpenglUtils {
	GLenum textureTarget(const Texture& texture)
	{
		GLenum target;

		switch (texture.type) {
		case Texture_Diffuse:
		case Texture_Specular:
		case Texture_Height:
		case Texture_Normal:
			target = GL_TEXTURE_2D; break;
		case Texture_Cube:
			target = GL_TEXTURE_CUBE_MAP; break;
		default:
			target = GL_TEXTURE_2D;
		}

		return target;
	}

	void bindTexture(const Texture& texture)
	{
		glBindTexture(textureTarget(texture), texture.gTexture.getId());
	}

	void bindTexture(GLenum targetUnit, const Texture& texture)
	{
		glActiveTexture(targetUnit);
		bindTexture(texture);
	}

	void checkErrors(const char * file, int line, bool silent) {
		GLenum err = glGetError();

		if (silent == true) return;
		if (err == GL_NO_ERROR) return;

		std::string error;

		switch(err) {
			case GL_INVALID_OPERATION:      		error = "INVALID_OPERATION";      			break;
			case GL_INVALID_ENUM:           		error = "INVALID_ENUM";           			break;
			case GL_INVALID_VALUE:          		error = "INVALID_VALUE";          			break;
			case GL_OUT_OF_MEMORY:          		error = "OUT_OF_MEMORY";          			break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; 	break;
			default: error = "unknown gl error";
		}

		if (error.size() != 0) {
			console::errp("GL: %s [%s:%i]", error.c_str(), file, line);
		}
	}
}
