#pragma once

#include "utils.h"

#include "model.h"

namespace render {

	class Shape
	{
	public:
		Shape(const Model& model);
		const Model& model() const { return model_; }

		void position(const math::Vector3& pos);
		math::Vector3 position() const;

	private:
		Model model_;

		math::Vector3 position_;
	};
}