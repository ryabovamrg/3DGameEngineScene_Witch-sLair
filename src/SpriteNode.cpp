#include "SpriteNode.h"
namespace ryabomar {
	SpriteNode::SpriteNode(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture,
		int framesHorizontally, int framesVertically,
		float frameSpeed,	// time of display per frame
		glm::vec3 position,
		glm::vec3 rotationAngles,
		glm::vec3 scalingFactors)
		: texture(texture), shaderProgram(shaderProgram)
	{
		this->isActive = true;
		this->position = position;
		this->rotation = rotationAngles;
		this->scale = scalingFactors;
		this->framesHorizontally = framesHorizontally;
		this->framesVertically = framesVertically;
		this->frameSpeed = frameSpeed;
		nVertices = mesh.nVertices;
		nTriangles = mesh.nTriangles;


		GLuint VBO, EBO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// reserve 1 slot for VBO
		glGenBuffers(1, &VBO);
		//select VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//send data to VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.nAttribsPerVertex * mesh.nVertices, mesh.verticesInterleaved, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * mesh.nTriangles, mesh.triangles, GL_STATIC_DRAW);

		GLint positionLoc = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), 0);

		/*
		GLint normalLoc = glGetAttribLocation(shaderProgram, "normal");
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), (void*)(sizeof(float) * 3));

		*/

		GLint texcoordLoc = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(texcoordLoc);
		glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, GL_FALSE, (sizeof(float) * 8), (void*)(sizeof(float) * 6));

		//release VAO
		glBindVertexArray(0);
		//release texture
		glBindTexture(GL_TEXTURE_2D, 0);


	}

	void SpriteNode::draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height) {

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE);
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shaderProgram);

		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection(width, height)));

		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));

		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->getModelTransform()));

		GLuint normalModelLoc = glGetUniformLocation(shaderProgram, "normalModel");
		glUniformMatrix4fv(normalModelLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(this->getModelTransform())));

		/* Time*/
		GLuint deltaTimeLoc = glGetUniformLocation(shaderProgram, "deltaTime");
		glUniform1f(deltaTimeLoc, Parameters::getInstance().deltaTime);
		GLuint elapsedTimeLoc = glGetUniformLocation(shaderProgram, "elapsedTime");
		glUniform1f(elapsedTimeLoc, Parameters::getInstance().timeElapsed);

		/* Sprite Sheet variables*/
		GLuint frameSpeedLoc = glGetUniformLocation(shaderProgram, "frameSpeed");
		glUniform1f(frameSpeedLoc, this->frameSpeed);
		GLuint framesHorizontallyLoc = glGetUniformLocation(shaderProgram, "framesHorizontally");
		glUniform1i(framesHorizontallyLoc, this->framesHorizontally);
		GLuint framesVerticallyLoc = glGetUniformLocation(shaderProgram, "framesVertically");
		glUniform1i(framesVerticallyLoc, this->framesVertically);

		glDrawElements(GL_TRIANGLES, nTriangles * 3, GL_UNSIGNED_INT, 0);


		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}

	bool SpriteNode::shouldDraw() {
		return isActive;
	}
}