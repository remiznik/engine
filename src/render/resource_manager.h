#pragma once

#include "core/types.h"
#include "core/file_system.h"
#include "model.h"

namespace render {

	class ResourceManager
	{
	public:
		ResourceManager(core::FileSystem& fileSystem);

		Model getModel(const string& name);

	private:
		Model loadModel(const string& name);

	private:
		const core::FileSystem& fileSystem_;
		unordered_map<string, Model> models_;
	};
}