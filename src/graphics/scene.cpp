#include "scene.h"
#include "shape.h"


namespace engine
{

	Scene::Scene() 
	{		
	}

	void Scene::addShape(const std::shared_ptr<Shape>& shape)
	{
		shapes_.push_back(shape);
	}

	bool Scene::draw()
	{
		return true;
	}
}
