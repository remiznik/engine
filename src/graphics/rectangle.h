#pragma once
#include "shape.h"

namespace engine
{
	class Rectangle : public Shape
	{
	public:
		Rectangle();
		
		virtual void draw() override;
		virtual void setPosition(float x, float y) override;
		virtual void setSize(int w, int h) override;
	};
}