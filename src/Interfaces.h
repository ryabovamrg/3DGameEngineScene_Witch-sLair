#pragma once

#include "Types.h"
#include "LightNode.h"
#include "Camera.h"
#include "pgr.h"

namespace ryabomar {
	/**
	Interface for everything that can be drawn
	*/
	class IDrawable {
	public:

		/** Draw function that is called every frame*/
		virtual void draw(std::vector <LightNode*>& lights, Camera* camera, int width, int height) = 0;
		/** Should this object be drawn or not*/
		virtual bool shouldDraw() = 0;
	};

	/**
	Interface for everything that can be updated
	*/
	class IUpdatable {
	public:
		/** Update object every frame */
		virtual void update(double deltaTime, double elpsedTime) = 0;
	};
}