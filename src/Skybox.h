#pragma once
#include "pgr.h"
#include "Camera.h"
#include <glm\gtx\euler_angles.hpp>
#include "Node.h"
#include "Interfaces.h"
namespace ryabomar {
	/**
	Node representing skybox
	*/

	class Skybox : public Node, public IDrawable
	{
	public:
		// cube texture of skybox
		GLuint cubeTextre;
		//compiled shader program used for this node
		GLuint skyboxShaderProgram;
		// Vertex array object that stores mesh of skybox
		GLuint vao;
		Skybox();


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