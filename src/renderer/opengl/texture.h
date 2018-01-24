#ifndef SRC_RENDERER_OPENGL_TEXTURE_H_
#define SRC_RENDERER_OPENGL_TEXTURE_H_

#include <glad/glad.h>
#include "../../lib/image_loader.h"

struct OpenglTexture {
	OpenglTexture();
	OpenglTexture(const OpenglTexture& other);
//	static OpenglTexture create2d();
//	static OpenglTexture craeteCube();

	GLuint getId() const;
	GLenum getTarget() const;

	void setId(GLuint id);
	void setTarget(GLenum target);

	void setup2d(const ImageLoader::Data& imageData);
	void setupCube(std::unordered_map<char, ImageLoader::Data>& imageDataList);
	void destroy();

	private:
		GLuint textureId;
		GLenum textureTarget;
};



#endif /* SRC_RENDERER_OPENGL_TEXTURE_H_ */
