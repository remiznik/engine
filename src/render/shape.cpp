#include "shape.h"

namespace render {
		
	Shape::Shape(const Model& model)
		: model_(model), position_(0, 0, 0)
	{}

	void Shape::position(const math::Vector3& pos)
	{
		position_ = pos;
	}

	math::Vector3 Shape::position() const
	{
		return position_;
	}

}