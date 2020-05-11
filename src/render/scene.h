#pragma once

#include <unordered_map>

#include "core/types.h"

#include "d3d12Utils/d3dUtil.h"

#include "render.h"


namespace render {

	class Shape
	{
	public:
		Shape(RenderD12& render);


		RenderItem* createRenderItem(const char* name, float x, float y, float z);
	private:
		void createGeom();

	private:
		RenderD12& render_;
		std::vector<std::unique_ptr<RenderItem>> allRitems_;
		std::unordered_map<string, std::unique_ptr<MeshGeometry>> geometries_;
		UINT objCBIndex_{ 0 };
	};
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
		std::vector<RenderItem*> opaqueRitems_;
		bool isCreated_ = false;
		Shape shape_;
	};
}