#include "scene.h"

namespace render {
	
	Scene::Scene(RenderD12& render)
		: render_(render)
	{}

	shared_ptr<Shape> Scene::createShape(const Model& model)
	{
		auto res = shapes_.insert(std::make_unique<Shape>(model));
		return *res.first;
	}
	bool Scene::initialize()
	{	
		return true;
	}

	void Scene::draw()
	{
		if (!isCreated_)
		{	
			for (auto& shape : shapes_)
			{
				render_.createRenderItem(shape->model().vertices(), shape->model().indices(), shape->position());
			}
			isCreated_ = true;
		}		
	}
}