#include "scene.h"

#include "d3d12Utils/GeometryGenerator.h"

namespace render {
		
	Shape::Shape(RenderD12& render, const Model& model)
		: render_(render), model_(model)
	{	
		//model_ = createModel();
	}
	
	
	void Shape::createGeom()
	{
		geo_ = std::move(render_.createGeometry("shapeGeo", model_.vertices(), model_.indices()));
	}

	RenderItem* Shape::createRenderItem(const string& name, float x, float y, float z)
	{
		if (geo_ == nullptr)
		{
			createGeom();
		}
		auto ritem = std::make_unique<RenderItem>();
		DirectX::XMMATRIX rightCylWorld = DirectX::XMMatrixTranslation(x, y, z);
		DirectX::XMStoreFloat4x4(&ritem->World, rightCylWorld);
		ritem->ObjCBIndex = objCBIndex_++;
		ritem->Geo = geo_.get();
		ritem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ritem->IndexCount = model_.submesh(name).IndexCount;
		ritem->StartIndexLocation = model_.submesh(name).StartIndexLocation;
		ritem->BaseVertexLocation = model_.submesh(name).BaseVertexLocation;
		allRitems_.push_back(std::move(ritem));
		
		return allRitems_.back().get();
	}

	Scene::Scene(RenderD12& render)
		: render_(render)
	{}

	void Scene::createShape(const string& name, const Model& model)
	{
		shape_ = std::make_unique<Shape>(render_, model);
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
			buildRenderItem();
			isCreated_ = true;
		}
		render_.setOpaque(opaqueRitems_);
	}

	
	void Scene::buildRenderItem()
	{
		auto boxRitem = shape_->createRenderItem(name_, 0.0f, 0.5f, 0.0f);
		opaqueRitems_.push_back(boxRitem);

		auto boxRitem1 = shape_->createRenderItem(name_, 0.0f, 0.5f, 0.0f);
		opaqueRitems_.push_back(boxRitem1);

		/*auto boxRitem = shape_->createRenderItem("box", 0.0f, 0.5f, 0.0f);		
		opaqueRitems_.push_back(boxRitem);

		auto gridRitem = shape_->createRenderItem("grid", 0.0f, 0.0f, 0.0f);
		opaqueRitems_.push_back(gridRitem);

		for (int i = 0; i < 5; ++i)
		{

			auto leftCylRitem = shape_->createRenderItem("cylinder", +5.0f, 1.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(leftCylRitem);

			auto rightCylRitem = shape_->createRenderItem("cylinder", -5.0f, 1.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(rightCylRitem);

			auto leftSphereRitem = shape_->createRenderItem("sphere", -5.0f, 3.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(leftSphereRitem);

			auto rightSphereRitem = shape_->createRenderItem("sphere", +5.0f, 3.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(rightSphereRitem);
		}*/				
	}
}