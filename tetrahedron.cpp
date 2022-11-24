#include "tetrahedron.h"
#include "src/shader_setup.h"

using namespace std;
using namespace CoreStructures;

//packed vertex array for tetrahedron
static float paVertexArray[] = {

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	-1.f, 0.0f, -1.0f, 1.0f,
	1.0f, 0.0f, -1.0f, 1.0f,
};

static float paColourArray[] = {

	1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
};

static unsigned int paIndexArray[] = {
	0, 1, 2, 3, 1, 0, 2 ,3 ,0, 3, 2, 1
};

tetrahedron::tetrahedron()
{
	//setup shader
	//paShader = setupShaders(string("shaders\\basic_shader.vs"), string("shaders\\basic_shader.fs"));

	//setup VAO
	glGenVertexArrays(1, &TetrahedronObj);
	glBindVertexArray(TetrahedronObj);

	//setup VBO for vertices
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paVertexArray), paVertexArray, GL_STATIC_DRAW);

	//setup vertex shader attribute bindings 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	//setup vbo for color
	glGenBuffers(1, &ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paColourArray), paColourArray, GL_STATIC_DRAW);

	//setup vertex shader attribute bindings 
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	//setup vbo for index
	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(paIndexArray), paIndexArray, GL_STATIC_DRAW);

	//enable vertex buffers
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//unbind VAO
	glBindVertexArray(0);

}

tetrahedron::~tetrahedron()
{
	//unbind VAO if still binded
	glBindVertexArray(0);

	//unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Delete buffers
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &ColorBuffer);
	glDeleteBuffers(1, &IndexBuffer);

	//Delete VAO
	glDeleteVertexArrays(1, &TetrahedronObj);

	//Delete shader
	glDeleteShader(paShader);

}

void tetrahedron::render(const CoreStructures::GUMatrix4& T)
{
	//get location matrix from shader
	static GLint mvpLocation = glGetUniformLocation(paShader, "mvpMatrix");

	//tell OpenGL to use shader
	glUseProgram(paShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(T.M));

	glBindVertexArray(TetrahedronObj);

	glDrawElements(GL_TRIANGLES, sizeof(paIndexArray) / sizeof(unsigned int), GL_UNSIGNED_INT, (const GLvoid*)0);
	//could do *(&paIndexArray + 1) - paIndexArray

	glBindVertexArray(0);
}
