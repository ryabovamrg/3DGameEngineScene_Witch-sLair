#pragma once
#include "pgr.h"
#include "Node.h"

namespace ryabomar {
	/**
		The node that  represents lightsource
	*/
	class LightNode : public Node
	{
	public:
		// values for Phong Lighting Model
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		// values for poinlight
		float constant;
		float linear;
		float quadratic;

		//values for spotlight
		float spotExponent;
		float cutOff;

		//light type
		enum class Type { DirectionalLight, PointLight, SpotLight };
		Type type;

		/**
		returns the direction of lightNode in global space
		*/
		glm::vec3 getDirection();
	};

}