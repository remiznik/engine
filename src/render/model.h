#pragma once

#include "core/types.h"

#include "utils.h"

namespace render {

	class Model
	{
	public:
		Model() = default;
		Model(const vector<math::Vertex>& vertices, const vector<std::uint16_t>& indices);

		const vector<math::Vertex>& vertices() const { return vertices_; }
		const vector<uint16_t>& indices() const { return indices_; }

	private:
		vector<math::Vertex> vertices_;
		vector<uint16_t> indices_;		
	};

}