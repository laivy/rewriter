#include "Stdafx.h"
#include "FbxHandler.h"
#include "fbxsdk.h"

namespace
{
	std::vector<Resource::Model::Vertex> LoadVertices(fbxsdk::FbxMesh* mesh)
	{
		/*
		* 제어점(Control Point)은 중복되지 않는 정점이다.
		* ex. 사각형은 4개의 제어점, 6개의 정점(=2개의 삼각형)으로 이루어져있다.
		*/

		int count{ mesh->GetControlPointsCount() };
		std::vector<Resource::Model::Vertex> vertices(count);
		for (int i{ 0 }; i < count; ++i)
		{
			auto point{ mesh->GetControlPointAt(i) };
			vertices[i].position.x = static_cast<float>(point[0]);
			vertices[i].position.y = static_cast<float>(point[1]);
			vertices[i].position.z = static_cast<float>(point[2]);
		}
		return vertices;
	}

	std::vector<int> LoadIndices(fbxsdk::FbxMesh* mesh)
	{
		std::vector<int> indices(mesh->GetPolygonCount() * 3ULL);
		std::memcpy(indices.data(), mesh->GetPolygonVertices(), sizeof(int) * indices.size());
		return indices;
	}

	std::vector<Resource::Model::Mesh> LoadMeshes(fbxsdk::FbxNode* root)
	{
		std::vector<Resource::Model::Mesh> meshes;

		std::function<void(fbxsdk::FbxNode*)> recursive =
			[&meshes, &recursive](fbxsdk::FbxNode* node)
			{
				if (auto attr{ node->GetNodeAttribute() }; attr && attr->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh)
				{
					auto fbxMesh{ node->GetMesh() };
					auto vertices{ LoadVertices(fbxMesh) };
					auto indices{ LoadIndices(fbxMesh) };

					auto& mesh{ meshes.emplace_back() };
					mesh.vertices = std::move(vertices);
					mesh.indices = std::move(indices);
				}

				int count{ node->GetChildCount() };
				for (int i{ 0 }; i < count; ++i)
					recursive(node->GetChild(i));
			};

		int count{ root->GetChildCount() };
		for (int i{ 0 }; i < count; ++i)
			recursive(root->GetChild(i));
		return meshes;
	}
}

FbxHandler::FbxHandler() :
	m_manager{ fbxsdk::FbxManager::Create() },
	m_scene{ fbxsdk::FbxScene::Create(m_manager, "Rewriter") }
{
	auto ioSettings{ fbxsdk::FbxIOSettings::Create(m_manager, IOSROOT) };
	m_manager->SetIOSettings(ioSettings);
}

FbxHandler::~FbxHandler()
{
	if (m_manager)
		m_manager->Destroy();
}

std::shared_ptr<Resource::Model> FbxHandler::Load(std::filesystem::path path)
{
	if (auto importer{ fbxsdk::FbxImporter::Create(m_manager, "Importer") })
	{
		if (!importer->Initialize(path.string().c_str(), -1, m_manager->GetIOSettings()))
			return nullptr;

		if (!importer->Import(m_scene))
			return nullptr;

		importer->Destroy();
	}

	fbxsdk::FbxGeometryConverter converter{ m_manager };
	converter.Triangulate(m_scene, true);

	auto root{ m_scene->GetRootNode() };
	auto meshes{ LoadMeshes(root) };

	auto model{ std::make_shared<Resource::Model>() };
	model->meshes = std::move(meshes);
	return model;
}
