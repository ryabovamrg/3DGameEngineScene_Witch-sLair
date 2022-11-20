#pragma once
#include <memory>
#include <vector>
#include "pgr.h"
#include <unordered_set>
#include "Types.h"
#include "glm/gtx/euler_angles.hpp"


namespace ryabomar {
	class Node;
	typedef std::shared_ptr <Node> p_node;
	/**
	Basic Node. It implements transformations of current node and relations with other Nodes

	*/
	class Node
	{
	public:
		virtual ~Node() {}
		/// Node id 
		int id;
		Node();
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale = glm::vec3(1.0f);

		/// returns model matrix in global space
		glm::mat4 getModelTransform();

		/**
		sets a parent to this node

		\param parent	new parent for this node
		*/
		void setParent(Node* parent);

		/**
		returnes the parent of this node

		\return		parent of this node
		*/
		Node* getParent();

		/**
		attaches new child to this node

		\param child	node to add as child
		*/
		void addChild(Node* child);


		/**
		returnes a set of all children
		*/
		std::unordered_set<Node*>& getChildren();

		/**
		checks if a given node is a child of this node

		\return true	a given node is a child
		\return false	a given node is not a child
		*/
		bool isChild(Node* child);

		/**
			returns right vector in local space
		*/
		glm::vec3 getRight();

		/**
			returns right vector in global space
		*/
		glm::vec3 getGlobalRight();

		/**
			returns up vector in local space
		*/
		glm::vec3 getUp();

		/**
			returns forward vector in local space
		*/
		glm::vec3 getForward();

		/**
			returns up vector in global space
		*/
		glm::vec3 getGlobalUp();

		/**
			returns forward vector in global space
		*/
		glm::vec3 getGlobalForward();

		/**
			returns position vector in global space
		*/
		glm::vec3 getGlobalPosition();

		/// should this node be drawn or not
		bool isActive;

	private:
		Node* parent = nullptr;
		std::unordered_set<Node*> children;

		/// global ineger that is used in constructor as increment to set node id
		static int lastId;

	};

}