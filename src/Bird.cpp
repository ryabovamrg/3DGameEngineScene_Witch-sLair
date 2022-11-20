#include "Bird.h"

namespace ryabomar {

	Bird::Bird(GLuint shaderProgram, birdMeshData mesh, float movementSpeed, float rotationSpeed, int startFrame, GLuint texture,
		Node* lantern, Node* eye,
		glm::vec3 position, glm::vec3 rotationAngles, glm::vec3 scalingFactors)
	{
		this->isActive = true;
		this->position = position;
		this->rotation = rotationAngles;
		this->scale = scalingFactors;
		this->movementSpeed = movementSpeed;
		this->rotationSpeed = rotationSpeed;
		this->startFrame = startFrame;
		this->texture = texture;

		this->lantern = lantern;
		this->eye = eye;
		if (lantern != nullptr) {
			this->lanternStartRot = lantern->rotation;
		}
		if (eye != nullptr) {
			this->eyeStartRot = eye->rotation;
		}

		color = mesh.color;
		nVertices = mesh.nVertices;
		nTriangles = mesh.nTriangles;
		nAnimFrames = mesh.nAnimFrames;

		this->shaderProgram = shaderProgram;
		GLuint EBO;

		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);


		// reserve 1 slot for VBO
		glGenBuffers(1, &VBO);
		//select VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//send data to VBO
		glBufferData(GL_ARRAY_BUFFER, mesh.nAnimFrames * sizeof(float) * 3 * mesh.nVertices, mesh.vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 3 * mesh.nTriangles, mesh.triangles, GL_STATIC_DRAW);

		this->positionLoc = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(this->positionLoc);
		glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nextFrame * nVertices * 3 * sizeof(float)));

		this->nextPositionLoc = glGetAttribLocation(shaderProgram, "nextPosition");
		glEnableVertexAttribArray(this->nextPositionLoc);
		glVertexAttribPointer(this->nextPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nextFrame * nVertices * 3 * sizeof(float)));



		//release VAO
		glBindVertexArray(0);
		//release texture
		glBindTexture(GL_TEXTURE_2D, 0);


	}

	void Bird::draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height) {

		//std::cout << "Bird is attempting to draw" << std::endl;
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shaderProgram);


		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection(width, height)));

		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));

		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->getModelTransform()));


		/* Camera*/
		GLuint camPosLoc = glGetUniformLocation(shaderProgram, "camera.position");
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camera->getGlobalPosition()));

		GLuint camDirLoc = glGetUniformLocation(shaderProgram, "camera.direction");
		glUniform3fv(camDirLoc, 1, glm::value_ptr(camera->getGlobalForward()));


		GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
		glUniform3fv(colorLoc, 1, glm::value_ptr(this->color));

		GLuint tLoc = glGetUniformLocation(shaderProgram, "t");
		glUniform1f(tLoc, this->t);



		/* Time*/
		GLuint deltaTimeLoc = glGetUniformLocation(shaderProgram, "deltaTime");
		glUniform1f(deltaTimeLoc, Parameters::getInstance().deltaTime);
		GLuint elapsedTimeLoc = glGetUniformLocation(shaderProgram, "elapsedTime");
		glUniform1f(elapsedTimeLoc, Parameters::getInstance().timeElapsed);

		glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(currFrame * nVertices * 3 * sizeof(float)));
		glVertexAttribPointer(this->nextPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nextFrame * nVertices * 3 * sizeof(float)));

		glDrawElements(GL_TRIANGLES, nTriangles * 3, GL_UNSIGNED_SHORT, 0);



		//glBlendFunc(GL_ONE, GL_ZERO);
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}


	void Bird::update(double deltaTime, double elapsedTime) {

		int speed = 150;
		int elapsedTimeMs = glutGet(GLUT_ELAPSED_TIME);
		int e = elapsedTimeMs % 150;            // relative time in the frame [0..150ms]  <- animFrameTimeMs
		this->currFrame = this->startFrame + elapsedTimeMs / 150;      // current frame index [0..10] ...        <- birds_data.nAnimFrames
		this->nextFrame = this->currFrame + 1;           // current frame + 1 modulo 11 [0..10]
		this->currFrame %= this->nAnimFrames;
		this->nextFrame %= this->nAnimFrames;
		this->t = float(e) / float(speed); // relative time in the frame [0..1]      <- e % 150ms


		this->position += (this->getForward() * movementSpeed * (float)deltaTime);
		this->rotation.y += glm::pi <float>() / 180 * rotationSpeed * deltaTime;

		if (lantern != nullptr && eye != nullptr) {


			lantern->rotation.z = this->lanternStartRot.z + sin(elapsedTime) / 2;
			eye->rotation.y = this->eyeStartRot.y + sin(elapsedTime * 5) / 5;
			eye->rotation.z = this->eyeStartRot.z + cos(elapsedTime * 2) / 2;
		}
	}

	bool Bird::shouldDraw() {
		return isActive;
	}
}