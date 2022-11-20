#include "Moon.h"
namespace ryabomar {
	Moon::Moon(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture, PhongMaterial material,
		glm::vec3 position, glm::vec3 rotationAngles, glm::vec3 scalingFactors)
		:DrawableNode(shaderProgram, mesh, texture, material, position, rotationAngles, scalingFactors)
	{
		this->position = glm::vec3(0.0f, 40.0f, -60.0f);
	}

	void Moon::update(double deltaTime, double elapsedTime) {
		

		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 180 * 5 * (float)deltaTime, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		this->position = rot * glm::vec4(this->position, 1.0);

		this->rotation.y += glm::pi<float>() / 180 * 10 * (float)deltaTime;
		this->rotation.x -= glm::pi<float>() / 180 * 10 * (float)deltaTime;


		
	}
}