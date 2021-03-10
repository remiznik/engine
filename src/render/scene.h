#pragma once

#include "shape.h"
#include "render.h"

namespace render {

	class Scene
	{
	public:
		Scene(RenderD12& render);

		bool initialize();
		void draw();

		shared_ptr<Shape> createShape(const Model& model);

	private:
		RenderD12& render_;		
		bool isCreated_ = false;

		set < shared_ptr<Shape> > shapes_;
	};
}