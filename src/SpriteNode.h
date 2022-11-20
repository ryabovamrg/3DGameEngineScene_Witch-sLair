#pragma once
#include "pgr.h"
#include "Node.h"
#include "Interfaces.h"
namespace ryabomar {
	/**
	Node that represents dynamic texture on given mesh
	*/
	class SpriteNode : public Node, public IDrawable
	{
	private:
		// Vertex array object that stores mesh
		GLuint VAO;
		//number of verticies and triangles in the mesh
		unsigned nVertices, nTriangles;
		//compiled shader program used for this node
		GLuint shaderProgram;

		// dynamic texture, spritesheet
		GLuint texture;
		// number of frames dynamic texture has horizontally
		int framesHorizontally;
		// number of frames dynamic texture has vertically
		int framesVertically;
		//the speed of frames changing
		float frameSpeed;

	public:
		/**
		Constructor

		\params shaderProgram		compile shader program used for this node
		\params mesh				mesh for this node
		\params texture				dynamic texture, spritesheet
		\params framesHorizontally	number of frames dynamic texture has horizontally
		\params framesVertically	number of frames dynamic texture has vertically
		\params position			node's position
		\params rotationAngles		node's rotation
		\params scalingFactors		node's scale

		*/
		SpriteNode(GLuint shaderProgram, pgr::MeshData mesh, GLuint texture,
			int framesHorizontally, int framesVertically,
			float frameSpeed,	// time of display per frame
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