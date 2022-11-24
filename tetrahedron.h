#pragma once
#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <glew\glew.h>


class tetrahedron : public CoreStructures::GUObject
{
private:
	GLuint TetrahedronObj; //VAO
	GLuint VertexBuffer;
	GLuint ColorBuffer;
	GLuint IndexBuffer;
	GLuint paShader;

public:
	tetrahedron();

	~tetrahedron();

	//T refers to the camera transform that gets updated in main file
	void render(const CoreStructures::GUMatrix4& T);
	
};

