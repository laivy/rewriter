#include "Stdafx.h"
#include "App.h"
#include "Clipboard.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

Hierarchy::Hierarchy()
{
	App::OnPropertyAdd.Register(this, std::bind_front(&Hierarchy::OnPropertyAdd, this));
	App::OnPropertySelected.Register(this, std::bind_front(&Hierarchy::OnPropertySelected, this));
	App::OnPropertyModified.Register(this, std::bind_front(&Hierarchy::OnPropertyModified, this));
}

void Hierarchy::Update(float deltaTime)
{
	// 유효하지 않은 프로퍼티 삭제
	for (const auto& invalid : m_invalids)
	{
		auto prop{ invalid.lock() };
		if (!prop)
			continue;

		if (m_roots.erase(prop) > 0)
			continue;

		for (const auto& root : m_roots | std::views::keys)
			Recurse(root, [&prop](const auto& p) { p->Delete(prop); });
	}
	m_invalids.clear();
}

void Hierarchy::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME, NULL, ImGuiWindowFlags_MenuBar))
	{
		Shortcut();
		DragDrop();
		RenderMenuBar();
		RenderTreeNode();
	}
	ImGui::End();
	ImGui::PopID();
}

void Hierarchy::OpenTree(const std::shared_ptr<Resource::Property>& prop)
{
	if (!IsOpened(prop))
		m_opens.emplace_back(prop);
}

bool Hierarchy::IsRoot(const std::shared_ptr<Resource::Property>& prop) const
{
	return m_roots.contains(prop);
}

void Hierarchy::OnPropertyAdd(const std::shared_ptr<Resource::Property>& prop)
{
	if (auto parent{ prop->GetParent() })
		OpenTree(parent);
}

void Hierarchy::OnPropertySelected(const std::shared_ptr<Resource::Property>& prop)
{
	// Ctrl키를 누르고 있으면 다른 노드들 선택 해제하지 않음
	if (!ImGui::IsKeyDown(ImGuiMod_Ctrl))
		m_selects.clear();
	m_selects.push_back(prop);
}

void Hierarchy::OnPropertyModified(const std::shared_ptr<Resource::Property>& prop)
{
	if (auto ancestor{ GetAncestor(prop) }; IsRoot(ancestor))
	{
		m_roots[ancestor].isModified = true;
	}
}

void Hierarchy::OnMenuFileNew()
{
	size_t index{ 0 };
	auto name{ std::format(L"{}{}", DEFAULT_FILE_NAME, Stringtable::DATA_FILE_EXT) };
	while (true)
	{
		auto it{ std::ranges::find_if(m_roots, [&name](const auto& root) { return root.first->GetName() == name; }) };
		if (it == m_roots.end())
			break;
		name = std::format(L"{}{}{}", DEFAULT_FILE_NAME, ++index, Stringtable::DATA_FILE_EXT);
	}

	auto root{ std::make_shared<Resource::Property>() };
	root->SetName(name);
	m_roots.emplace(root, Root{ .isModified = true });
}

void Hierarchy::OnMenuFileOpen()
{
	std::array<wchar_t, MAX_PATH> filePath{};

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = filePath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrDefExt = L"dat";
	if (!::GetOpenFileName(&ofn))
		return;

	// 경로 및 파일 이름들 추출
	std::filesystem::path path{ ofn.lpstrFile };
	std::vector<std::wstring> fileNames;
	auto ptr{ ofn.lpstrFile };
	ptr[ofn.nFileOffset - 1] = L'\0';
	ptr += ofn.nFileOffset;
	while (*ptr)
	{
		fileNames.emplace_back(ptr);
		ptr += ::lstrlen(ptr) + 1;
	}

	// 로드
	if (fileNames.size() == 1)
	{
		LoadDataFile(path);
	}
	else
	{
		for (const auto& file : fileNames)
			LoadDataFile(path / file);
	}
}

void Hierarchy::OnMenuFileSave()
{
	if (m_selects.empty())
		return;

	for (const auto& select : m_selects)
	{
		auto prop{ select.lock() };
		if (!prop)
			return;

		auto ancestor{ GetAncestor(prop) };
		if (!IsRoot(ancestor))
			return;

		auto& root{ m_roots.at(ancestor) };
		root.isModified = false;
		if (root.path.empty())
		{
			std::wstring filePath{ ancestor->GetName() };
			filePath.resize(MAX_PATH);

			::OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
			ofn.lpstrFile = filePath.data();
			ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT.data();
			ofn.nMaxFile = MAX_PATH;
			if (!::GetSaveFileName(&ofn))
				return;

			root.path = filePath;
			ImGui::GetIO().ClearInputKeys();
		}
		ancestor->SetName(root.path.filename().wstring());
		Resource::Save(ancestor, root.path.wstring());
	}
}

void Hierarchy::OnMenuFileSaveAs()
{
}

void Hierarchy::OnCut()
{
	std::vector<std::shared_ptr<Resource::Property>> targets;
	for (const auto& select : m_selects)
	{
		if (auto prop{ select.lock() })
		{
			targets.push_back(prop);
			Delete(prop);
		}
	}

	if (auto clipboard{ Clipboard::GetInstance() })
	{
		clipboard->Clear();
		clipboard->Copy(targets);
	}
}

void Hierarchy::OnCopy()
{
	std::vector<std::shared_ptr<Resource::Property>> targets;
	for (const auto& select : m_selects)
	{
		if (auto prop{ select.lock() })
			targets.push_back(prop);
	}

	if (auto clipboard{ Clipboard::GetInstance() })
	{
		clipboard->Clear();
		clipboard->Copy(targets);
	}
}

void Hierarchy::OnPaste()
{
	if (m_selects.size() != 1)
		return;

	auto select{ m_selects.front().lock() };
	if (!select)
		return;

	if (auto clipboard{ Clipboard::GetInstance() })
		clipboard->Paste(select);
}

void Hierarchy::Shortcut()
{
	if (!ImGui::IsWindowFocused())
		return;

	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_N))
		OnMenuFileNew();
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_O))
		OnMenuFileOpen();
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_S))
		OnMenuFileSave();
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_X))
		OnCut();
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_C))
		OnCopy();
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_V))
		OnPaste();
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyDown(ImGuiKey_UpArrow))
	{
		do
		{
			if (m_selects.size() != 1)
				break;

			auto selected{ m_selects.front().lock() };
			if (!selected)
				break;

			auto parent{ selected->GetParent() };
			if (!parent)
				break;

			auto& children{ parent->GetChildren() };
			auto it{ std::ranges::find(children, selected) };
			if (it == children.begin() || it == children.end())
				break;

			std::iter_swap(it, it - 1);
		} while (false);
	}
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyDown(ImGuiKey_DownArrow))
	{
		do
		{
			if (m_selects.size() != 1)
				break;

			auto selected{ m_selects.front().lock() };
			if (!selected)
				break;

			auto parent{ selected->GetParent() };
			if (!parent)
				break;

			auto& children{ parent->GetChildren() };
			auto it{ std::ranges::find(children, selected) };
			if (it == children.end() || it == children.end() - 1)
				break;

			std::iter_swap(it, it + 1);
		} while (false);
	}
	if (ImGui::IsKeyDown(ImGuiKey_F2))
	{
		auto window{ ImGui::FindWindowByName("Inspector") };
		ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		for (const auto& select : m_selects)
		{
			if (auto prop{ select.lock() })
				Delete(prop);
		}
	}
}

void Hierarchy::DragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (!ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		return;

	if (auto payload{ ImGui::AcceptDragDropPayload("EXPLORER/OPENFILE") })
	{
		std::wstring filePath{ static_cast<const wchar_t*>(payload->Data) };
		LoadDataFile(filePath);
	}

	ImGui::EndDragDropTarget();
}

void Hierarchy::RenderMenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu(MENU_FILE))
	{
		if (ImGui::MenuItem(MENU_FILE_NEW, "Ctrl+N") || (ImGui::IsKeyPressed(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_N)))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileNew();
		}
		if (ImGui::MenuItem(MENU_FILE_OPEN, "Ctrl+O") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileOpen();
		}
		if (ImGui::MenuItem(MENU_FILE_SAVE, "Ctrl+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSave();
		}
		if (ImGui::MenuItem(MENU_FILE_SAVEAS, "Ctrl+Shift+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSaveAs();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}

void Hierarchy::RenderTreeNode()
{
	ImGui::PushID("PROPERTY");
	for (const auto& root : m_roots | std::views::keys)
		RenderNode(root);
	ImGui::PopID();
}

void Hierarchy::RenderNode(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::PushID(prop.get());

	bool isRoot{ IsRoot(prop) };
	bool isSelected{ IsSelected(prop) };

	// 수정 사항 있으면 '*' 추가
	auto name{ Util::wstou8s(prop->GetName()) };
	if (isRoot && m_roots[prop].isModified)
		name.insert(0, "* ");

	// 루트가 아닌 말단 노드는 Selectable
	if (!isRoot && prop->GetChildren().empty())
	{
		if (ImGui::Selectable(name.c_str(), isSelected))
			App::OnPropertySelected.Notify(prop);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY/PROPERTY", &prop, sizeof(prop));
			ImGui::TextUnformatted(name.c_str());
			ImGui::EndDragDropSource();
		}

		RenderNodeContextMenu(prop);
		ImGui::PopID();
		return;
	}

	// 그 외는 TreeNode
	bool isOpened{ IsOpened(prop) };
	if (isOpened)
		ImGui::SetNextItemOpen(true);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, isRoot ? ImVec2{ 0.0f, 5.0f } : ImVec2{ 0.0f, 2.0f });
	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth };
	if (isSelected)
		flag |= ImGuiTreeNodeFlags_Selected;
	bool isTreeNodeOpen{ ImGui::TreeNodeEx(name.c_str(), flag) };
	ImGui::PopStyleVar();

	// 닫혀있어도 클릭 되도록
	if (ImGui::IsItemClicked())
		App::OnPropertySelected.Notify(prop);

	// 트리 여닫기
	if (ImGui::IsItemToggledOpen())
	{
		if (isOpened)
			std::erase_if(m_opens, [&prop](const auto& p) { return p.lock() == prop; });
		else
			OpenTree(prop);
	}

	// 드래그 드랍
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("HIERARCHY/PROPERTY", &prop, sizeof(prop));
		ImGui::TextUnformatted(name.c_str());
		ImGui::EndDragDropSource();
	}

	// 컨텍스트 메뉴
	RenderNodeContextMenu(prop);

	// 하위 트리
	if (isTreeNodeOpen)
	{
		for (const auto& [_, child] : *prop)
			RenderNode(child);
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void Hierarchy::RenderNodeContextMenu(const std::shared_ptr<Resource::Property>& prop)
{
	if (!ImGui::BeginPopupContextItem("CONTEXT"))
		return;

	App::OnPropertySelected.Notify(prop);

	if (ImGui::Selectable("Add (A)") || ImGui::IsKeyPressed(ImGuiKey_A))
	{
		ImGui::CloseCurrentPopup();

		size_t index{ 0 };
		std::wstring name{ DEFAULT_NODE_NAME };
		while (true)
		{
			if (auto child{ prop->Get(name) })
			{
				name = std::format(L"{}{}", DEFAULT_NODE_NAME, ++index);
				continue;
			}
			break;
		}

		auto child{ std::make_shared<Resource::Property>() };
		child->SetName(name);
		child->SetParent(prop);
		prop->Add(child);
		App::OnPropertyAdd.Notify(child);
		App::OnPropertyModified.Notify(prop);
	}

	if (ImGui::Selectable("Delete (D)") || ImGui::IsKeyPressed(ImGuiKey_D))
	{
		ImGui::CloseCurrentPopup();
		for (const auto& select : m_selects)
		{
			if (auto prop{ select.lock() })
				Delete(prop);
		}
	}

	ImGui::EndPopup();
}

void Hierarchy::LoadDataFile(const std::filesystem::path& path)
{
	auto root{ Resource::Get(path.wstring()) };
	root->SetName(path.filename().wstring());
	m_roots.emplace(root, path);
}

void Hierarchy::Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func)
{
	for (const auto& [_, child] : *prop)
		Recurse(child, func);
	func(prop);
}

void Hierarchy::Delete(const std::shared_ptr<Resource::Property>& prop)
{
	m_invalids.emplace_back(prop);
	App::OnPropertyDelete.Notify(prop);
	if (auto parent{ prop->GetParent() })
		App::OnPropertyModified.Notify(parent);
}

std::shared_ptr<Resource::Property> Hierarchy::GetAncestor(const std::shared_ptr<Resource::Property>& prop)
{
	auto ancestor{ prop };
	auto parent{ prop };
	while (parent = parent->GetParent())
		ancestor = parent;
	return ancestor;
}

bool Hierarchy::IsSelected(const std::shared_ptr<Resource::Property>& prop) const
{
	auto it{ std::ranges::find_if(m_selects, [&prop](const auto& p) { return p.lock() == prop; }) };
	return it != m_selects.end();
}

bool Hierarchy::IsOpened(const std::shared_ptr<Resource::Property>& prop) const
{
	auto it{ std::ranges::find_if(m_opens, [&prop](const auto& p) { return p.lock() == prop; }) };
	return it != m_opens.end();
}
