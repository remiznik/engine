#include "resource_manager.h"

#include "core/m_math.h"
#include <iostream>
#include <sstream>

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
		std::stringstream  stream(s);

		string ss;
		int vertexCount;
		int triangleCount;
		stream >> ss >> vertexCount >> ss >>triangleCount;
		stream >> ss >>  ss >>ss >> ss;

		vector<core::math::Vertex> vertices;
		for (int i = 0; i < vertexCount; ++i)
		{
			float vertex[6];
			stream >> vertex[0] >> vertex[1] >> vertex[2] >> vertex[3] >> vertex[4] >> vertex[5];

			core::math::Vertex v;
			v.pos = core::math::Vector3(vertex[0], vertex[1], vertex[2]);
			v.color = core::math::Vector4(vertex[3], vertex[4], vertex[5], 0);
			
			vertices.push_back(v);			
		}
		stream >> ss  >> ss >> ss;

		vector<uint16_t> indecses;
		for (int i = 0; i < triangleCount; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				int ind;
				stream >> ind;
				indecses.push_back(ind);
			}
		}

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)indecses.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		return Model(vertices, indecses, { {name, submesh}});
	}
}