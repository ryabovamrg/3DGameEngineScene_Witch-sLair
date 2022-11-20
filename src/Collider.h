#pragma once
#include "pgr.h"
#include "Node.h"
#include <algorithm>
#include <iostream>

namespace ryabomar {
	/**
	Node that detects nodes' intersections
	*/
	class Collider : public Node
	{
	public:
		//collider type
		enum Type { SPHERE_COLLIDER, BOX_COLLIDER };
		Type  type;

		/**
		checks if current collider is intersecting with another collider
		\param other	another collder
		*/
		virtual bool  isIntersecting(Collider* other) = 0;

		/**
		checks if current collider is intersecting with given point
		\param point	given point
		*/
		virtual bool  isPointInside(glm::vec3 point) = 0;

	};

	/**
	Collider in the shape of box
	*/
	class BoxCollider : public Collider {
	public:
		glm::vec3 aabb_min, aabb_max;
		BoxCollider(glm::vec3 aabb_min, glm::vec3 aabb_max);
		virtual bool  isIntersecting(Collider* other) override;
		virtual bool  isPointInside(glm::vec3 point)  override;
	};

	/**
	Collider in the shape of sphere
	*/
	class SphereCollider : public Collider {
	public:
		float bounderRadius;
		SphereCollider(float bounderRadius);
		virtual bool  isIntersecting(Collider* other) override;
		virtual bool  isPointInside(glm::vec3 point)  override;
	};
}