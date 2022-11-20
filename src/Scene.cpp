#include "Scene.h"

namespace ryabomar {

	std::vector <Node*>& Scene::getNodes() { return nodes; }
	std::vector <IDrawable*>& Scene::getDrawable() { return drawable_nodes; }
	std::vector <IUpdatable*>& Scene::getUpdatable() { return updatable_nodes; }
	std::vector <Collider*>& Scene::getColliders() { return collider_nodes; }

	void Scene::add(Node* node) {
		// if we already have this node in our list, we ignore adding
		if (std::find(nodes.begin(), nodes.end(), node) != nodes.end()) {
			return;
		}
		// adding given node to primary list
		nodes.push_back(node);

		/**
		dinamic cast is used to downcast pointer from parent to child, if it's not possible,
		dynamic cast returns null_ptr. That way we are checking the type of given node and adding it's pointer
		to specific vectorts as well.

		*/

		IDrawable* drawable = dynamic_cast<IDrawable*>(node);
		if (drawable != nullptr) {
			drawable_nodes.push_back(drawable);
		}

		IUpdatable* updatable = dynamic_cast<IUpdatable*>(node);
		if (updatable != nullptr) {
			updatable_nodes.push_back(updatable);
		}

		Collider* collider = dynamic_cast<Collider*>(node);
		if (collider != nullptr) {
			collider_nodes.push_back(collider);
		}

	}
}