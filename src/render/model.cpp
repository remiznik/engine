#include "model.h"

namespace render {

	Model::Model(const vector<math::Vertex>& vertices, const vector<std::uint16_t>& indices)
		: vertices_(vertices), indices_(indices)
	{}
}