#pragma once

#include <unordered_map>

#include "core/types.h"

#include "d3d12Utils/d3dUtil.h"

#include "render.h"


namespace render {
	class Scene
	{
	public:
		Scene(RenderD12& render);

		bool initialize();
		void draw();

	private:
		void buildGeometry();
		void buildRenderItem();

	private:
		RenderD12& render_;
		std::unordered_map<string, std::unique_ptr<MeshGeometry>> geometries_;
		// List of all the render items.
		std::vector<std::unique_ptr<RenderItem>> allRitems_;
		std::vector<RenderItem*> opaqueRitems_;
		bool isCreated_ = false;
	};
}