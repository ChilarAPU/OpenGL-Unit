#pragma once

#pragma once

#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <glew\glew.h>


// Model a simple textured quad oriented to face along the +z axis (so the textured quad faces the viewer in (right-handed) eye coordinate space.  The quad is modelled using VBOs and VAOs and rendered using the basic texture shader in Resources\Shaders\basic_texture.vs and Resources\Shaders\basic_texture.fs

class objImporter : public CoreStructures::GUObject {

private:

	GLuint					objVertexArray;

	GLuint					objVertexBuffer;
	GLuint					objTextureCoordBuffer;

	GLuint					objShader;

	GLuint					texture;

	//
	// Private API
	//

	void loadShader();
	void setupVAO();


	//
	// Public API
	//

public:

	objImporter(const char* texturePath);
	objImporter(GLuint initTexture);

	~objImporter();

	void render(const CoreStructures::GUMatrix4& T);
};
