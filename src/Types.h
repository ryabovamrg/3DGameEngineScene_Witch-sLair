#pragma once

#include "pgr.h"
namespace ryabomar {
	/**
	The structure that stores bird mesh.
	Taken from PGR seminar 2.
	*/
	struct birdMeshData {
		int nTriangles; // we use only triangles, each triangle is 3xshort int
		const unsigned short* triangles;
		int nVertices; // we store each vertex as 3xfloat for each frame
		const float* vertices;
		glm::vec3 color;
		int nAnimFrames; // it will be always at least 1 (static model)
	};

	/**
	The structure that stores PhongMaterial values.
	*/
	struct PhongMaterial {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 specularExponent;
		glm::vec3 textureScale;
		float shininess;
	};


	/**
	Contains basic constant vectors directions used in calculations
	*/
	namespace DIRECTION {
		/*         +y
					| /
				____|/____+x
				   /|
				+z/ |           */

		const glm::vec3 UP{ 0.0f, 1.0f,  0.0f };
		const glm::vec3 RIGHT{ 1.0f, 0.0f,  0.0f };
		const glm::vec3 FORWARD{ 0.0f, 0.0f,  -1.0f };

		const glm::vec3 DOWN = -UP;
		const glm::vec3 LEFT = -RIGHT;
		const glm::vec3 BACK = -FORWARD;

		const glm::vec3 ONE{ 1.0f };
		const glm::vec3 ZERO{ 0.0f };
	}
	namespace DIR = DIRECTION;

}