#include "LightNode.h"
namespace ryabomar {
	glm::vec3 LightNode::getDirection() {
		return this->getForward();
	}
}