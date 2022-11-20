#include "Collider.h"

namespace ryabomar {
	// ================= SPHERE ===========================================
	/*https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection*/
	bool  SphereCollider::isIntersecting(Collider* other) {

		if (other->type == Collider::Type::SPHERE_COLLIDER)
		{
			SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other);
			if (glm::distance(this->getGlobalPosition(), other->getGlobalPosition()) <= this->bounderRadius + otherSphere->bounderRadius) {
				return true;
			}
		}
		else if (other->type == Collider::Type::BOX_COLLIDER) {
			BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other);

			glm::vec3 global_aabb_min = otherBox->aabb_min + otherBox->getGlobalPosition();
			glm::vec3 global_aabb_max = otherBox->aabb_max + otherBox->getGlobalPosition();

			float x = std::max(global_aabb_min.x, std::min(this->getGlobalPosition().x, global_aabb_max.x));
			float y = std::max(global_aabb_min.y, std::min(this->getGlobalPosition().y, global_aabb_max.y));
			float z = std::max(global_aabb_min.z, std::min(this->getGlobalPosition().z, global_aabb_max.z));


			if (this->isPointInside(glm::vec3(x, y, z))) {
				return true;
			}

		}

		return false;
	}

	SphereCollider::SphereCollider(float radius) {

		this->bounderRadius = radius;
		this->type = Collider::Type::SPHERE_COLLIDER;
	}

	bool SphereCollider::isPointInside(glm::vec3 point) {
		if (glm::distance(point, this->getGlobalPosition()) <= this->bounderRadius) {
			return true;
		}
		return false;
	}



	// ================= BOX ===========================================

	BoxCollider::BoxCollider(glm::vec3 aabb_min, glm::vec3 aabb_max) {

		this->aabb_min = aabb_min;
		this->aabb_max = aabb_max;
		this->type = Collider::Type::BOX_COLLIDER;
	}



	bool BoxCollider::isPointInside(glm::vec3 point) {

		glm::vec3 global_aabb_min = aabb_min + this->getGlobalPosition();
		glm::vec3 global_aabb_max = aabb_max + this->getGlobalPosition();

		if (point.x > global_aabb_min.x&& point.x < global_aabb_max.x
			&& point.y > global_aabb_min.y&& point.y < global_aabb_max.y
			&& point.z > global_aabb_min.z&& point.z < global_aabb_max.z) {
			return true;
		}
		return false;
	}

	bool  BoxCollider::isIntersecting(Collider* other) {


		if (other->type == Collider::Type::SPHERE_COLLIDER)
		{
			SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other);

			glm::vec3 global_aabb_min = this->aabb_min + this->getGlobalPosition();
			glm::vec3 global_aabb_max = this->aabb_max + this->getGlobalPosition();

			float x = std::max(global_aabb_min.x, std::min(otherSphere->getGlobalPosition().x, global_aabb_max.x));
			float y = std::max(global_aabb_min.y, std::min(otherSphere->getGlobalPosition().y, global_aabb_max.y));
			float z = std::max(global_aabb_min.z, std::min(otherSphere->getGlobalPosition().z, global_aabb_max.z));


			if (this->isPointInside(glm::vec3(x, y, z))) {
				return true;
			}
		}
		else if (other->type == Collider::Type::BOX_COLLIDER) {
			BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other);

			glm::vec3 this_global_aabb_min = this->aabb_min + this->getGlobalPosition();
			glm::vec3 this_global_aabb_max = this->aabb_max + this->getGlobalPosition();
			glm::vec3 other_global_aabb_min = otherBox->aabb_min + otherBox->getGlobalPosition();
			glm::vec3 other_global_aabb_max = otherBox->aabb_max + otherBox->getGlobalPosition();

			if ((this_global_aabb_min.x <= other_global_aabb_max.x && this_global_aabb_max.x >= other_global_aabb_min.x) &&
				(this_global_aabb_min.y <= other_global_aabb_max.y && this_global_aabb_max.y >= other_global_aabb_min.y) &&
				(this_global_aabb_min.z <= other_global_aabb_max.z && this_global_aabb_max.z >= other_global_aabb_min.z)) {

				return true;
			}


		}
	}


}