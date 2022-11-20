#include <iostream>
#include "pgr.h"
#include "Camera.h"
#include "Parameters.h"
#include "Types.h"
#include "sceneGraph/AxesNode.h"
#include "Skybox.h"
#include "Assets.h"
#include "ModelLoader.h"
#include "Node.h"
#include "DrawableNode.h"
#include "Scene.h"
#include "SpriteNode.h"
#include "Moon.h"
#include "Collider.h"
#include "Bird.h"

using namespace ryabomar;

/* ====== GLOBAL VARIABLES ==========*/
const char* WIN_TITLE = "PGR game engine";

GLuint shaderProgram;
GLuint spriteShaderProgram;
GLuint geoShaderProgram;
GLuint waterShaderProgram; 
GLuint birdShaderProgram;
GLuint moonShaderProgram;

Camera* mainCamera;
Camera* staticCamera1;
Camera* staticCamera2;
Camera* dynamicCamera1;
Camera* dynamicCamera2;
Camera* currentCamera;

Collider* playerCollider;
Node* player;

/// vector containing all light in scene
std::vector <LightNode*> lights;
/// vector containing all trees in scene
std::vector <Node*> trees;
/// vector containing all bushes in scene
std::vector <Node*> bushes;

///current mouse position used to move camera & for interaction
glm::ivec2 mousePos, lastMousePos;

/// layers of scene that are rendered in order from layer0 to layer2
/// LAYER0 --- background
/// LAYER1 --- regular nodes
/// LAYER2 --- nodes with transparency
enum layers { LAYER0, LAYER1, LAYER2, _layerCount };
Scene scene[_layerCount];

/// fire dynamic texture used with mouse click
GLuint fireSpriteSheet;

/// keys table allowes to handle multiple keys at once
enum KEYS { UP, LEFT, RIGHT, DOWN, INSIDE, OUTSIDE, 
	MOUSE_LEFT, MOUSE_RIGHT, F1, BUTTON_C,
	_count };
bool keyboard[_count];

/// Callbacks for OpenGL errors
void APIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* userParam)
{
	if(type == GL_DEBUG_TYPE_ERROR)
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	
}

/// Process inputs from keyboard and mouse
void processInput() {
	// if camera can be moved
	if (!currentCamera->isStatic) {
		// if the camera is mainCamera, we move our player
		if(currentCamera == mainCamera)
		{
			//PLAYER TRANSLATION
			glm::vec3 deltaPosition = glm::vec3(0.0f);
			glm::vec3 oldPosition = player->getGlobalPosition();
			if (keyboard[UP]) {
				deltaPosition += 0.1f * currentCamera->getGlobalUp();
			}
			if (keyboard[DOWN]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalUp();
			}
			if (keyboard[RIGHT]) {
				deltaPosition += 0.1f * currentCamera->getGlobalRight();
			}
			if (keyboard[LEFT]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalRight();
			}
			if (keyboard[INSIDE]) {
				deltaPosition += 0.1f * currentCamera->getGlobalForward();
			}
			if (keyboard[OUTSIDE]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalForward();
			}
			player->position += deltaPosition;
			for (Collider* collider : scene[LAYER1].getColliders()) {
				if (playerCollider != collider && playerCollider->isIntersecting(collider)) {
					player->position = oldPosition;
					std::cout << "COLLISION!" << std::endl;
					break;
				}
			}
		}//elsewise we move the camera
		else if (currentCamera == staticCamera1 || currentCamera == staticCamera2) {
			//CAMERA TRANSLATION
			glm::vec3 deltaPosition = glm::vec3(0.0f);
			glm::vec3 oldPosition = currentCamera->getGlobalPosition();
			if (keyboard[UP]) {
				deltaPosition += 0.1f * currentCamera->getGlobalUp();
			}
			if (keyboard[DOWN]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalUp();
			}
			if (keyboard[RIGHT]) {
				deltaPosition += 0.1f * currentCamera->getGlobalRight();
			}
			if (keyboard[LEFT]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalRight();
			}
			if (keyboard[INSIDE]) {
				deltaPosition += 0.1f * currentCamera->getGlobalForward();
			}
			if (keyboard[OUTSIDE]) {
				deltaPosition -= 0.1f * currentCamera->getGlobalForward();
			}
			currentCamera->position += deltaPosition;
			
		}
		//mouse 
		glm::ivec2 deltaMousePos = mousePos - lastMousePos;
		lastMousePos = mousePos;
		if (keyboard[MOUSE_LEFT]) {
			currentCamera->rotation.x += deltaMousePos.y * Parameters::getInstance().deltaTime * 0.025;
			currentCamera->rotation.y += deltaMousePos.x * Parameters::getInstance().deltaTime * 0.025;

			/* if <i,j> ý x, nothing changes, x returned
			*  if x <= i ret i
			*  if x >= j ret j
			*/
			currentCamera->rotation.x = std::clamp(currentCamera->rotation.x, glm::radians(-60.0f), glm::radians(60.0f));

		}
	}

	// F1 loads Config.txt file, if load fails, default values are used instead
	if (keyboard[F1]) {
		Parameters::getInstance().loadParameters();
		glutReshapeWindow(Parameters::getInstance().windowWidth, Parameters::getInstance().windowHeight);

		player->position = Parameters::getInstance().playerPos;
		player->rotation = Parameters::getInstance().playerRot;

		mainCamera->position = Parameters::getInstance().mainCameraPos;
		mainCamera->rotation = Parameters::getInstance().mainCameraRot;

		staticCamera1->position = Parameters::getInstance().staticCamera1Pos;
		staticCamera1->rotation = Parameters::getInstance().staticCamera1Rot;

		staticCamera2->position = Parameters::getInstance().staticCamera2Pos;
		staticCamera2->rotation = Parameters::getInstance().staticCamera2Rot;

		keyboard[F1] = false;
	}
	// C button makes a static camera non-static, alowing it to move and rotate
	if (keyboard[BUTTON_C]) {
		if (currentCamera == staticCamera1 || currentCamera == staticCamera2) {
			if (currentCamera->isStatic) {
				currentCamera->isStatic = false;
			}
			else {
				currentCamera->isStatic = true;
			}

		}
	}
}

/// Key processing callbacks
void specialKey(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		keyboard[INSIDE] = true;
	}
	if (key == GLUT_KEY_DOWN) {
		keyboard[OUTSIDE] = true;
	}
	if (key == GLUT_KEY_RIGHT) {
		keyboard[RIGHT] = true;
	}
	if (key == GLUT_KEY_LEFT) {
		keyboard[LEFT] = true;
	}
	if (key == GLUT_KEY_F1) {
		keyboard[F1] = true;
	}
}
void specialKeyReleased(int key, int x, int y) {

	if (key == GLUT_KEY_UP) {
		keyboard[INSIDE] = false;
	}
	if (key == GLUT_KEY_DOWN) {
		keyboard[OUTSIDE] = false;
	}
	if (key == GLUT_KEY_RIGHT) {
		keyboard[RIGHT] = false;
	}
	if (key == GLUT_KEY_LEFT) {
		keyboard[LEFT] = false;
	}	
	if (key == GLUT_KEY_F1) {
		keyboard[F1] = false;
	}
}
void key(unsigned char key, int x, int y)
{
	if (key == 27) {	//	quit requested
		glutLeaveMainLoop();
	}
	//TRANSLATION
	if (key == 'w' || key == 'W') {
		keyboard[INSIDE] = true;
	}if (key == 'a' || key == 'A') {
		keyboard[LEFT] = true;
	}if (key == 's' || key == 'S') {
		keyboard[OUTSIDE] = true;
	}
	if (key == 'd' || key == 'D') {
		keyboard[RIGHT] = true;
	}
	if (key == 'q' || key == 'Q') {
		keyboard[UP] = true;
	}
	if (key == 'e' || key == 'E') {
		keyboard[DOWN] = true;
	}

	if (key == 'c' || key == 'C') {
		keyboard[BUTTON_C] = true;
	}
	

}
void keyReleased(unsigned char key, int x, int y)
{
	if (key == 27) {	//	quit requested
		glutLeaveMainLoop();
	}
	//TRANSLATION
	if (key == 'w' || key == 'W') {
		keyboard[INSIDE] = false;
	}if (key == 'a' || key == 'A') {
		keyboard[LEFT] = false;
	}if (key == 's' || key == 'S') {
		keyboard[OUTSIDE] = false;
	}
	if (key == 'd' || key == 'D') {
		keyboard[RIGHT] = false;
	}
	if (key == 'q' || key == 'Q') {
		keyboard[UP] = false;
	}
	if (key == 'e' || key == 'E') {
		keyboard[DOWN] = false;
	}
	if (key == 'c' || key == 'C') {
		keyboard[BUTTON_C] = false;
	}

}

/// Process input when mouse button pressed
void mouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			keyboard[MOUSE_LEFT] = true;
			
		}
		else {
			keyboard[MOUSE_LEFT] = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			keyboard[MOUSE_RIGHT] = true;
			int nodeId = 1;
			glReadPixels(x, Parameters::getInstance().windowHeight - y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &nodeId);

			// if we clicked on the tree or on the bush, the pair of new spriteNodes with fire is generated.
			for (Node* node : scene[LAYER1].getNodes()) {
				for (Node* tree : trees) {
					if (node->id == nodeId && node == tree) {
				
						float offset = 5.0f;
						glm::vec3 spritePosition = glm::vec3(0.f, offset, 0.0f);

						Node* sprite1 = new SpriteNode(spriteShaderProgram, planeMeshData, fireSpriteSheet, 8, 8, 10, spritePosition, glm::vec3(0.0f), glm::vec3(4.5f));
						Node* sprite2 = new SpriteNode(spriteShaderProgram, planeMeshData, fireSpriteSheet, 8, 8, 10, spritePosition, glm::vec3(0.0f,  glm::pi<float>()/2, 0.0f), glm::vec3(4.5f));
						scene[LAYER2].add(sprite1);
						scene[LAYER2].add(sprite2);
						node->addChild(sprite1);
						node->addChild(sprite2);

						LightNode* fireLight = new LightNode();
						fireLight->type = LightNode::Type::PointLight;
						fireLight->ambient = glm::vec3(0.8f);
						fireLight->diffuse = glm::vec3(1.0f, 0.2f, 0.0f);
						fireLight->specular = glm::vec3(1.0f);
						fireLight->constant = 1.0f;
						fireLight->linear = 0.09f;
						fireLight->quadratic = 0.032f;
						fireLight->position = glm::vec3(0.0f);
						lights.push_back(fireLight);
						scene->add(fireLight);
						sprite1->addChild(fireLight);

					}
				}
				for (Node* bush : bushes) {

					if (node->id == nodeId && node == bush) {

						
						float offset = 2.0f;
						glm::vec3 spritePosition = glm::vec3(0.f, offset, 0.0f);

						Node* sprite1 = new SpriteNode(spriteShaderProgram, planeMeshData, fireSpriteSheet, 8, 8, 10, spritePosition, glm::vec3(0.0f), glm::vec3(2.5f));
						Node* sprite2 = new SpriteNode(spriteShaderProgram, planeMeshData, fireSpriteSheet, 8, 8, 10, spritePosition, glm::vec3(0.0f, glm::pi<float>() / 2, 0.0f), glm::vec3(2.5f));

						scene[LAYER2].add(sprite1);
						scene[LAYER2].add(sprite2);
						node->addChild(sprite1);
						node->addChild(sprite2);

						LightNode* fireLight = new LightNode();
						fireLight->type = LightNode::Type::PointLight;
						fireLight->ambient = glm::vec3(0.8f);
						fireLight->diffuse = glm::vec3(1.0f, 0.2f, 0.0f);
						fireLight->specular = glm::vec3(1.0f);
						fireLight->constant = 1.0f;
						fireLight->linear = 0.09f;
						fireLight->quadratic = 0.032f;
						fireLight->position = glm::vec3(0.0f);
						lights.push_back(fireLight);
						scene->add(fireLight);
						sprite1->addChild(fireLight);
					}
				}
			}
		}
		else {
			keyboard[MOUSE_RIGHT] = false;
		}
	}
}

void mouseMotion(int x, int y) {
	mousePos = glm::ivec2(x, y);
}


/// Changes the viewport of the window, so the window content is not stretched 
void reshapeFunc(int newWidth, int newHeight) {
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
	Parameters::getInstance().windowHeight = newHeight;
	Parameters::getInstance().windowWidth = newWidth;
}

// Draw fuction that is called every frame.
void draw() {

	
	//time management
	double tmpTime = ((double)glutGet(GLUT_ELAPSED_TIME)) / 1000;
	double& deltaTime = Parameters::getInstance().deltaTime;
	double& timeElapsed = Parameters::getInstance().timeElapsed;
	deltaTime = tmpTime - timeElapsed;
	timeElapsed = tmpTime;

	//clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //std::cout << "CAMERA pos: " << currentCamera->getGlobalPosition().x << " " << currentCamera->getGlobalPosition().y << " " << currentCamera->getGlobalPosition().z << std::endl;
   //std::cout << "CAMERA rot: " << currentCamera->rotation.x << " " << currentCamera->rotation.y << " " << currentCamera->rotation.z << std::endl;

	/* STENCIL MAGIC*/
	// use of stencil buffer to peek objects
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// draw all the drawable content of each scene layer
	for (IDrawable* node : scene[LAYER0].getDrawable()) {
		if (!node->shouldDraw()) {
			continue;
		}
		// IDrawable does not have an id
		Node* tmpNode = dynamic_cast<Node*> (node);

		if (tmpNode != nullptr) {
			glStencilFunc(GL_ALWAYS, tmpNode->id, GL_KEEP);
		}
		node->draw(lights, currentCamera, Parameters::getInstance().windowWidth, Parameters::getInstance().windowHeight);
	}

	for (IDrawable* node : scene[LAYER1].getDrawable()) {
		if (!node->shouldDraw()) {
			continue;
		}
		Node* tmpNode = dynamic_cast<Node*> (node);

		if (tmpNode != nullptr) {
			glStencilFunc(GL_ALWAYS, tmpNode->id, GL_KEEP);
		}

		node->draw(lights, currentCamera, Parameters::getInstance().windowWidth, Parameters::getInstance().windowHeight);
	}

	//disbling stencil to not rewrite objects we will peek
	glDisable(GL_STENCIL_TEST);

	//sorting the content on layer2 in order of distance
	// layer2 has objects with transparency
	std::vector <IDrawable*>& sortedLayer2 = scene[LAYER2].getDrawable();
	glm::vec3 cameraPosition = currentCamera->getGlobalPosition();
	// using comparator to sort them
	std::sort(sortedLayer2.begin(), sortedLayer2.end(), [&cameraPosition](IDrawable* left, IDrawable* right) -> bool 
	{
		glm::vec3 leftPosition = dynamic_cast <Node*> (left)->getGlobalPosition();
		glm::vec3 rightPosition = dynamic_cast <Node*> (right)->getGlobalPosition();
		return glm::distance(leftPosition, cameraPosition) > glm::distance(rightPosition, cameraPosition);
	});

	// draw the content of sorted vector using Blending to enable transparensy
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (IDrawable* node : sortedLayer2) {
		if (!node->shouldDraw()) {
			continue;
		}
		node->draw(lights, currentCamera, Parameters::getInstance().windowWidth, Parameters::getInstance().windowHeight);

	}
	// disable Blending
	glBlendFunc(GL_ONE, GL_ZERO);

	// Update all updatable content of the scenes
	for (IUpdatable* node : scene[LAYER0].getUpdatable()) {
		node->update(deltaTime, timeElapsed);
	}
	for (IUpdatable* node : scene[LAYER1].getUpdatable()) {
		node->update(deltaTime, timeElapsed);
	}
	for (IUpdatable* node : scene[LAYER2].getUpdatable()) {
		node->update(deltaTime, timeElapsed);
	}
	
	//pgr::sg::AxesNode an;
	//an.draw(currentCamera->getView(), currentCamera->getProjection(WIN_WIDTH, WIN_HEIGHT));

	glutSwapBuffers();
	glutPostRedisplay();
	processInput();

}

enum MenuParams { PLAYER_CAMERA, STATIC_CAMERA_1, STATIC_CAMERA_2, DYNAMIC_CAMERA1, DYNAMIC_CAMERA2};
void createGLUTMenus() {
	int menu;
	// create GLUT menu
	// use of lambda function to tell GLUT how to process menu
	menu = glutCreateMenu([] (int param) -> void {
		switch (param) {
		case MenuParams::PLAYER_CAMERA:
			currentCamera = mainCamera;
			break;

		case MenuParams::STATIC_CAMERA_1:
			currentCamera = staticCamera1;
			break;

		case MenuParams::STATIC_CAMERA_2:
			currentCamera = staticCamera2;
			break;

		case MenuParams::DYNAMIC_CAMERA1:
			currentCamera = dynamicCamera1;
			break;

		case MenuParams::DYNAMIC_CAMERA2:
			currentCamera = dynamicCamera2;
			break;
		}
	});
	// create menu entries
	glutAddMenuEntry("Player Camera",    MenuParams::PLAYER_CAMERA );
	glutAddMenuEntry("Static Camera 1",  MenuParams::STATIC_CAMERA_1);
	glutAddMenuEntry("Static Camera 2",  MenuParams::STATIC_CAMERA_2);
	glutAddMenuEntry("Dynamic Camera 1", MenuParams::DYNAMIC_CAMERA1);
	glutAddMenuEntry("Dynamic Camera 2", MenuParams::DYNAMIC_CAMERA2);
	// open menu on mouse middle button
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void init(int argc, char** argv) {
	std::cout << "Version 185a" << std::endl;
	/* Framework init */
	glutInit(&argc, argv);

	//load parameters from Config file
	Parameters::getInstance().loadParameters();

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(Parameters::getInstance().windowWidth, Parameters::getInstance().windowHeight);
	glutCreateWindow(WIN_TITLE);

	// callbacks
	glutDisplayFunc(draw);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(keyReleased);
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(specialKeyReleased);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mouseMotion);
	glutReshapeFunc(reshapeFunc);

	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	createGLUTMenus();

	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	glDebugMessageCallback(MessageCallback, nullptr);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

}

/// This function creates all cameras and sets their positions and rotations.
/// It also sets current camera to mainCamera.
void initCameras() {

	mainCamera = new Camera(Parameters::getInstance().mainCameraPos, Parameters::getInstance().mainCameraRot, false);
	staticCamera1 = new Camera(Parameters::getInstance().staticCamera1Pos, Parameters::getInstance().staticCamera1Rot, true);
	staticCamera2 = new Camera(Parameters::getInstance().staticCamera2Pos, Parameters::getInstance().staticCamera2Rot, true);
	dynamicCamera1 = new Camera(Parameters::getInstance().mainCameraPos, Parameters::getInstance().mainCameraRot, true);
	dynamicCamera2 = new Camera(Parameters::getInstance().mainCameraPos, Parameters::getInstance().mainCameraRot, true);
	scene->add(mainCamera);
	scene->add(staticCamera1);
	scene->add(staticCamera2);
	scene->add(dynamicCamera1);
	scene->add(dynamicCamera2);
	currentCamera = mainCamera;
}

/// This function imports textures, meshes and materials and uses them to create Nodes.
/// All Nodes are then added to the scene at scpecific layer, some Nodes may have children attached to them.
/// This function also sets up the lights and saves them to the lights vector.
void initScene() {

	/* ==================== TEXTURES ===================================================================== */
	GLuint whiteTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\white.jpg", false);
	GLuint yellowTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\yellow.jpg", false);
	GLuint redTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\red.jpg", false);
	GLuint eyeTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\eye.jpg", false);

	GLuint moon1Texture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\io.png", false);
	GLuint moon2Texture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\planet.jpg", false);

	GLuint testTex1 = pgr::createTexture(".\\assets\\models\\test\\img_test.png", false);
	GLuint testTex2 = pgr::createTexture(".\\assets\\models\\test\\test.png", false);
	/* =================================================================================================== */



	/* ==================== LIGHTS ===================================================================== */
	/* SUN */
	GLuint lightYellowTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\bottles1.png", true);
	Node* sunSphere = new DrawableNode(shaderProgram, pgr::icosphere3Data, redTexture, lightMaterial, glm::vec3(15.0f));
	sunSphere->isActive = false;
	scene[LAYER1].add(sunSphere);
	LightNode* sun = new LightNode();
	sun->type = LightNode::Type::DirectionalLight;
	sun->ambient = glm::vec3(0.15f);
	sun->diffuse = glm::vec3(0.6f);
	sun->specular = glm::vec3(0.4f);
	sun->position = glm::vec3(0.0f);
	sun->rotation = glm::vec3(0.0f, 0.0f, 1.0f);
	lights.push_back(sun);
	sun->setParent(sunSphere);
	sunSphere->rotation = glm::vec3(0.0f, glm::pi<float>() / 2, glm::pi<float>() / 2);
	/* SPOTLIGHT */
	LightNode* spotLight = new LightNode();
	spotLight->type = LightNode::Type::SpotLight;
	spotLight->ambient = glm::vec3(0.8f);
	spotLight->diffuse = glm::vec3(0.8f, 0.5f, 0.2f);
	spotLight->specular = glm::vec3(0.5f);
	spotLight->position = glm::vec3(0.0f, -2.0f, 0.0f);
	spotLight->rotation = glm::vec3(0.0f, glm::pi<float>() / 2, 0.0f);
	spotLight->cutOff = cos(glm::radians(12.5f * 5));
	spotLight->spotExponent = 10.0f;
	lights.push_back(spotLight);

	Node* spotSphere = new DrawableNode(shaderProgram, pgr::icosphere3Data, eyeTexture, lightMaterial);
	scene[LAYER1].add(spotSphere);
	spotSphere->scale *= 0.6;
	spotSphere->position = glm::vec3(-0.3f, -2.4f, 0.0f);
	spotSphere->rotation = glm::vec3(0.0f, 0.0f, glm::pi<float>() / 2);


	/* POINT LIGHTS */
	LightNode* pointLight1 = new LightNode();
	pointLight1->type = LightNode::Type::PointLight;
	pointLight1->ambient = glm::vec3(0.4f);
	pointLight1->diffuse = glm::vec3(0.7f, 0.55f, 0.0f);
	pointLight1->specular = glm::vec3(0.7f);
	pointLight1->constant = 1.0f;
	pointLight1->linear = 0.09f;
	pointLight1->quadratic = 0.032f;
	pointLight1->position = glm::vec3(0.0f);
	lights.push_back(pointLight1);

	LightNode* pointLight2 = new LightNode();
	pointLight2->type = LightNode::Type::PointLight;
	pointLight2->ambient = glm::vec3(0.4f);
	pointLight2->diffuse = glm::vec3(0.7f, 0.55f, 0.0f);
	pointLight2->specular = glm::vec3(0.7f);
	pointLight2->constant = 1.0f;
	pointLight2->linear = 0.09f;
	pointLight2->quadratic = 0.032f;
	pointLight2->position = glm::vec3(0.0f);
	lights.push_back(pointLight2);

	LightNode* pointLight3 = new LightNode();
	pointLight3->type = LightNode::Type::PointLight;
	pointLight3->ambient = glm::vec3(0.4f);
	pointLight3->diffuse = glm::vec3(0.7f, 0.55f, 0.0f);
	pointLight3->specular = glm::vec3(0.7f);
	pointLight3->constant = 1.0f;
	pointLight3->linear = 0.09f;
	pointLight3->quadratic = 0.032f;
	pointLight3->position = glm::vec3(0.0f);
	lights.push_back(pointLight3);

	//near the door
	Node* pointSphere1 = new DrawableNode(shaderProgram, pgr::icosphere3Data, lightYellowTexture, lightMaterial, glm::vec3(-33.9769f, 5.74321f, 2.71518f));
	//on top of the bed
	Node* pointSphere2 = new DrawableNode(shaderProgram, pgr::icosphere3Data, lightYellowTexture, lightMaterial, glm::vec3(-45.5943f, 5.68341f, 3.40726f));
	//on the table
	Node* pointSphere3 = new DrawableNode(shaderProgram, pgr::icosphere3Data, lightYellowTexture, lightMaterial, glm::vec3(-35.5209f, 3.11647f, -4.37523f));

	scene[LAYER1].add(pointSphere1);
	scene[LAYER1].add(pointSphere2);
	scene[LAYER1].add(pointSphere3);
	pointSphere1->scale *= 0.3;
	pointSphere2->scale *= 0.3;
	pointSphere3->scale *= 0.3;
	pointLight1->setParent(pointSphere1);
	pointLight2->setParent(pointSphere2);
	pointLight3->setParent(pointSphere2);
	/* =================================================================================================== */




	/*  ==================== TERRAIN ========================================================= */
	const char* terrainOBJpath = ".\\assets\\models\\terrain\\terrain.obj";
	const char* terrainMTLpath = ".\\assets\\models\\terrain\\terrain.mtl";
	PhongMaterial terrainMaterial = ModelLoader::loadMaterial(terrainMTLpath);
	GLuint terrainTexture = pgr::createTexture(".\\assets\\models\\terrain\\terrainTexture.jpg", true);

	Node* terrain = new DrawableNode(shaderProgram, ModelLoader::loadMesh(terrainOBJpath), terrainTexture, terrainMaterial, glm::vec3(0.0f, 0.0f, 0.0f));
	terrain->scale *= 8;
	scene[LAYER1].add(terrain);
	/* =================================================================================================== */



	/*  ==================== PLAYER ========================================================= */
	player = new DrawableNode(shaderProgram, pgr::icosphere3Data, testTex1, goldMaterial, Parameters::getInstance().playerPos, Parameters::getInstance().playerRot);
	player->isActive = false;
	playerCollider = new SphereCollider(1.0f);
	scene[LAYER1].add(player);
	scene[LAYER1].add(playerCollider);
	playerCollider->setParent(player);
	currentCamera->setParent(player);
	player->addChild(currentCamera);
	/* =================================================================================================== */



	/*  ==================== COLLIDERS ========================================================= */
	Node* box1 = new DrawableNode(shaderProgram, pgr::cubeData, testTex1, goldMaterial, glm::vec3(-60.0f, 34.0f, 0.0f), glm::vec3(0.0f), glm::vec3(3.0f, 50.0f, 60.0f));
	Node* colliderBox1 = new BoxCollider(glm::vec3(-3.0f, -50.0f, -60.0f), glm::vec3(3.0f, 50.0f, 60.0f));
	scene[LAYER2].add(box1);
	scene[LAYER1].add(colliderBox1);
	colliderBox1->setParent(box1);
	box1->isActive = false;

	Node* box2 = new DrawableNode(shaderProgram, pgr::cubeData, testTex1, goldMaterial, glm::vec3(60.0f, 34.0f, 0.0f), glm::vec3(0.0f), glm::vec3(3.0f, 50.0f, 60.0f));
	Node* colliderBox2 = new BoxCollider(glm::vec3(-3.0f, -50.0f, -60.0f), glm::vec3(3.0f, 50.0f, 60.0f));
	scene[LAYER2].add(box2);
	scene[LAYER1].add(colliderBox2);
	colliderBox2->setParent(box2);
	box2->isActive = false;

	Node* box3 = new DrawableNode(shaderProgram, pgr::cubeData, testTex1, goldMaterial, glm::vec3(0.0f, 34.0f, 60.0f), glm::vec3(0.0f), glm::vec3(60.0f, 50.0f, 3.0f));
	Node* colliderBox3 = new BoxCollider(glm::vec3(-60.0f, -50.0f, -3.0f), glm::vec3(60.0f, 50.0f, 3.0f));
	scene[LAYER2].add(box3);
	scene[LAYER1].add(colliderBox3);
	colliderBox3->setParent(box3);
	box3->isActive = false;

	Node* box4 = new DrawableNode(shaderProgram, pgr::cubeData, testTex1, goldMaterial, glm::vec3(0.0f, 34.0f, -60.0f), glm::vec3(0.0f), glm::vec3(60.0f, 50.0f, 3.0f));
	Node* colliderBox4 = new BoxCollider(glm::vec3(-60.0f, -50.0f, -3.0f), glm::vec3(60.0f, 50.0f, 3.0f));
	scene[LAYER2].add(box4);
	scene[LAYER1].add(colliderBox4);
	colliderBox4->setParent(box4);
	box4->isActive = false;


	/* =================================================================================================== */



	/*  ==================== TREES ========================================================= */
	const char* treesMTLpath = ".\\assets\\models\\trees\\trees.mtl";
	const PhongMaterial treeMaterial = ModelLoader::loadMaterial(treesMTLpath);

	const char* tree1OBJpath = ".\\assets\\models\\trees\\tree1\\tree1.obj";
	const char* tree2OBJpath = ".\\assets\\models\\trees\\tree2\\tree2.obj";
	const char* tree3OBJpath = ".\\assets\\models\\trees\\tree3\\tree3.obj";
	const char* tree4OBJpath = ".\\assets\\models\\trees\\tree4\\tree4.obj";
	const char* tree5OBJpath = ".\\assets\\models\\trees\\tree5\\tree5.obj";

	const pgr::MeshData tree1MeshData = ModelLoader::loadMesh(tree1OBJpath);
	const pgr::MeshData tree2MeshData = ModelLoader::loadMesh(tree2OBJpath);
	const pgr::MeshData tree3MeshData = ModelLoader::loadMesh(tree3OBJpath);
	const pgr::MeshData tree4MeshData = ModelLoader::loadMesh(tree4OBJpath);
	const pgr::MeshData tree5MeshData = ModelLoader::loadMesh(tree5OBJpath);

	GLuint tree1Texture = pgr::createTexture(".\\assets\\models\\trees\\tree1\\tree1.png", true);
	GLuint tree2Texture = pgr::createTexture(".\\assets\\models\\trees\\tree2\\tree2.png", true);
	GLuint tree3Texture = pgr::createTexture(".\\assets\\models\\trees\\tree3\\tree3.png", true);
	GLuint tree4Texture = pgr::createTexture(".\\assets\\models\\trees\\tree4\\tree4.png", true);
	GLuint tree5Texture = pgr::createTexture(".\\assets\\models\\trees\\tree5\\tree5.png", true);


	Node* tree1_1 = new DrawableNode(shaderProgram, tree1MeshData, tree1Texture, treeMaterial, glm::vec3(4.5f, -4.0f, -9.5f), glm::vec3(0.0f), glm::vec3(1.5f));
	scene[LAYER1].add(tree1_1);
	trees.push_back(tree1_1);
	Node* tree1_2 = new DrawableNode(shaderProgram, tree1MeshData, tree1Texture, treeMaterial, glm::vec3(-20.5f, -1.0f, 15.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree1_2);
	trees.push_back(tree1_2);
	Node* tree1_3 = new DrawableNode(shaderProgram, tree1MeshData, tree1Texture, treeMaterial, glm::vec3(-34.5f, 1.0f, -18.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree1_3);
	trees.push_back(tree1_3);


	Node* tree2_1 = new DrawableNode(shaderProgram, tree2MeshData, tree2Texture, treeMaterial, glm::vec3(-3.0f, -4.0f, 25.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree2_1);
	trees.push_back(tree2_1);
	Node* tree2_2 = new DrawableNode(shaderProgram, tree2MeshData, tree2Texture, treeMaterial, glm::vec3(-27.0f, -1.0f, -9.0f), glm::vec3(0.0f), glm::vec3(1.5));
	scene[LAYER1].add(tree2_2);
	trees.push_back(tree2_2);
	Node* tree2_3 = new DrawableNode(shaderProgram, tree2MeshData, tree2Texture, treeMaterial, glm::vec3(7.0f, -4.0f, -35.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree2_3);
	trees.push_back(tree2_3);

	Node* tree3_1 = new DrawableNode(shaderProgram, tree3MeshData, tree3Texture, treeMaterial, glm::vec3(8.0f, -1.0f, -18.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	scene[LAYER1].add(tree3_1);
	trees.push_back(tree3_1);
	Node* tree3_2 = new DrawableNode(shaderProgram, tree3MeshData, tree3Texture, treeMaterial, glm::vec3(-39.0f, -1.0f, 20.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree3_2);
	trees.push_back(tree3_2);
	Node* tree3_3 = new DrawableNode(shaderProgram, tree3MeshData, tree3Texture, treeMaterial, glm::vec3(-19.0f, 1.0f, -40.0f), glm::vec3(0.0f), glm::vec3(1.7f));
	scene[LAYER1].add(tree3_3);
	trees.push_back(tree3_3);

	Node* tree4_1 = new DrawableNode(shaderProgram, tree4MeshData, tree4Texture, treeMaterial, glm::vec3(-8.5f, -3.0f, -6.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree4_1);
	trees.push_back(tree4_1);
	Node* tree4_2 = new DrawableNode(shaderProgram, tree4MeshData, tree4Texture, treeMaterial, glm::vec3(8.0f, -4.0f, 35.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree4_2);
	trees.push_back(tree4_2);
	Node* tree4_3 = new DrawableNode(shaderProgram, tree4MeshData, tree4Texture, treeMaterial, glm::vec3(-29.0f, 1.0f, -32.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	scene[LAYER1].add(tree4_3);
	trees.push_back(tree4_3);
	Node* tree4_4 = new DrawableNode(shaderProgram, tree4MeshData, tree4Texture, treeMaterial, glm::vec3(-5.0f, -1.0f, -20.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	scene[LAYER1].add(tree4_4);
	trees.push_back(tree4_4);

	Node* tree5_1 = new DrawableNode(shaderProgram, tree5MeshData, tree5Texture, treeMaterial, glm::vec3(-1.5f, -4.0f, 10.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	scene[LAYER1].add(tree5_1);
	trees.push_back(tree5_1);
	Node* tree5_2 = new DrawableNode(shaderProgram, tree5MeshData, tree5Texture, treeMaterial, glm::vec3(-15.0f, -2.0f, -12.0f), glm::vec3(0.0f), glm::vec3(1.3f));
	scene[LAYER1].add(tree5_2);
	trees.push_back(tree5_2);
	Node* tree5_3 = new DrawableNode(shaderProgram, tree5MeshData, tree5Texture, treeMaterial, glm::vec3(25.0f, -4.0f, -22.0f), glm::vec3(0.0f), glm::vec3(1.8f));
	scene[LAYER1].add(tree5_3);
	trees.push_back(tree5_3);
	/* =================================================================================================== */



	/*  ==================== BUSHES ========================================================= */

	const char* bushesMTLpath = ".\\assets\\models\\trees\\trees.mtl";
	const PhongMaterial bushMaterial = ModelLoader::loadMaterial(bushesMTLpath);

	const char* bush1OBJpath = ".\\assets\\models\\trees\\bush1\\bush1.obj";
	const char* bush2OBJpath = ".\\assets\\models\\trees\\bush2\\bush2.obj";
	const char* bush3OBJpath = ".\\assets\\models\\trees\\bush3\\bush3.obj";
	const char* bush4OBJpath = ".\\assets\\models\\trees\\bush4\\bush4.obj";
	const char* bush5OBJpath = ".\\assets\\models\\trees\\bush5\\bush5.obj";
	const char* bush6OBJpath = ".\\assets\\models\\trees\\bush6\\bush6.obj";

	GLuint bush1Texture = pgr::createTexture(".\\assets\\models\\trees\\bush1\\bush1.png", true);
	GLuint bush2Texture = pgr::createTexture(".\\assets\\models\\trees\\bush2\\bush2.png", true);
	GLuint bush3Texture = pgr::createTexture(".\\assets\\models\\trees\\bush3\\bush3.png", true);
	GLuint bush4Texture = pgr::createTexture(".\\assets\\models\\trees\\bush4\\bush4.png", true);
	GLuint bush5Texture = pgr::createTexture(".\\assets\\models\\trees\\bush5\\bush5.png", true);
	GLuint bush6Texture = pgr::createTexture(".\\assets\\models\\trees\\bush6\\bush6.png", true);


	const pgr::MeshData bush1MeshData = ModelLoader::loadMesh(bush1OBJpath);
	const pgr::MeshData bush2MeshData = ModelLoader::loadMesh(bush2OBJpath);
	const pgr::MeshData bush3MeshData = ModelLoader::loadMesh(bush3OBJpath);
	const pgr::MeshData bush4MeshData = ModelLoader::loadMesh(bush4OBJpath);
	const pgr::MeshData bush5MeshData = ModelLoader::loadMesh(bush5OBJpath);
	const pgr::MeshData bush6MeshData = ModelLoader::loadMesh(bush6OBJpath);

	Node* bush1 = new DrawableNode(shaderProgram, bush1MeshData, bush1Texture, bushMaterial, glm::vec3(5.0f, -4.0f, 0.0f));
	scene[LAYER1].add(bush1);
	bushes.push_back(bush1);
	Node* bush2 = new DrawableNode(shaderProgram, bush2MeshData, bush2Texture, bushMaterial, glm::vec3(0.0f, -2.0f, -6.0f));
	scene[LAYER1].add(bush2);
	bushes.push_back(bush2);
	Node* bush3 = new DrawableNode(shaderProgram, bush3MeshData, bush3Texture, bushMaterial, glm::vec3(-10.0f, -6.0f, 10.0f));
	scene[LAYER1].add(bush3);
	bushes.push_back(bush3);

	Node* bush4 = new DrawableNode(shaderProgram, bush4MeshData, bush4Texture, bushMaterial, glm::vec3(-20.0f, 1.8f, 20.0f));
	scene[LAYER1].add(bush4);
	bushes.push_back(bush4);

	Node* bush5 = new DrawableNode(shaderProgram, bush5MeshData, bush5Texture, bushMaterial, glm::vec3(-10.0f, -0.8f, 10.0f));
	scene[LAYER1].add(bush5);
	bushes.push_back(bush5);
	Node* bush6 = new DrawableNode(shaderProgram, bush6MeshData, bush6Texture, bushMaterial, glm::vec3(-10.0f, 2.0f, -40.0f));
	scene[LAYER1].add(bush6);
	bushes.push_back(bush6);
	/* =================================================================================================== */



	/*  ==================== SPRITE ========================================================= */
	GLuint digitsSpriteSheet = pgr::createTexture(".\\assets\\dynamicTextures\\flame_2_digits.jpg", false);
	GLuint digits = pgr::createTexture(".\\assets\\dynamicTextures\\digits.png", false);
	fireSpriteSheet = pgr::createTexture(".\\assets\\dynamicTextures\\fire_2.png", false);
	GLuint stars = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\sky3.png", false);
	/* =================================================================================================== */



	/*  ==================== SKYBOX ========================================================= */
	Node* skybox = new Skybox();
	scene[LAYER0].add(skybox);
	/* =================================================================================================== */



	/*  ==================== WATER ========================================================= */
	GLuint waterTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\water.png", true);
	GLuint water2Texture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\water2.png", true);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, water2Texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTexture);

	Node* waterPlane = new DrawableNode(waterShaderProgram, planeMeshData, waterTexture, waterMaterial, glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(glm::pi<float>() / 2, 0.0f, 0.0f), glm::vec3(50.0f));
	scene[LAYER1].add(waterPlane);
	/* =================================================================================================== */



	/* ==================== HOUSE ============================================================================================= */

	Node* house = new Node();
	scene[LAYER1].add(house);

	//---- HOUSE BASE ---- //

	const char* roofOBJpath = ".\\assets\\models\\house\\roof.obj";
	const char* roofMTLpath = ".\\assets\\models\\house\\roof.mtl";
	GLuint roofTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\roof.png", true);
	Node* roof = new DrawableNode(shaderProgram, ModelLoader::loadMesh(roofOBJpath), roofTexture, ModelLoader::loadMaterial(roofMTLpath), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(roof);
	roof->setParent(house);

	const char* houseBaseOBJpath = ".\\assets\\models\\house\\houseBase.obj";
	const char* houseBaseMTLpath = ".\\assets\\models\\house\\houseBase.mtl";
	GLuint houseBaseTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\houseBase.jpg", true);
	Node* houseBase = new DrawableNode(shaderProgram, ModelLoader::loadMesh(houseBaseOBJpath), houseBaseTexture, ModelLoader::loadMaterial(".\\assets\\models\\house\\furniture.mtl"), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(houseBase);
	houseBase->setParent(house);

	const char* lianaOBJpath = ".\\assets\\models\\house\\liana.obj";
	const char* lianaMTLpath = ".\\assets\\models\\house\\liana.mtl";
	GLuint lianaTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\liana.jpg", true);
	Node* liana = new DrawableNode(shaderProgram, ModelLoader::loadMesh(lianaOBJpath), lianaTexture, ModelLoader::loadMaterial(lianaMTLpath), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(liana);
	liana->setParent(house);

	const char* windowsOBJpath = ".\\assets\\models\\house\\windows.obj";
	const char* windowsMTLpath = ".\\assets\\models\\house\\windows.mtl";
	GLuint windowsTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\windows.jpg", true);
	Node* windows = new DrawableNode(shaderProgram, ModelLoader::loadMesh(windowsOBJpath), windowsTexture, ModelLoader::loadMaterial(windowsMTLpath), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(windows);
	windows->setParent(house);


	const char* glassWindowOBJpath = ".\\assets\\models\\house\\glassWindow.obj";
	const char* glassWindowMTLpath = ".\\assets\\models\\house\\glassWindow.mtl";
	GLuint glassWindowTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\glass.png", true);
	Node* glassWindow = new DrawableNode(shaderProgram, ModelLoader::loadMesh(glassWindowOBJpath), glassWindowTexture, ModelLoader::loadMaterial(".\\assets\\models\\house\\furniture.mtl"), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER2].add(glassWindow);
	glassWindow->setParent(house);

	//---- FURNITURE ---- //
	const char* furnitureOBJpath = ".\\assets\\models\\house\\furniture.obj";
	const char* furnitureMTLpath = ".\\assets\\models\\house\\furniture.mtl";
	GLuint furnitureTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\furniture.jpg", true);
	const PhongMaterial funitureMaterial = ModelLoader::loadMaterial(furnitureMTLpath);
	Node* furniture = new DrawableNode(shaderProgram, ModelLoader::loadMesh(furnitureOBJpath), furnitureTexture, funitureMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(furniture);
	furniture->setParent(house);

	const char* metallicFurnitureOBJpath = ".\\assets\\models\\house\\metallicFurniture.obj";
	const char* metallicFurnitureMTLpath = ".\\assets\\models\\house\\metallicFurniture.mtl";
	GLuint metallicFurnitureTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\metal.png", true);
	Node* metallicFurniture = new DrawableNode(shaderProgram, ModelLoader::loadMesh(metallicFurnitureOBJpath), metallicFurnitureTexture, lightMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(metallicFurniture);
	metallicFurniture->setParent(house);

	const char* bedClothOBJpath = ".\\assets\\models\\house\\bedCloth.obj";
	const char* bedClothMTLpath = ".\\assets\\models\\house\\bedCloth.mtl";
	GLuint bedClothTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\cloth.png", true);
	Node* bedCloth = new DrawableNode(shaderProgram, ModelLoader::loadMesh(bedClothOBJpath), bedClothTexture, ModelLoader::loadMaterial(bedClothMTLpath), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(bedCloth);
	bedCloth->setParent(house);

	const char* rugOBJpath = ".\\assets\\models\\house\\rug.obj";
	const char* rugMTLpath = ".\\assets\\models\\house\\rug.mtl";
	GLuint rugTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\rug.jpg", true);
	Node* rug = new DrawableNode(shaderProgram, ModelLoader::loadMesh(rugOBJpath), rugTexture, ModelLoader::loadMaterial(rugMTLpath), glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(rug);
	rug->setParent(house);

	//--- BOOKS --- //
	const char* booksMTLpath = ".\\assets\\models\\house\\books.mtl";
	const PhongMaterial booksMaterial = ModelLoader::loadMaterial(booksMTLpath);
	const char* books1OBJpath = ".\\assets\\models\\house\\books1.obj";
	GLuint books1Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\books1.png", true);
	Node* books1 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(books1OBJpath), books1Texture, booksMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(books1);
	books1->setParent(house);

	const char* books2OBJpath = ".\\assets\\models\\house\\books2.obj";
	GLuint books2Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\books2.png", true);
	Node* books2 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(books2OBJpath), books2Texture, booksMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(books2);
	books2->setParent(house);

	const char* books3OBJpath = ".\\assets\\models\\house\\books3.obj";
	GLuint books3Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\books3.png", true);
	Node* books3 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(books3OBJpath), books3Texture, booksMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(books3);
	books3->setParent(house);

	const char* books4OBJpath = ".\\assets\\models\\house\\books4.obj";
	GLuint books4Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\books4.png", true);
	Node* books4 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(books4OBJpath), books4Texture, booksMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(books4);
	books4->setParent(house);


	//--- BOTTLES --- //

	const char* bottlesMTLpath = ".\\assets\\models\\house\\bottles.mtl";
	const PhongMaterial bottlesMaterial = ModelLoader::loadMaterial(bottlesMTLpath);
	const char* bottles1OBJpath = ".\\assets\\models\\house\\bottles1.obj";
	GLuint bottles1Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\bottles1.png", true);
	Node* bottles1 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(bottles1OBJpath), bottles1Texture, bottlesMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(bottles1);
	bottles1->setParent(house);

	const char* bottles2OBJpath = ".\\assets\\models\\house\\bottles2.obj";
	GLuint bottles2Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\bottles2.png", true);
	Node* bottles2 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(bottles2OBJpath), bottles2Texture, bottlesMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(bottles2);
	bottles2->setParent(house);

	const char* bottles3OBJpath = ".\\assets\\models\\house\\bottles3.obj";
	GLuint bottles3Texture = pgr::createTexture(".\\assets\\models\\house\\textures\\bottles3.png", true);
	Node* bottles3 = new DrawableNode(shaderProgram, ModelLoader::loadMesh(bottles3OBJpath), bottles3Texture, bottlesMaterial, glm::vec3(0.0f, -1.0f, 0.0f));
	scene[LAYER1].add(bottles3);
	bottles3->setParent(house);


	house->scale *= 0.5;
	house->position = glm::vec3(-37.0f, 1.3f, -6.5f);
	house->rotation = glm::vec3(0.0f, 1.3f, 0.0f);

	/* ========================================================================================================================== */



	/* ==================== POTION ============================================================================================= */
	GLuint potionTexture = pgr::createTexture(".\\assets\\models\\blendFiles\\textures\\potion.jpg", false);
	Node* potionPlane = new DrawableNode(geoShaderProgram, gridPlaneMeshData, potionTexture, lightMaterial, glm::vec3(-42.9948f, 2.86705f, -5.49364f), glm::vec3(0.0f), glm::vec3(0.5f, 0.5f, 1.0f));
	scene[LAYER1].add(potionPlane);
	potionPlane->scale *= 0.75;
	/* ========================================================================================================================== */



	/* ==================== MOON ============================================================================================= */
	Node* moonFlying = new Moon(moonShaderProgram, pgr::icosphere3Data, moon1Texture, lightMaterial, glm::vec3(1.0f, 0.0f, 0.0f));
	scene[LAYER1].add(moonFlying);
	moonFlying->scale *= 6;
	/* ========================================================================================================================== */



	/* ==================== LANTERN ============================================================================ */
	const char* lanternOBJpath = ".\\assets\\models\\lantern\\lantern.obj";
	const char* lanternMTLpath = ".\\assets\\models\\lantern\\lantern.mtl";
	GLuint lanternTexture = pgr::createTexture(".\\assets\\models\\house\\textures\\metal.png", true);
	Node* lantern = new DrawableNode(shaderProgram, ModelLoader::loadMesh(lanternOBJpath), lanternTexture, lightMaterial,
		glm::vec3(0.0f, 0.0f, 0.25f), glm::vec3(0.0f, glm::pi<float>() * 3 / 2, glm::pi<float>() / 6), glm::vec3(0.2));
	scene[LAYER1].add(lantern);

	lantern->addChild(spotSphere);
	lantern->addChild(spotLight);
	/* ========================================================================================================================== */



	/* ==================== BIRDS ====================================================================== */
	Node* bird = new Bird(birdShaderProgram, birds_data, 4.0, 10, 8, stars, lantern, spotSphere, glm::vec3(-5.0f, 15.0f, -10.0f));
	scene[LAYER1].add(bird);
	bird->scale *= 2;
	bird->addChild(lantern);
	bird->addChild(dynamicCamera1);
	dynamicCamera1->position = glm::vec3(0.0f, -1.0f, -5.0f);
	dynamicCamera1->rotation = glm::vec3(0.0f, glm::pi <float>(), 0.0f);


	Node* bird2 = new Bird(birdShaderProgram, birds_data, 4.0, -22.5, 0, stars, nullptr, nullptr, glm::vec3(0.0f, 12.0f, -10));
	scene[LAYER1].add(bird2);
	bird2->scale *= 2;
	bird2->addChild(dynamicCamera2);
	dynamicCamera2->position = glm::vec3(0.0f, 1.5f, 2.5f);
	dynamicCamera2->rotation = glm::vec3(-glm::pi <float>() / 12, 0.0f, 0.0f);

	Node* bird3 = new Bird(birdShaderProgram, birds_data, 5.0, 20.5, 3, stars, nullptr, nullptr, glm::vec3(5.0f, 10.0f, -10));
	scene[LAYER1].add(bird3);

	Node* bird4 = new Bird(birdShaderProgram, birds_data, 2.0, -5.5, 7, stars, nullptr, nullptr, glm::vec3(0.0f, 6.0f, -10));
	scene[LAYER1].add(bird4);

	Node* bird5 = new Bird(birdShaderProgram, birds_data, 5.5, 22.5, 5, stars, nullptr, nullptr, glm::vec3(5.0f, 11.0f, -10));
	scene[LAYER1].add(bird5);
	/* ========================================================================================================================== */
}


int main(int argc, char** argv)
{
	init(argc, argv);
	GLuint shaders[] = {
		pgr::createShaderFromFile(GL_VERTEX_SHADER,   ".\\assets\\shaders\\vertShader.vert"),
		pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\fragmShader.frag"),		
		0 };

	shaderProgram = pgr::createProgram(shaders);

	GLuint moonShaders[] = {
	pgr::createShaderFromFile(GL_VERTEX_SHADER,   ".\\assets\\shaders\\moon.vert"),
	pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\moon.frag"),
	0 };

	moonShaderProgram = pgr::createProgram(shaders);

	GLuint SpriteShaders[] = {
		pgr::createShaderFromFile(GL_VERTEX_SHADER, ".\\assets\\shaders\\sprite.vert"),
		pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\sprite.frag"), 0
	};
	spriteShaderProgram = pgr::createProgram(SpriteShaders);

	GLuint geometryUpdateShaders[] = {
		pgr::createShaderFromFile(GL_VERTEX_SHADER,   ".\\assets\\shaders\\geometryUpdate.vert"),
		pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\geometryUpdate.frag"),
		0 };

	geoShaderProgram = pgr::createProgram(geometryUpdateShaders);

	GLuint waterShaders[] = {
		pgr::createShaderFromFile(GL_VERTEX_SHADER,   ".\\assets\\shaders\\water.vert"),
		pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\water.frag"),
		0 };

	waterShaderProgram = pgr::createProgram(waterShaders);
	
	GLuint birdShaders[] = {
	pgr::createShaderFromFile(GL_VERTEX_SHADER,   ".\\assets\\shaders\\bird.vert"),
	pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\bird.frag"),
	0 };

	birdShaderProgram = pgr::createProgram(birdShaders);


	initCameras();
	initScene();

	Parameters::getInstance().timeElapsed = ((double)glutGet(GLUT_ELAPSED_TIME)) / 1000;
	Parameters::getInstance().deltaTime = 0;

	glutMainLoop();
	return 0;
}
