#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Node.h"
#include "Util.h"

Hierarchy::Hierarchy()
{
	m_onNodeSelect = { std::bind_front(&Hierarchy::OnNodeSelect, this) };
	Global::OnPropertySelect.Add(&m_onNodeSelect);
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Update(float deltaTime)
{
}

void Hierarchy::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME, NULL, ImGuiWindowFlags_MenuBar))
	{
		ProcessDragDrop();
		RenderMenu();
		RenderNode();
		DeleteInvalidNodes();
	}
	ImGui::End();
	ImGui::PopID();
}

void Hierarchy::RenderMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(MENU_FILE))
		{
			if (ImGui::MenuItem(MENU_FILE_NEW))
				OnMenuFileNew();
			if (ImGui::MenuItem(MENU_FILE_OPEN))
				OnMenuFileOpen();
			if (ImGui::MenuItem(MENU_FILE_SAVE))
				OnMenuFileSave();
			if (ImGui::MenuItem(MENU_FILE_SAVEAS))
				OnMenuFileSaveAs();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Hierarchy::RenderNode()
{
	auto menu =
		[](Resource::Property* prop)
		{
			if (ImGui::BeginPopupContextItem("POPUP_CONTEXT_ITEM"))
			{
				if (ImGui::Selectable("Add"))
				{
					auto child{ std::make_shared<Resource::Property>() };
					child->SetName(L"New Node");
					prop->Add(child);
					Global::OnPropertyAdd.Notify(nullptr);
				}
				ImGui::EndPopup();
			}
		};

	std::function<void(Resource::Property*)> render =
		[&](Resource::Property* prop)
		{
			ImGui::PushID(prop);
			if (prop->children.empty())
			{
				if (ImGui::Selectable(Util::wstou8s(prop->GetName()).c_str()))
					Global::OnPropertySelect.Notify(nullptr);
				menu(prop);
			}
			else
			{
				ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick };
				if (ImGui::TreeNodeEx(Util::wstou8s(prop->GetName()).c_str(), flag))
				{
					if (ImGui::IsItemClicked())
						Global::OnPropertySelect.Notify(nullptr);

					menu(prop);

					for (const auto& [_, child] : *prop)
						render(child.get());

					ImGui::TreePop();
				}
			}
			ImGui::PopID();
		};

	ImGui::PushID("NODE");
	for (const auto& node : m_roots)
	{
		// 노드는 트리노드로 렌더링		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 5.0f });
		ImGuiTreeNodeFlags flag{ 
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | 
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth
		};
		if (ImGui::TreeNodeEx(Util::wstou8s(node->name).c_str(), flag))
		{
			if (ImGui::IsItemClicked())
				Global::OnPropertySelect.Notify(nullptr);

			menu(node.get());

			// 프로퍼티 렌더링
			for (const auto& child : node->children)
				render(child.get());

			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	ImGui::PopID();
}

void Hierarchy::DeleteInvalidNodes()
{
	//std::erase_if(m_roots, [](const auto& root) { return !root->IsValid(); });
	//for (const auto& root : m_roots)
	//	root->DeleteInvalidChildren();
}

void Hierarchy::ProcessDragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
	{
		if (auto payload{ ImGui::AcceptDragDropPayload("FILE_TO_HIERARCHY") })
		{
			std::string_view filePath{ static_cast<const char*>(payload->Data) };
			OnFileDragDrop(filePath.data());
		}
		ImGui::EndDragDropTarget();
	}
}

void Hierarchy::OnFileDragDrop(std::string_view path)
{
}

void Hierarchy::OnMenuFileNew()
{
	size_t index{ 1 };
	std::wstring name{ DEFAULT_FILE_NAME };
	while (true)
	{
		auto it{ std::ranges::find_if(m_roots, [&name](const auto& node)
			{
				return name == node->name;
			}) };

		if (it == m_roots.cend())
			break;

		name = std::format(L"{}{}", DEFAULT_FILE_NAME, index++);
	}

	auto prop{ std::make_shared<Resource::Property>() };
	prop->SetName(name);
	m_roots.push_back(prop);
}

void Hierarchy::OnMenuFileOpen()
{
	std::wstring filePath(MAX_PATH, L'\0');

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = filePath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"dat";
	GetOpenFileName(&ofn);

	// 경로가 비어있는지 확인
	std::erase(filePath, L'\0');
	if (filePath.empty())
		return;

	// 역슬래시를 슬래시로 변환
	std::ranges::replace(filePath, L'\\', L'/');

	// 로드
	auto prop{ Resource::Load(filePath) };
	m_roots.push_back(prop);
}

void Hierarchy::OnMenuFileSave()
{
	//PropInfo* node{ nullptr };
	//if (auto inspector{ Inspector::GetInstance() })
	//	node = inspector->GetNode();
	//if (!node)
	//	return;
	//while (node->GetParent())
	//	node = node->GetParent();
	//assert(node->IsRoot() && "NODE MUST BE ROOT NODE");
	//node->Save();
}

void Hierarchy::OnMenuFileSaveAs()
{
}

void Hierarchy::OnNodeSelect(std::shared_ptr<Resource::Property> prop)
{
	//std::function<void(Node*)> lambda = [&](Node* n)
	//	{
	//		// Ctrl키를 누르고 있으면 누른 노드 추가 선택
	//		if (ImGui::GetIO().KeyCtrl)
	//		{
	//			//if (n == node)
	//			//	n->SetSelect(true);
	//			return;
	//		}

	//		// 선택한 노드 외에 전부 선택 해제
	//		//n->SetSelect(n == node ? true : false);

	//		// 재귀
	//		for (auto& child : *n)
	//			lambda(child.get());
	//	};

	//for (auto& root : m_roots)
	//	for (auto& child : *root)
	//		lambda(child.get());
}