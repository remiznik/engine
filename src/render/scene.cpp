#include "scene.h"

#include "d3d12Utils/GeometryGenerator.h"

namespace render {
	
	Shape::Shape(RenderD12& render)
		: render_(render)
	{	

	}

	void Shape::createGeom()
	{
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
		GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
		GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		//
		// We are concatenating all the geometry into one big vertex/index buffer.  So
		// define the regions in the buffer each submesh covers.
		//

		// Cache the vertex offsets to each object in the concatenated vertex buffer.
		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = (UINT)box.Vertices.size();
		UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
		UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

		// Cache the starting index for each object in the concatenated index buffer.
		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = (UINT)box.Indices32.size();
		UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
		UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

		// Define the SubmeshGeometry that cover different 
		// regions of the vertex/index buffers.

		SubmeshGeometry boxSubmesh;
		boxSubmesh.IndexCount = (UINT)box.Indices32.size();
		boxSubmesh.StartIndexLocation = boxIndexOffset;
		boxSubmesh.BaseVertexLocation = boxVertexOffset;

		SubmeshGeometry gridSubmesh;
		gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
		gridSubmesh.StartIndexLocation = gridIndexOffset;
		gridSubmesh.BaseVertexLocation = gridVertexOffset;

		SubmeshGeometry sphereSubmesh;
		sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
		sphereSubmesh.StartIndexLocation = sphereIndexOffset;
		sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

		SubmeshGeometry cylinderSubmesh;
		cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
		cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
		cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//

		auto totalVertexCount =
			box.Vertices.size() +
			grid.Vertices.size() +
			sphere.Vertices.size() +
			cylinder.Vertices.size();

		std::vector<Vertex> vertices(totalVertexCount);

		UINT k = 0;
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = box.Vertices[i].Position;
			vertices[k].Color = DirectX::XMFLOAT4(DirectX::Colors::DarkGreen);
		}

		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = grid.Vertices[i].Position;
			vertices[k].Color = DirectX::XMFLOAT4(DirectX::Colors::ForestGreen);
		}

		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = sphere.Vertices[i].Position;
			vertices[k].Color = DirectX::XMFLOAT4(DirectX::Colors::Crimson);
		}

		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = cylinder.Vertices[i].Position;
			vertices[k].Color = DirectX::XMFLOAT4(DirectX::Colors::SteelBlue);
		}

		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		std::unique_ptr<MeshGeometry> geo(render_.createGeometry("shapeGeo", vertices, indices));

		geo->DrawArgs["box"] = boxSubmesh;
		geo->DrawArgs["grid"] = gridSubmesh;
		geo->DrawArgs["sphere"] = sphereSubmesh;
		geo->DrawArgs["cylinder"] = cylinderSubmesh;

		geometries_[geo->Name] = std::move(geo);

	}

	RenderItem* Shape::createRenderItem(const char* name, float x, float y, float z)
	{
		if (geometries_.empty())
		{
			createGeom();
		}
		auto ritem = std::make_unique<RenderItem>();
		DirectX::XMMATRIX rightCylWorld = DirectX::XMMatrixTranslation(x, y, z);
		DirectX::XMStoreFloat4x4(&ritem->World, rightCylWorld);
		ritem->ObjCBIndex = objCBIndex_++;
		ritem->Geo = geometries_["shapeGeo"].get();
		ritem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ritem->IndexCount = ritem->Geo->DrawArgs[name].IndexCount;
		ritem->StartIndexLocation = ritem->Geo->DrawArgs[name].StartIndexLocation;
		ritem->BaseVertexLocation = ritem->Geo->DrawArgs[name].BaseVertexLocation;
		allRitems_.push_back(std::move(ritem));
		
		return allRitems_.back().get();
	}

	Scene::Scene(RenderD12& render)
		: render_(render), shape_(render)
	{}

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
		auto boxRitem = shape_.createRenderItem("box", 0.0f, 0.5f, 0.0f);		
		opaqueRitems_.push_back(boxRitem);

		auto gridRitem = shape_.createRenderItem("grid", 0.0f, 0.0f, 0.0f);
		opaqueRitems_.push_back(gridRitem);

		for (int i = 0; i < 5; ++i)
		{

			auto leftCylRitem = shape_.createRenderItem("cylinder", +5.0f, 1.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(leftCylRitem);

			auto rightCylRitem = shape_.createRenderItem("cylinder", -5.0f, 1.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(rightCylRitem);

			auto leftSphereRitem = shape_.createRenderItem("sphere", -5.0f, 3.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(leftSphereRitem);

			auto rightSphereRitem = shape_.createRenderItem("sphere", +5.0f, 3.5f, -10.0f + i * 5.0f);
			opaqueRitems_.push_back(rightSphereRitem);
		}	
			
	}
}