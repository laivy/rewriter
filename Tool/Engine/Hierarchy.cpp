#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Node.h"

Hierarchy::Hierarchy()
{
	m_onNodeSelect = { std::bind_front(&Hierarchy::OnNodeSelect, this) };
	Global::OnNodeSelect.Add(&m_onNodeSelect);
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
	for (const auto& root : m_roots)
		root->Render();
}

void Hierarchy::DeleteInvalidNodes()
{
	std::erase_if(m_roots, [](const auto& root) { return !root->IsValid(); });
	for (const auto& root : m_roots)
		root->DeleteInvalidChildren();
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
	std::string name{ DEFAULT_FILE_NAME };
	while (true)
	{
		auto it{ std::ranges::find_if(m_roots, [&name](const auto& p)
			{
				return name == p->GetName();
			}) };

		if (it == m_roots.cend())
			break;

		name = std::format("{}{}", DEFAULT_FILE_NAME, index++);
	}

	auto root{ std::make_unique<Node>() };
	root->SetProperty(std::make_shared<Resource::Property>());
	root->SetName(name);
	root->SetFilePath(name);
	m_roots.emplace_back(root.release());
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

	// 노드 추가
	auto root{ std::make_unique<Node>() };
	root->SetProperty(prop);
	root->SetFilePath(filePath);
	m_roots.emplace_back(root.release());
}

void Hierarchy::OnMenuFileSave()
{
	Node* node{ nullptr };
	if (auto inspector{ Inspector::GetInstance() })
		node = inspector->GetNode();
	if (!node)
		return;
	while (node->GetParent())
		node = node->GetParent();
	assert(node->IsRoot() && "NODE MUST BE ROOT NODE");
	//node->Save();
}

void Hierarchy::OnMenuFileSaveAs()
{
}

bool Hierarchy::OnNodeSelect(Node* node)
{
	std::function<void(Node*)> lambda = [&](Node* n)
		{
			// Ctrl키를 누르고 있으면 누른 노드 추가 선택
			if (ImGui::GetIO().KeyCtrl)
			{
				if (n == node)
					n->SetSelect(true);
				return;
			}

			// 선택한 노드 외에 전부 선택 해제
			n->SetSelect(n == node ? true : false);

			// 재귀
			for (auto& child : *n)
				lambda(child.get());
		};

	for (auto& root : m_roots)
		for (auto& child : *root)
			lambda(child.get());

	return false;
}