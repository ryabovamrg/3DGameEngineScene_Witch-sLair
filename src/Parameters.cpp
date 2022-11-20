#include "Parameters.h"
namespace ryabomar {

	void Parameters::setDefault() {
		timeElapsed = ((double)glutGet(GLUT_ELAPSED_TIME)) / 1000;
		windowWidth = 1020;
		windowHeight = 850;

		playerPos = glm::vec3(0.0f, 0.0f, 0.0f);
		playerRot = glm::vec3(0.0f, 0.0f, 0.0f);

		mainCameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		mainCameraRot = glm::vec3(0.0f, 0.0f, 0.0f);

		staticCamera1Pos = glm::vec3(-4.62181, -0.976276, 18.6999);
		staticCamera1Rot = glm::vec3(0.234, -0.513999, 0);

		staticCamera2Pos = glm::vec3(14.1206, -0.594031, 16.2792);
		staticCamera2Rot = glm::vec3(0.176, 0.766001, 0);
	}


	void Parameters::loadParameters() {

		std::string line;
		std::ifstream myfile(".\\assets\\Config.txt");
		// open file
		if (myfile.is_open())
		{
			// read file in lines
			for (std::string line; std::getline(myfile, line); )
			{
				// detecting the first word in the line and saving it to the type
				std::istringstream in(line);
				std::string type;
				in >> type;

				//checking the type
				if (type == "PlayerPosition")
				{
					float x, y, z;
					if (in >> x >> y >> z)
					{
						playerPos = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in PlayerPosition." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "PlayerRotation ")
				{
					float x, y, z;
					if (in >> x >> y >> z)
					{
						playerRot = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in PlayerRotation ." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "MainCameraPosition")
				{
					float x, y, z;
					if (in >> x >> y >> z)
					{
						mainCameraPos = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in MainCameraPosition." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "MainCameraRotation")
				{
					float x, y, z;
					if (in >> x >> y >> z) {
						mainCameraRot = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in MainCameraRotation." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "1stCameraPosition")
				{
					float x, y, z;
					if (in >> x >> y >> z) {
						staticCamera1Pos = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in 1stCameraPosition." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "1stCameraRotation")
				{
					float x, y, z;
					if (in >> x >> y >> z) {
						staticCamera1Rot = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in 1stCameraRotation." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "2ndCameraPosition")
				{
					float x, y, z;
					if (in >> x >> y >> z) {
						staticCamera2Pos = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in 2ndCameraPosition." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "2ndCameraRotation")
				{
					float x, y, z;
					if (in >> x >> y >> z) {
						staticCamera2Rot = glm::vec3(x, y, z);
					}
					else {
						std::cout << "ERROR: config file contains unusable data in 2ndCameraRotation." << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "windowWidth")
				{
					int x;
					if (in >> x) {
						windowWidth = x;
					}
					else {
						std::cout << "ERROR: config file contains unusable data in windowWidth" << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
				if (type == "windowHeight")
				{
					int y;
					if (in >> y) {
						windowHeight = y;
					}
					else {
						std::cout << "ERROR: config file contains unusable data in windowHeight" << std::endl << "Default values used instead." << std::endl;
						setDefault();
						return;
					}
				}
			}
			std::cout << "File loading: Loaded OK!" << std::endl;
			myfile.close();
		}
		else {
			std::cout << "ERROR: Config file not open! Default values used instead." << std::endl;
			setDefault();
			return;
		}
	}



	Parameters::Parameters() {

		timeElapsed = ((double)glutGet(GLUT_ELAPSED_TIME)) / 1000;

		windowWidth = 1020;
		windowHeight = 850;

		playerPos = glm::vec3(0.0f, 0.0f, 0.0f);
		playerRot = glm::vec3(0.0f, 0.0f, 0.0f);


		mainCameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		mainCameraRot = glm::vec3(0.0f, 0.0f, 0.0f);

		staticCamera1Pos = glm::vec3(-4.62181, -0.976276, 18.6999);
		staticCamera1Rot = glm::vec3(0.234, -0.513999, 0);

		staticCamera2Pos = glm::vec3(14.1206, -0.594031, 16.2792);
		staticCamera2Rot = glm::vec3(0.176, 0.766001, 0);
	}


}