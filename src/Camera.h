#pragma once
#include "pgr.h"
#include "Parameters.h"
#include "Types.h"
#include "glm/gtx/euler_angles.hpp"
#include "Node.h"

namespace ryabomar {
	class Node;

	/**
	Node that is using transormations to project scene content on screen
	*/
	class Camera : public Node
	{
	public:
		// can camera be moved or rotated
		bool isStatic;

		/**
		returns view matrix of this camera based on node's transformations
		*/
		glm::mat4 getView();

		/**
		returns projection matrix of this camera using perspective projection
		*/
		glm::mat4 getProjection(int width, int height);

		/**
		Constructor

		\params cameraPosition
		\params cameraRotation
		\params isCameraStatic		can camera be moved or rotated
		*/
		Camera(glm::vec3 cameraPosition, glm::vec3 cameraRotation, bool isCameraStatic);
	};

}