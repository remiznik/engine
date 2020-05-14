#include "resource_manager.h"

#include <iostream>

namespace render {
		
	ResourceManager::ResourceManager(core::FileSystem& fileSystem)
		: fileSystem_(fileSystem)
	{

	}
	
	Model ResourceManager::getModel(const string& name)
	{
		auto it = models_.find(name);
		if (it != models_.end())
		{
			return it->second;
		}
		else
		{
			auto model = loadModel(name);
			auto res = models_.emplace(name, model);
			return res.first->second;
		}
	}

	Model ResourceManager::loadModel(const string& name)
	{
		auto fileName = "../res/Models/" + name;
		auto s = fileSystem_.readFile(fileName);
		std::cout << s << std::endl;

		return Model();
	}
}