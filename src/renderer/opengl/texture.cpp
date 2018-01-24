#include "texture.h"

OpenglTexture::OpenglTexture()
	: textureId(0)
	, textureTarget(GL_TEXTURE_2D)
{}

OpenglTexture::OpenglTexture(const OpenglTexture& other)
	: textureId(other.textureId)
	, textureTarget(other.textureTarget)
{}

//static OpenglTexture OpenglTexture::create2d()
//{
//	OpenglTexture texture;
//
//	texture.setTarget(GL_TEXTURE_2D);
//
//	return texture;
//}
//
//static OpenglTexture OpenglTexture::craeteCube()
//{
//	OpenglTexture texture;
//
//	texture.setTarget(GL_TEXTURE_CUBE_MAP);
//
//	return texture;
//}

void OpenglTexture::destroy()
{
	if (textureId != 0) {
		glDeleteTextures(1, &textureId);
	}
}

GLuint OpenglTexture::getId() const
{
	return textureId;
}

GLenum OpenglTexture::getTarget() const
{
	return textureTarget;
}

void OpenglTexture::setId(GLuint id)
{
	textureId = id;
}

void OpenglTexture::setTarget(GLenum target)
{
	textureTarget = target;
}

void OpenglTexture::setup2d(const ImageLoader::Data& imageData)
{
	glGenTextures(1, &textureId);
	setId(textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageData.width, imageData.height, 0, imageData.format, GL_UNSIGNED_BYTE, imageData.get());

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenglTexture::setupCube(std::unordered_map<char, ImageLoader::Data>& imageDataList)
{
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	for (int i = 0; i < 6; i++) {
		const std::unordered_map<char, ImageLoader::Data>::const_iterator got = imageDataList.find(i);

		if (got == imageDataList.end()) {
			console::warn("cannot get image map", i);
			continue;
		}

		ImageLoader::Data imageData = got->second;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageData.width, imageData.height, 0, imageData.format, GL_UNSIGNED_BYTE, imageData.get());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

