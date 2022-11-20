#include "Skybox.h"
namespace ryabomar {

	const GLfloat cubeVertices[] = {
		// position    
		1, -1, -1,
		1, -1,  1,
		-1, -1,  1,
		-1, -1, -1,
		1,  1, -1,
		-1,  1, -1,
		-1,  1,  1,
		1,  1,  1,
		1, -1, -1,
		1,  1, -1,

		1,  1,  1,
		1, -1,  1,
		1, -1,  1,
		1,  1,  1,
		-1,  1,  1,
		-1, -1,  1,
		-1, -1,  1,
		-1,  1,  1,
		-1,  1, -1,
		-1, -1, -1,

		1,  1, -1,
		1, -1, -1,
		-1, -1, -1,
		-1,  1, -1
	};
	const GLuint cubeTriangles[] = {
		0,  1,  2,      0,  2,  3,      4,  5,  6,      4,  6,  7,      8,  9, 10,      8, 10, 11,
	   12, 13, 14,     12, 14, 15,     16, 17, 18,     16, 18, 19,     20, 21, 22,     20, 22, 23,
	};

	pgr::MeshData skyboxCube = { 8, 12, 3, cubeVertices, cubeTriangles };

	Skybox::Skybox()
	{
		this->isActive = true;
		glGenTextures(1, &cubeTextre);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextre);

		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\RIGHT.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\LEFT.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\TOP.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\BOTTOM.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\FRONT.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		pgr::loadTexImage2D(".\\assets\\skybox\\SKYBOX_NIGHT\\BACK.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		GLuint SkyboxShaders[] = {
			pgr::createShaderFromFile(GL_VERTEX_SHADER, ".\\assets\\shaders\\skybox.vert"),
			pgr::createShaderFromFile(GL_FRAGMENT_SHADER, ".\\assets\\shaders\\skybox.frag"), 0
		};
		skyboxShaderProgram = pgr::createProgram(SkyboxShaders);


		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * 3, skyboxCube.verticesInterleaved, GL_STATIC_DRAW);

		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 12 * 3, skyboxCube.triangles, GL_STATIC_DRAW);

		GLint positionLoc = glGetAttribLocation(skyboxShaderProgram, "position");
		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Skybox::draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height)
	{
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextre);
		glUseProgram(skyboxShaderProgram);

		glm::mat4 proj = camera->getProjection(width, height);
		GLuint projectionLoc = glGetUniformLocation(skyboxShaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));


		Node* tmpNode = camera->getParent();

		glm::mat4 rot = glm::yawPitchRoll(camera->rotation.y, camera->rotation.x, camera->rotation.z);
		while (tmpNode != nullptr) {
			glm::mat4 tmpRot = glm::yawPitchRoll(tmpNode->rotation.y, tmpNode->rotation.x, tmpNode->rotation.z);
			rot = tmpRot * rot;
			tmpNode = tmpNode->getParent();
		}
		glm::mat4 view = glm::inverse(rot);
		GLuint viewLoc = glGetUniformLocation(skyboxShaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glUseProgram(0);
	}
	bool Skybox::shouldDraw() {
		return isActive;
	}
}