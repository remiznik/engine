#include "model.h"

#include "d3d12Utils/GeometryGenerator.h"

namespace render {

	core::math::Vector2 convert(DirectX::XMFLOAT2 pos)
	{
		return core::math::Vector2(pos.x, pos.y);
	}

	core::math::Vector3 convert(DirectX::XMFLOAT3 pos)
	{
		return core::math::Vector3(pos.x, pos.y, pos.z);
	}

	core::math::Vector4 convert(DirectX::XMFLOAT4 pos)
	{
		return core::math::Vector4(pos.x, pos.y, pos.z, pos.w);
	}

	Model createModel()
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
		
		vector<core::math::Vertex> vertices;
		vertices.resize(totalVertexCount);
		
		UINT k = 0;
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
		{
			vertices[k].pos = convert(box.Vertices[i].Position);
			vertices[k].color = convert(DirectX::XMFLOAT4(DirectX::Colors::DarkGreen));
		}
		
		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
		{
			vertices[k].pos = convert(grid.Vertices[i].Position);
			vertices[k].color = convert(DirectX::XMFLOAT4(DirectX::Colors::ForestGreen));
		}
		
		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
		{
			vertices[k].pos = convert(sphere.Vertices[i].Position);
			vertices[k].color = convert(DirectX::XMFLOAT4(DirectX::Colors::Crimson));
		}
		
		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
		{
			vertices[k].pos = convert(cylinder.Vertices[i].Position);
			vertices[k].color = convert(DirectX::XMFLOAT4(DirectX::Colors::SteelBlue));
		}
		
		vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
		indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
		indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
		indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

		unordered_map<std::string, SubmeshGeometry> subMeshes;
		subMeshes["box"] = boxSubmesh;
		subMeshes["grid"] = gridSubmesh;
		subMeshes["sphere"] = sphereSubmesh;
		subMeshes["cylinder"] = cylinderSubmesh;
		
		return Model(vertices, indices, subMeshes);
	}


	Model::Model(const vector<core::math::Vertex>& vertices, const vector<std::uint16_t>& indices, const unordered_map<std::string, SubmeshGeometry>& subMeshes)
		: vertices_(vertices), indices_(indices), subMeshes_(subMeshes)
	{
	}

	SubmeshGeometry Model::submesh(const std::string& name) const
	{
		auto it = subMeshes_.find(name);
		if (it != subMeshes_.end())
		{
			return it->second;
		}
		return SubmeshGeometry();
	}
}