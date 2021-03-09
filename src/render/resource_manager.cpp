#include "resource_manager.h"

#include "utils.h"

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

		vector<math::Vertex> vertices;
		for (int i = 0; i < vertexCount; ++i)
		{
			float vertex[6];
			stream >> vertex[0] >> vertex[1] >> vertex[2] >> vertex[3] >> vertex[4] >> vertex[5];

			math::Vertex v;
			v.pos = math::Vector3(vertex[0], vertex[1], vertex[2]);
			v.color = math::Vector4(vertex[3], vertex[4], vertex[5], 0);
			
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

		
		return Model(vertices, indecses);
	}

	Model ResourceManager::getObjModel(const string& name)
	{
		auto fileName = "../res/Models/" + name;
		auto s = fileSystem_.readFile(fileName);
		std::stringstream  stream(s);

		string ss;
		std::getline(stream, ss);
		while (ss != "v")
		{
			stream >> ss;
		}

		vector<math::Vertex> vertices;		
		while (ss == "v")
		{
			float x, y, z;
			stream >> x >> y >> z >> ss;
			math::Vertex v;
			v.pos = math::Vector3(x, y, z);
			v.color = math::Vector4(1, 1, 1, 0);
			vertices.push_back(v);
		}

		//while (ss == "vt")
		//{
		//	float x, y;
		//	stream >> x >> y >> ss;
		//}
		//
		//while (ss == "vn")
		//{
		//	float x, y, z;
		//	stream >> x >> y >> z >> ss;			
		//}
		stream >> ss >> ss ;
		vector<uint16_t> indecses;
		while (ss == "f")
		{
			string line;
			std::getline(stream, line);
			if (line.empty())
				break;
			
			std::stringstream sLine(line);
			for (string item; std::getline(sLine, item, ' ');)
			{
				if (!item.empty())
				{
					indecses.push_back(std::stoi(item) - 1);
				}
			}
			stream >> ss;
		}	
		
		
		return Model(vertices, indecses);
	}

}