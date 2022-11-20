#pragma once
#include "Node.h"
#include "Types.h"
#include "Camera.h"
#include "pgr.h"
#include "Interfaces.h"

namespace ryabomar {

	/**
		The node that can be drawn
	*/
	class DrawableNode : public Node, public IDrawable
	{
	private:
		// material for use in lighting in shader
		PhongMaterial material;
		//texture of this node's mesh
		GLuint texture;
		// Vertex array object that stores mesh
		GLuint VAO;
		//number of verticies and triangles in the mesh
		unsigned nVertices, nTriangles;
		//compiled shader program used for this node
		GLuint shaderProgram;


	public:
		/**
		Constructor

		\params shaderProgram		compiled shader program used for this node
		\params mesh				mesh for this node
		\params texture				texture of this node's mesh
		\params material			material for use in lighting in shader
		\params position			node's position
		\params rotationAngles		node's rotation
		\params scalingFactors		node's scale

		*/
		DrawableNode(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture, PhongMaterial material,
			glm::vec3 position = glm::vec3(0.0),
			glm::vec3 rotationAngles = glm::vec3(0.0),
			glm::vec3 scalingFactors = glm::vec3(1.0));

		/**
		Draw function that is called every frame

		\params lights			vector of all lightsources in the scene
		\params camera			current camera
		\params width			window width
		\params height			window height

		*/
		virtual void draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height) override;

		/** should this node be drawn or not */
		virtual bool shouldDraw();
	};

}