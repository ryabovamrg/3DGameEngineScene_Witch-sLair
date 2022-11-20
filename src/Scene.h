#pragma once

#include "pgr.h"
#include "Node.h"
#include "Interfaces.h"
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "Collider.h"
namespace ryabomar {
	/**
	Collection of Nodes
	*/
	class Scene {
	public:
		Scene() {}
		/**
		Destructor that deletes all nodes to free memory
		*/
		~Scene() { for (Node* node : nodes) delete node; }

		/**
		Returns all nodes stored in the scene in one vector of Node*
		*/
		std::vector <Node*>& getNodes();

		/**
		Returns all nodes that are drawable stored in the scene in one vector of IDrawable*
		*/
		std::vector <IDrawable*>& getDrawable();

		/**
		Returns all nodes that are updatable stored in the scene in one vector of IUpdatable*
		*/
		std::vector <IUpdatable*>& getUpdatable();

		/**
		Returns all nodes that are colliders stored in the scene in one vector of Collider*
		*/
		std::vector <Collider*>& getColliders();

		/**
		Adds given node to scene
		*/
		void add(Node* node);

	private:
		// primary list of all given nodes
		std::vector <Node*>       nodes;

		// list of all given drawable nodes, contains copies of pointers to objects from nodes vector
		std::vector <IDrawable*>  drawable_nodes;

		// list of all given updatable nodes, contains copies of pointers to objects from nodes vector
		std::vector <IUpdatable*> updatable_nodes;

		// list of all given colliedrs nodes, contains copies of pointers to objects from nodes vector
		std::vector <Collider*>   collider_nodes;

	};
}