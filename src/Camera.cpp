#pragma once
#include "Camera.h"
namespace ryabomar {
	glm::mat4 Camera::getView() {

		return glm::inverse(this->getModelTransform());

		glm::vec3 pos, target, up;
		pos = this->getGlobalPosition();
		up = this->getGlobalUp();
		target = pos + this->getGlobalForward();

		return glm::lookAt(pos, target, up);
	}

	glm::mat4 Camera::getProjection(int width, int height) {
		double ratio = (float)(width) / (float)(height);
		return glm::perspective(glm::radians(45.0), ratio, 0.1, 100.0);
	}

	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraRotation, bool isCameraStatic) {
		this->position = cameraPosition;
		this->rotation = cameraRotation;
		this->isStatic = isCameraStatic;
	}
}