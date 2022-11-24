// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <glew/glew.h>
#include <glew/wglew.h>
#include <GL\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include <iostream>
#include <string>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "src/aiWrapper.h"
#include "src/texture_loader.h"
#include "tetrahedron.h"
#include "Sound.h"

using namespace std;
using namespace CoreStructures;

#pragma region Scene variables and resources

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int								mouse_x, mouse_y;
bool							mDown = false;

GUClock* mainClock = nullptr;

//
// Main scene resources
//
tetrahedron* Tetrahedron = nullptr;
GUPivotCamera* mainCamera = nullptr;

GLuint knifeTexture;
GLuint beastTexture;
GLuint towerTexture;

//custom shader assignment
GLuint paShader;

unsigned int aiVAO;

aiWrapper* Knife = nullptr;
aiWrapper* Beast = nullptr;
aiWrapper* Tower = nullptr;
//array of knife models that are fly towards the player
aiWrapper* flyingKnives[5] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};


bool wKeyDown;
bool sKeyDown;
bool aKeyDown;
bool dKeyDown;

//sound
Sound* music;

#pragma endregion


#pragma region Function Prototypes

void init(int argc, char* argv[]); // Main scene initialisation function
void update(void); // Main scene update function
void display(void); // Main scene render function

// Event handling functions
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyDown(unsigned char key, int x, int y);
void closeWindow(void);
void reportContextVersion(void);
void reportExtensions(void);
void playerMovement();
//i refers to amount of times the fuction has been run through
void spawnKnives(int i);
void moveKnives();

void enableTextureGeneration(const char *filePath);

#pragma endregion


int main(int argc, char* argv[])
{
	init(argc, argv);
	glutMainLoop();

	// Stop clock and report final timing data
	if (mainClock) {

		mainClock->stop();
		mainClock->reportTimingData();
		mainClock->release();
	}

	return 0;
}


void init(int argc, char* argv[]) {

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE/* | GLUT_MULTISAMPLE*/);
	glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window
	int windowWidth = 800;
	int windowHeight = 600;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("3D Example 01");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Register callback functions
	glutIdleFunc(update); // Main scene update function
	glutDisplayFunc(display); // Main render function
	glutKeyboardFunc(keyDown); // Key down handler
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutMouseWheelFunc(mouseWheel); // Mouse wheel event handler
	glutCloseFunc(closeWindow); // Main resource cleanup handler


	// Initialise glew
	glewInit();

	// Initialise OpenGL...

	wglSwapIntervalEXT(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW); // Default anyway

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Setup main camera
	float viewportAspect = (float)windowWidth / (float)windowHeight;
	mainCamera = new GUPivotCamera(0.0f, 0.0f, 15.0f, 55.0f, viewportAspect, 0.1f);
	cout << mainCamera->cameraLocation() << endl;

	//setup custom shaders
	paShader = setupShaders(string("shaders\\basic_texture.vs"), string("shaders\\basic_texture.fs"));

	//initialise new tetrahedron
	Tetrahedron = new tetrahedron();

	// Setup and start the main clock
	mainClock = new GUClock();

	Knife = new aiWrapper(string("Knife.obj"),
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType, mainCamera);

	knifeTexture = fiLoadTexture("..\\Common2\\Resources\\Textures\\knife_texture.png", TextureProperties(false));

	Beast = new aiWrapper(string("Kuratchi_l_no_rig_ver.1.0.fbx"),
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType, mainCamera);
	beastTexture = fiLoadTexture("..\\Common2\\Resources\\Textures\\Kuratch_ver.1.0.png", TextureProperties(false));

	Tower = new aiWrapper(string("Broadway_Tower.fbx"),
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType, mainCamera);
	towerTexture = fiLoadTexture("..\\Common2\\Resources\\Textures\\Broadway_Tower_u1_v1.png", TextureProperties(false));

	music = new Sound(
		"..\\Common2\\Resources\\Sounds\\lo-fi rain.wav",\
		mainCamera
	);

	//gnerate knife models inside the array
	for (int i = 0; i < 5; i++) {
		flyingKnives[i] = new aiWrapper(string("Knife.obj"),
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType, mainCamera);
	}
	int counter = 0;
	//set flyingKnife starting position 
	glutTimerFunc(1000, spawnKnives, counter);

}

// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	// Update clock
	mainClock->tick();

	// Redraw the screen
	display();

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "RT-Graphics 3D Demo. Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);

	playerMovement();
	moveKnives();
}


void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Get view-projection transform as a GUMatrix4
	GUMatrix4 T = mainCamera->projectionTransform() * mainCamera->viewTransform() * GUMatrix4::translationMatrix(-Beast->getModelPosition().x, -Beast->getModelPosition().y, -Beast->getModelPosition().z);
	GLuint shaderAlphaLoc = glGetUniformLocation(paShader, "alpha");
	float shaderAlpha = 1;
	/*if (Tetrahedron) {
		Tetrahedron->render(T);
	}*/
	glUseProgram(paShader);
	glUniform1f(shaderAlphaLoc, shaderAlpha);

	//getting locations of uniform shader variables
	static GLint mvpLocation = glGetUniformLocation(paShader, "mvpMatrix");
	static GLint modelLocation = glGetUniformLocation(paShader, "model");

	GUMatrix4 modelMatrix = GUMatrix4::translationMatrix(0, 0, 0) * GUMatrix4::rotationMatrix(0, 0, 0);
	GUMatrix4 mvpMatrix = T * modelMatrix;
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(mvpMatrix.M));
	//inserting model matrix inside shader for normals
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const GLfloat*)&(modelMatrix.M));
	glBindTexture(GL_TEXTURE_2D, knifeTexture);
	Knife->aiRenderVBO(T);

	//Spawn in all knives
	for (int i = 0; i < 5; i++) {
		modelMatrix = GUMatrix4::translationMatrix(flyingKnives[i]->getModelPosition().x, flyingKnives[i]->getModelPosition().y, flyingKnives[i]->getModelPosition().z) * GUMatrix4::rotationMatrix(0, 1.57, 0);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const GLfloat*)&(modelMatrix.M));
		modelMatrix = GUMatrix4::translationMatrix(flyingKnives[i]->getModelPosition().x, flyingKnives[i]->getModelPosition().y, flyingKnives[i]->getModelPosition().z) * GUMatrix4::rotationMatrix(0, 1.57, 0) * GUMatrix4::scaleMatrix(.1, .1, .1);
		mvpMatrix = T * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(mvpMatrix.M));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const GLfloat*)&(modelMatrix.M));
		glBindTexture(GL_TEXTURE_2D, knifeTexture);
		flyingKnives[i]->aiRenderVBO(T);
	}


	modelMatrix = GUMatrix4::translationMatrix(0, -5, -5) * GUMatrix4::rotationMatrix(-1.57, 0, 0);
	mvpMatrix = T * modelMatrix;
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(mvpMatrix.M));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const GLfloat*)&(modelMatrix.M));
	glBindTexture(GL_TEXTURE_2D, towerTexture);
	Tower->aiRenderVBO(T);

	modelMatrix = GUMatrix4::translationMatrix(Beast->getModelPosition().x, Beast->getModelPosition().y, Beast->getModelPosition().z) * GUMatrix4::rotationMatrix(Beast->getModelRotation().x, Beast->getModelRotation().y, Beast->getModelRotation().z);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const GLfloat*)&(modelMatrix.M));
	//re-declaring the model matrix as I cant scale the model before shading with normals.
	modelMatrix = GUMatrix4::translationMatrix(Beast->getModelPosition().x, Beast->getModelPosition().y, Beast->getModelPosition().z) * GUMatrix4::rotationMatrix(Beast->getModelRotation().x, Beast->getModelRotation().y, Beast->getModelRotation().z) * GUMatrix4::scaleMatrix(.1, .1, .1);
	mvpMatrix = T * modelMatrix;
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(mvpMatrix.M));
	glBindTexture(GL_TEXTURE_2D, beastTexture);

	//calls required for transparent object
	//always call tranparent objects last in the render queue
	shaderAlpha = 0.5;
	glUniform1f(shaderAlphaLoc, shaderAlpha);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);
	Beast->aiRenderVBO(T);
	//calls required for transparent object
	glDisable(GL_BLEND);
	//glEnable(GL_CULL_FACE);

	glutSwapBuffers();
}



#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	int dx = x - mouse_x;
	int dy = y - mouse_y;

	if (mainCamera) {
		mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);

		//set the position of the sound to the camera location
		auto cameraLocation = mainCamera->cameraLocation();
		alListenerfv(AL_POSITION, (ALfloat*)(&cameraLocation));

		cameraLocation.normalise();
		ALfloat orientation[] = { -cameraLocation.x, -cameraLocation.y, -cameraLocation.z, 0.0, 0.0, 0.0 };
		alListenerfv(AL_ORIENTATION, orientation);
	}

	mouse_x = x;
	mouse_y = y;
}


void mouseWheel(int wheel, int direction, int x, int y) {

	if (mainCamera) {

		if (direction < 0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (direction > 0)
			mainCamera->scaleCameraRadius(0.9f);
	}
	//Quieter when scrolling away, louder when scrolling in
	// set the position of the sound to the camera location
	auto cameraLocation = mainCamera->cameraLocation();
	alListenerfv(AL_POSITION, (ALfloat*)(&cameraLocation));

	cameraLocation.normalise();
	ALfloat orientation[] = { -cameraLocation.x, -cameraLocation.y, -cameraLocation.z, 0.0, 0.0, 0.0 };
	alListenerfv(AL_ORIENTATION, orientation);
}


void keyDown(unsigned char key, int x, int y) {
	// Toggle fullscreen (This does not adjust the display mode however!)
	if (key == 'f')
		glutFullScreenToggle();
	if (key == 'w')
		wKeyDown = true;
	if (key == 's')
		sKeyDown = true;
}


void closeWindow(void) {

	// Clean-up scene resources

	if (mainCamera)
		mainCamera->release();

	//if (aiBeast)
		//exampleModel->release();
}


#pragma region Helper Functions

void reportContextVersion(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

void reportExtensions(void) {

	cout << "Extensions supported...\n\n";

	const char* glExtensionString = (const char*)glGetString(GL_EXTENSIONS);

	char* strEnd = (char*)glExtensionString + strlen(glExtensionString);
	char* sptr = (char*)glExtensionString;

	while (sptr < strEnd) {

		int slen = (int)strcspn(sptr, " ");
		printf("%.*s\n", slen, sptr);
		sptr += slen + 1;
	}
}

void playerMovement()
{
	GUVector3 PlayerPos = Beast->getModelPosition();
	GUVector3 PlayerRot = Beast->getModelRotation();
	if (GetAsyncKeyState('W')) {
		PlayerPos.z += -0.2;
		PlayerRot.y = 3.14;
		//wKeyDown = false;
	}
	if (GetAsyncKeyState('S')) {
		PlayerPos.z += 0.2;
		PlayerRot.y = 0;
		//sKeyDown = false;
	}
	if (GetAsyncKeyState('A')) {
		//playerRot.y += 0.2;
		PlayerPos.x -= 0.2;
		PlayerRot.y = -1.57;
		aKeyDown = false;
	}
	if (GetAsyncKeyState('D')) {
		PlayerPos.x += 0.2;
		PlayerRot.y = 1.57;
		dKeyDown = false;
	}
	Beast->setModelPosition(PlayerPos);
	Beast->setModelRotation(PlayerRot);
}

void enableTextureGeneration(const char *filePath)
{
	//load texture
	GLuint texture = fiLoadTexture(filePath, TextureProperties(false));
	//set texture generation in all axes
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glBindTexture(GL_TEXTURE_3D, texture);
}

void spawnKnives(int i) 
{
	for (int j = 0; j < 5; j++) {
		flyingKnives[j]->setModelPosition(GUVector3((rand() % 4) - 2, (rand() % 4) - 2, (rand() % 4) - 2));
	}
	cout << i << endl;
	i++;
	return glutTimerFunc(1000, spawnKnives, i);
}

void moveKnives()
{
	for (int j = 0; j < 5; j++) {
		GUVector3 KnifePos = flyingKnives[j]->getModelPosition();
		KnifePos.z += 0.3;
		flyingKnives[j]->setModelPosition(KnifePos);
	}
}

#pragma endregion


