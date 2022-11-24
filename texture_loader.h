#pragma once

#include <glew\glew.h>

struct TextureProperties {

	bool		flipImageY;

	TextureProperties(bool flipImageY) {

		this->flipImageY = flipImageY;
	}
};

// FreeImage texture loader
GLuint fiLoadTexture(const char *filename, const TextureProperties& properties);
