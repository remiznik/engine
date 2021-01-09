#pragma once


namespace engine
{
	class Shape
	{
	public:
		virtual void draw() = 0;
		virtual void setPosition(float x, float y) = 0;
		virtual void setSize(int w, int h) = 0;
	};
}