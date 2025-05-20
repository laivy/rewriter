#include "Stdafx.h"
#include "FbxHandler.h"
#include "fbxsdk.h"

namespace
{
	std::vector<Resource::Model::Mesh::Vertex> LoadVertices(fbxsdk::FbxMesh* mesh)
	{
		using EMappingMode = fbxsdk::FbxGeometryElement::EMappingMode;
		using EReferenceMode = fbxsdk::FbxGeometryElement::EReferenceMode;

		auto GetVertexPosition =
			[mesh](int controlPointIndex)
			{
				auto controlPoint{ mesh->GetControlPointAt(controlPointIndex) };
				Float3 position{};
				position.x = static_cast<float>(controlPoint[0]);
				position.y = static_cast<float>(controlPoint[1]);
				position.z = static_cast<float>(controlPoint[2]);
				return position;
			};

		auto GetVertexNormal =
			[mesh](int vertexIndex, int controlPointIndex)
			{
				Float3 normal{};
				if (mesh->GetElementNormalCount() <= 0)
					return normal;

				auto elementNormal{ mesh->GetElementNormal() };
				switch (elementNormal->GetMappingMode())
				{
				case EMappingMode::eByControlPoint:
				{
					fbxsdk::FbxVector4 vector{};
					switch (elementNormal->GetReferenceMode())
					{
					case EReferenceMode::eDirect:
					{
						vector = elementNormal->GetDirectArray().GetAt(controlPointIndex);
						break;
					}
					case EReferenceMode::eIndexToDirect:
					{
						int index{ elementNormal->GetIndexArray().GetAt(controlPointIndex) };
						vector = elementNormal->GetDirectArray().GetAt(index);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}
					break;
				}
				case EMappingMode::eByPolygonVertex:
				{
					fbxsdk::FbxVector4 vector{};
					switch (elementNormal->GetReferenceMode())
					{
					case EReferenceMode::eDirect:
					{
						vector = elementNormal->GetDirectArray().GetAt(vertexIndex);
						break;
					}
					case EReferenceMode::eIndexToDirect:
					{
						int index{ elementNormal->GetIndexArray().GetAt(vertexIndex) };
						vector = elementNormal->GetDirectArray().GetAt(index);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}

					normal.x = static_cast<float>(vector[0]);
					normal.y = static_cast<float>(vector[1]);
					normal.z = static_cast<float>(vector[2]);
					break;
				}
				default:
					assert(false && "INVALID MAPPING MODE");
					break;
				}
				return normal;
			};

		auto GetVertexUV =
			[mesh](int vertexIndex, int controlPointIndex)
			{
				Float2 uv{};
				if (mesh->GetElementUVCount() <= 0)
					return uv;

				auto elementUV{ mesh->GetElementUV() };
				switch (elementUV->GetMappingMode())
				{
				case EMappingMode::eByControlPoint:
				{
					fbxsdk::FbxVector4 vector{};
					switch (elementUV->GetReferenceMode())
					{
					case EReferenceMode::eDirect:
					{
						vector = elementUV->GetDirectArray().GetAt(controlPointIndex);
						break;
					}
					case EReferenceMode::eIndexToDirect:
					{
						int index{ elementUV->GetIndexArray().GetAt(controlPointIndex) };
						vector = elementUV->GetDirectArray().GetAt(index);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}
					break;
				}
				case EMappingMode::eByPolygonVertex:
				{
					fbxsdk::FbxVector2 vector{};
					switch (elementUV->GetReferenceMode())
					{
					case EReferenceMode::eDirect:
					{
						vector = elementUV->GetDirectArray().GetAt(vertexIndex);
						break;
					}
					case EReferenceMode::eIndexToDirect:
					{
						int index{ elementUV->GetIndexArray().GetAt(vertexIndex) };
						vector = elementUV->GetDirectArray().GetAt(index);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}

					uv.x = static_cast<float>(vector[0]);
					uv.y = static_cast<float>(vector[1]);
					break;
				}
				default:
					assert(false && "INVALID MAPPING MODE");
					break;
				}
				return uv;
			};

		auto GetMaterialIndex =
			[mesh](int vertexIndex, int controlPointIndex)
			{
				int index{ -1 };
				if (mesh->GetElementMaterialCount() <= 0)
					return index;

				auto elementMaterial{ mesh->GetElementMaterial() };
				switch (elementMaterial->GetMappingMode())
				{
				case EMappingMode::eByControlPoint:
				{
					switch (elementMaterial->GetReferenceMode())
					{
					case EReferenceMode::eIndexToDirect:
					{
						index = elementMaterial->GetIndexArray().GetAt(controlPointIndex);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}
					break;
				}
				case EMappingMode::eByPolygonVertex:
				{
					switch (elementMaterial->GetReferenceMode())
					{
					case EReferenceMode::eIndexToDirect:
					{
						index = elementMaterial->GetIndexArray().GetAt(vertexIndex);
						break;
					}
					default:
						assert(false && "INVALID REFERENCE MODE");
						break;
					}
					break;
				}
				case EMappingMode::eAllSame:
				{
					index = 0;
					break;
				}
				default:
					assert(false && "INVALID MAPPING MODE");
					break;
				}
				return index;
			};

		std::vector<Resource::Model::Mesh::Vertex> vertices(mesh->GetControlPointsCount());

		int vertexIndex{ 0 };
		int polygonCount{ mesh->GetPolygonCount() };
		for (int i{ 0 }; i < polygonCount; ++i)
		{
			for (int j{ 0 }; j < 3; ++j)
			{
				int controlPointIndex{ mesh->GetPolygonVertex(i, j) };
				auto& vertex{ vertices.at(controlPointIndex) };
				vertex.position = GetVertexPosition(controlPointIndex);
				vertex.normal = GetVertexNormal(vertexIndex, controlPointIndex);
				vertex.uv = GetVertexUV(vertexIndex, controlPointIndex);
				vertex.materialIndex = GetMaterialIndex(vertexIndex, controlPointIndex);
				++vertexIndex;
			}
		}
		return vertices;
	}

	std::vector<int> LoadIndices(fbxsdk::FbxMesh* mesh)
	{
		std::vector<int> indices(mesh->GetPolygonCount() * 3ULL);
		std::memcpy(indices.data(), mesh->GetPolygonVertices(), sizeof(int) * indices.size());
		return indices;
	}

	std::vector<Resource::Model::Mesh::Material> LoadMaterials(fbxsdk::FbxMesh* mesh)
	{
		std::vector<Resource::Model::Mesh::Material> materials;

		auto node{ mesh->GetNode() };
		if (!node)
			return materials;

		int materialCount{ node->GetMaterialCount() };
		for (int i{ 0 }; i < materialCount; ++i)
		{
			Resource::Model::Mesh::Material material{};

			auto fbxMaterial{ node->GetMaterial(i) };
			if (auto prop{ fbxMaterial->FindProperty("DiffuseColor") }; prop.IsValid())
			{
				auto value{ prop.Get<FbxDouble3>() };
				material.diffuse.x = static_cast<float>(value[0]);
				material.diffuse.y = static_cast<float>(value[1]);
				material.diffuse.z = static_cast<float>(value[2]);
			}

			materials.push_back(material);
		}

		return materials;
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
					mesh.materials = LoadMaterials(fbxMesh);
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
