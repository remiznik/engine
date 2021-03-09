#include "scene.h"

namespace render {
	
	Scene::Scene(RenderD12& render)
		: render_(render)
	{}

	void Scene::createShape(const string& name, const Model& model)
	{
		shape_ = std::make_unique<Shape>(model);
		name_ = name;
	}

	bool Scene::initialize()
	{	
		return true;
	}

	void Scene::draw()
	{
		if (!isCreated_)
		{	
			isCreated_ = true;
			render_.createRenderItem(shape_->model().vertices(), shape_->model().indices());
		}		
	}
}