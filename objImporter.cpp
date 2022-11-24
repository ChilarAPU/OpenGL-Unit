
#include "objImporter.h"
#include "src/shader_setup.h"
#include "src/texture_loader.h"
#include "src/aiWrapper.h"


using namespace std;
using namespace CoreStructures;


// Geometry data for textured quad (this is rendered directly as a triangle strip)

static float quadPositionArray[] = {

	-0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f
};

static float quadTextureCoordArray[] = {

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
};



//
// Private API
//

void objImporter::loadShader() {

	// setup shader for textured quad
	objShader = setupShaders(string("Shaders\\basic_texture.vs"), string("Shaders\\basic_texture.fs"));
}


void objImporter::setupVAO() {

	// setup VAO for textured quad object
	glGenVertexArrays(1, &objVertexArray);
	glBindVertexArray(objVertexArray);


	// setup vbo for position attribute
	glGenBuffers(1, &objVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositionArray), quadPositionArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);


	// setup vbo for texture coord attribute
	glGenBuffers(1, &objTextureCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, objTextureCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordArray), quadTextureCoordArray, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	// enable vertex buffers for textured quad rendering (vertex positions and colour buffers)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);

	// unbind textured quad VAO
	glBindVertexArray(0);
}


objImporter::objImporter(const char* texturePath) {

	loadShader();
	setupVAO();

	// Load texture
	texture = fiLoadTexture(texturePath, TextureProperties(true));
}


objImporter::objImporter(GLuint initTexture) {

	loadShader();
	setupVAO();

	texture = initTexture;
}


objImporter::~objImporter() {

	// unbind textured quad VAO
	glBindVertexArray(0);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &objVertexBuffer);
	glDeleteBuffers(1, &objTextureCoordBuffer);

	glDeleteVertexArrays(1, &objVertexArray);

	glDeleteShader(objShader);
}


void objImporter::render(const GUMatrix4& T) {

	static GLint mvpLocation = glGetUniformLocation(objShader, "mvpMatrix");

	glUseProgram(objShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(T.M));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(objVertexArray);

	// draw quad directly - no indexing needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind VAO for textured quad
	glBindVertexArray(0);
}


