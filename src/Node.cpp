#include "Node.h"
namespace ryabomar {
	int Node::lastId = 0;

	Node::Node() {
		this->id = lastId++;
	}

	glm::vec3 Node::getGlobalPosition() {

		glm::vec4 res;

		if (parent != nullptr) {
			res = parent->getModelTransform() * glm::vec4(position, 1.0);
		}
		else {
			res = glm::vec4(position, 1.0f);
		}

		return res;
	}

	glm::mat4 Node::getModelTransform() {

		glm::mat4 res;
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 rot = glm::yawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
		glm::mat4 scaleXYZ = glm::scale(glm::mat4(1.0f), scale);
		res = translation * rot * scaleXYZ;


		if (parent != nullptr) {
			res = parent->getModelTransform() * res;
		}
		return res;
	}

	void Node::setParent(Node* parent) {
		this->parent = parent;
		if (parent == nullptr) return;
		parent->children.insert(this);
	}


	Node* Node::getParent() { return parent; }

	void Node::addChild(Node* child) {
		if (child == nullptr) return;
		children.insert(child);
		child->parent = this;
	}

	std::unordered_set<Node*>& Node::getChildren() {
		return children;
	}

	bool Node::isChild(Node* child) {
		return children.find(child) == children.end();
	}

	glm::vec3 Node::getForward() {

		glm::mat4 rot = glm::yawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
		return  glm::vec3(rot * glm::vec4(DIRECTION::FORWARD, 0.0));
	}

	glm::vec3 Node::getGlobalForward() {
		glm::vec3 res = this->getForward();
		Node* tmpParent = this->parent;
		while (tmpParent != nullptr)
		{
			glm::mat4 rot = glm::yawPitchRoll(tmpParent->rotation.y, tmpParent->rotation.x, tmpParent->rotation.z);

			res = glm::vec3(rot * glm::vec4(res, 0.0));
			tmpParent = tmpParent->getParent();
		}
		return glm::normalize(res);
	}

	glm::vec3 Node::getUp() {
		glm::mat4 rot = glm::yawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
		return  glm::vec3(rot * glm::vec4(DIRECTION::UP, 0.0));
	}
	glm::vec3 Node::getGlobalUp() {
		glm::vec3 res = this->getUp();
		Node* tmpParent = this->parent;
		while (tmpParent != nullptr)
		{
			glm::mat4 rot = glm::yawPitchRoll(tmpParent->rotation.y, tmpParent->rotation.x, tmpParent->rotation.z);

			res = glm::vec3(rot * glm::vec4(res, 0.0));
			tmpParent = tmpParent->getParent();
		}
		return glm::normalize(res);
	}

	glm::vec3 Node::getRight() {
		glm::mat4 rot = glm::yawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
		return  glm::vec3(rot * glm::vec4(DIRECTION::RIGHT, 0.0));
	}


	glm::vec3 Node::getGlobalRight() {
		glm::vec3 res = this->getRight();
		Node* tmpParent = this->parent;
		while (tmpParent != nullptr)
		{
			glm::mat4 rot = glm::yawPitchRoll(tmpParent->rotation.y, tmpParent->rotation.x, tmpParent->rotation.z);

			res = glm::vec3(rot * glm::vec4(res, 0.0));
			tmpParent = tmpParent->getParent();
		}
		return glm::normalize(res);
	}
}