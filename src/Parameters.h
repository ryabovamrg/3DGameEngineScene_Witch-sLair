#pragma once
#include "pgr.h"
#include "Types.h"
#include <iostream>
#include <fstream>
#include <sstream>
namespace ryabomar {
	/**
	Class representing the parameters of the applications
	*/
	class Parameters
	{
	public:
		// window size
		int windowWidth, windowHeight;

		//time variables
		double timeElapsed;
		double deltaTime;

		// player positions and rotations
		glm::vec3 playerPos, playerRot;

		// camera positions and rotations
		glm::vec3 mainCameraPos, mainCameraRot;
		glm::vec3 staticCamera1Pos, staticCamera1Rot;
		glm::vec3 staticCamera2Pos, staticCamera2Rot;

		/**
		sets variables of the class to deafult values
		*/
		void setDefault();

		/**
		Assignes values to the variables in this class based on Config.txt file
		*/
		void loadParameters();
		Parameters();

		/**
		singleton implementation
		*/
		static Parameters& getInstance()
		{
			static Parameters instance;
			return instance;
		}
		Parameters(const Parameters& other) {}
		Parameters& operator=(const Parameters& other) {}
	};
}