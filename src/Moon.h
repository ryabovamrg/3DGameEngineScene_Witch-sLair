#pragma once
#include "Interfaces.h"
#include "DrawableNode.h"
#include "pgr.h"
#include "iostream"
namespace ryabomar {

	/**
		Node perpesenting an animated moving moon
	*/

	class Moon : public IUpdatable, public DrawableNode
	{
	public:
		/**
		Constructor

		\params shaderProgram		compile shader program used for this node
		\params mesh				mesh for this node
		\params texture				texture of this node's mesh
		\params material			material for use in lighting in shader
		\params position			node's position
		\params rotationAngles		node's rotation
		\params scalingFactors		node's scale

		*/
		Moon(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture, PhongMaterial material,
			glm::vec3 position = glm::vec3(0.0),
			glm::vec3 rotationAngles = glm::vec3(0.0),
			glm::vec3 scalingFactors = glm::vec3(1.0));


		/** update node every frame */
		virtual void update(double deltaTime, double elapsedTime) override;
	};

}