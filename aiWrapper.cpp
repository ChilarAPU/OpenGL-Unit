
#include "aiWrapper.h"
#include <glew/glew.h>
#include <iostream>

using namespace std;
using namespace CoreStructures;

// Create an instance of the Importer class
static Assimp::Importer				importer;

aiWrapper::aiWrapper(const std::string& filename, unsigned int flags, GUPivotCamera* camera)
{
	model = aiImportModel(filename, flags);
	aiSetupVBO(model, camera);
	//aiReportScene(model);
}

aiWrapper::~aiWrapper()
{

}

const aiScene* aiWrapper::aiImportModel(const std::string& filename, unsigned int flags)
{
	const aiScene* scene = importer.ReadFile(filename, flags);

	if (!scene)
		cout << "Sorry, " << filename << " not found!" << endl;
	else
		cout << filename << " loaded okay!" << endl;

	return scene;
}

void aiWrapper::aiReportScene(const aiScene* scene)
{
	auto numMeshes = scene->mNumMeshes;
	auto M = scene->mMeshes;

	for (unsigned int i = 0; i < numMeshes; ++i) {

		cout << "Mesh " << i << ":" << endl;

		auto mptr = M[i];

		cout << "num vertices = " << M[i]->mNumVertices << endl;
		cout << "num faces = " << M[i]->mNumFaces << endl;
	}
}

void aiWrapper::aiSetupVBO(const aiScene* scene, CoreStructures::GUPivotCamera* mainCamera)
{

	auto numMeshes = scene->mNumMeshes; //Holds amount of meshes in the model
	auto Meshes = scene->mMeshes; //arrays of meshes in the model
	
	//declare VBO buffers
	GLuint VertexBuffer;
	GLuint TexCoordBuffer;
	GLuint NormalsBuffer;
	GLuint IndexBuffer;

	vector<aiVector3D> V; //Holds all vertices
	vector<aiVector3D> TC; //Holds all TextureCoordinates
	vector<aiVector3D> N; //Holds all Normals (if normals are included in model file)

	//Insert data from model into variables above
	for (unsigned int i = 0; i < numMeshes; ++i) {
		aiVector3D* texCoordArray = Meshes[i]->mTextureCoords[0];
		for (unsigned int j = 0; j < Meshes[i]->mNumVertices; ++j) {
			V.push_back(Meshes[i]->mVertices[j]);
			TC.push_back(texCoordArray[j]);
			N.push_back(Meshes[i]->mNormals[j]);
		}
		for (unsigned int k = 0; k < Meshes[i]->mNumFaces; ++k) {

			Indices.push_back(Meshes[i]->mFaces[k].mIndices[0]);
			Indices.push_back(Meshes[i]->mFaces[k].mIndices[1]);
			Indices.push_back(Meshes[i]->mFaces[k].mIndices[2]);
		}
	}

	//setup VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//setup position VBO
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(aiVector3D), V.data(), GL_STATIC_DRAW);
	//https://stackoverflow.com/questions/14234361/opengl-using-vbo-with-stdvector to get the vector to work with buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	//setup TextureCoord VBO
	glGenBuffers(1, &TexCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, TC.size() * sizeof(aiVector3D), TC.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	//need to go into vs shader file and change/add texture coord packet. Currently does not have one

	//setup normals VBO
	glGenBuffers(1, &NormalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, N.size() * sizeof(aiVector3D), N.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	//setup index VBO
	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(6);

	glBindVertexArray(0);
}

void aiWrapper::aiRenderVBO(const CoreStructures::GUMatrix4& T)
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, (const GLvoid*)0);
}

void aiWrapper::setModelPosition(GUVector3 Pos)
{
	modelPos = Pos;
}

void aiWrapper::setModelRotation(GUVector3 Rot)
{
	modelRot = Rot;
}

GUVector3 aiWrapper::getModelPosition()
{
	return modelPos;
}

GUVector3 aiWrapper::getModelRotation()
{
	return modelRot;
}
