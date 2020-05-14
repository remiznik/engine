#pragma once

#include "core/types.h"
#include "core/m_math.h"

#include "d3d12Utils/d3dUtil.h"

namespace render {

	

	class Model
	{
	public:
		Model() = default;
		Model(const vector<core::math::Vertex>& vertices,  	const vector<std::uint16_t>& indices,
		const unordered_map<std::string, SubmeshGeometry>& subMeshes);
		
		SubmeshGeometry submesh(const std::string& name) const;

		const vector<core::math::Vertex>& vertices() const { return vertices_; }
		const vector<std::uint16_t>& indices() const { return indices_; }

	private:
		vector<core::math::Vertex> vertices_;
		vector<std::uint16_t> indices_;
		unordered_map<std::string, SubmeshGeometry> subMeshes_;
	};

	Model createModel();
}