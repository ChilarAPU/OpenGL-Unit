#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <CoreStructures/GUPivotCamera.h>
#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector3.h>
#include <vector>
#include <string>
#include "shader_setup.h"
using namespace std;
using namespace CoreStructures;

class aiWrapper : public CoreStructures::GUObject {

private:
	//TODO:
	//replace these variables with structs that hold all relevant variables together

	GLuint VAO;
	//Holds all indices of model
	vector<GLuint> Indices;

	const aiScene* model;

	//currently the players default position and rotation
	GUVector3 modelPos = GUVector3(-1.f, 1.f, 1.f);
	GUVector3 modelRot = GUVector3(0.0, 3.14, 0.0);

public:
	aiWrapper(const std::string& filename, unsigned int flags, GUPivotCamera* camera);
	~aiWrapper();
	const aiScene* aiImportModel(const std::string& filename, unsigned int flags);
	void aiReportScene(const aiScene* scene);
	void aiSetupVBO(const aiScene* scene, CoreStructures::GUPivotCamera* mainCamera);
	void aiRenderVBO(const CoreStructures::GUMatrix4& T);

	void setModelPosition(GUVector3 Pos);
	void setModelRotation(GUVector3 Rot);
	GUVector3 getModelPosition();
	GUVector3 getModelRotation();
};