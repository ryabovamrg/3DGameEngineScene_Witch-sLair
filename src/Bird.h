#pragma once

#include "pgr.h"
#include "Node.h"
#include "Interfaces.h"
#include "Types.h"

namespace ryabomar {

	/**
		Node perpesenting an animated moving bird
	*/
	class Bird : public Node, public IDrawable, public IUpdatable
	{
	private:
		// Vertex array object
		GLuint VAO;
		// Vertex buffer object that containg mesh, used here for animation
		GLuint VBO;
		//number of verticies and triangles in the mesh
		unsigned nVertices, nTriangles;
		//compile shader program used for this node
		GLuint shaderProgram;
		//texture that is projected on the mesh in the shader, since there is no information of object's normals and texcoords
		GLuint texture;

		float movementSpeed = 1.0f;
		float rotationSpeed = 1.0f;

		// parameter for animation, used in shader
		float t;
		//default color of the bird
		glm::vec3 color;
		//number of frames in animation 
		int nAnimFrames;

		//locations of attrib pointers
		GLuint positionLoc;
		GLuint nextPositionLoc;

		// child nodes, used for hierarchical animation
		Node* lantern;
		Node* eye;

		//childerns' strating rotations
		glm::vec3 lanternStartRot;
		glm::vec3 eyeStartRot;

	public:
		// frame in amination we start from
		int startFrame = 0;

		int currFrame = 0;
		int nextFrame = 1;

		Bird(GLuint shaderProgram, birdMeshData mesh,
			float movementSpeed, float rotationSpeed,
			int startFrame, GLuint texture,
			Node* lantern, Node* eye,
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
		virtual void draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height);

		/** should this node be drawn or not */
		virtual bool shouldDraw();

		/** update node every frame */
		virtual void update(double deltaTime, double elpsedTime);
	};

}