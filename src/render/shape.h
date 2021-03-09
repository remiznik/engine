#pragma once

#include "model.h"

namespace render {

	class Shape
	{
	public:
		Shape(const Model& model);
		const Model& model() const { return model_; }

	private:
		Model model_;
	};
}