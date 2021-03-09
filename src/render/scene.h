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

		void createShape(const string& name, const Model& model);

	private:
		RenderD12& render_;		
		bool isCreated_ = false;
		std::unique_ptr<Shape> shape_{ nullptr };
		string name_;
	};
}