#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <Common/Type.h>

namespace Resource
{
	struct Model
	{
		struct Mesh
		{
			struct Vertex
			{
				Float3 position;
				Float3 normal;
				Float2 uv;
				int32_t materialIndex;
			};

			struct Material
			{
				Float3 diffuse;
			};

			std::unique_ptr<void, std::function<void(void*)>> vertexBuffer; // Graphics::VertexBuffer
			std::unique_ptr<void, std::function<void(void*)>> indexBuffer; // Graphics::IndexBuffer
#ifdef _TOOL
			std::vector<Vertex> vertices;
			std::vector<int> indices;
#endif
			std::vector<Material> materials;
		};

		std::vector<Mesh> meshes;
	};
}
