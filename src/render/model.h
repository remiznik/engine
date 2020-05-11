#pragma once

#include <unordered_map>

#include "d3d12Utils/d3dUtil.h"

namespace render {

		class Model
	{
	public:
		Model();
		
		SubmeshGeometry submesh(const std::string& name) const;

		const std::vector<Vertex>& vertices() const { return vertices_; }
		const std::vector<std::uint16_t>& indices() const { return indices_; }

	private:
		std::vector<Vertex> vertices_;
		std::vector<std::uint16_t> indices_;
		std::unordered_map<std::string, SubmeshGeometry> subMeshes_;
	};
}