#pragma once

namespace Resource
{
	struct Model
	{
		struct Mesh
		{
			std::vector<Float3> vertices;
			std::vector<int> indices;
		};

		std::vector<Mesh> meshes;
	};
}
