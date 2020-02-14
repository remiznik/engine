#include "scene.h"
#include "shape.h"

namespace engine
{

	Scene::Scene() :
		renderTarget_(sf::VideoMode(200, 200), "SFML works!")
	{		
	}

	void Scene::addShape(const std::shared_ptr<Shape>& shape)
	{
		shapes_.push_back(shape);
	}
	void Scene::draw()
	{
		renderTarget_.clear();
		for (auto shape: shapes_)
		{
			shape->draw(renderTarget_);
		}
		renderTarget_.display();
	}
}