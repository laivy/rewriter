#include "Stdafx.h"
#include "App.h"
#include "Clipboard.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

Hierarchy::Hierarchy()
{
	App::OnPropertySelect.Register(this, std::bind_front(&Hierarchy::OnPropertySelect, this));
}

void Hierarchy::Update(float deltaTime)
{
	// 유효하지 않은 프로퍼티 삭제
	for (const auto& invalid : m_invalids)
	{
		if (m_roots.erase(invalid) > 0)
			continue;

		for (const auto& [root, _] : m_roots)
			Recurse(root, [&invalid](const auto& prop) { prop->Delete(invalid); });
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

void Hierarchy::OnPropertySelect(std::shared_ptr<Resource::Property> prop)
{
	// Ctrl키를 누르고 있으면 다른 노드들 선택 해제하지 않음
	if (!ImGui::IsKeyDown(ImGuiMod_Ctrl))
		m_selects.clear();
	m_selects.push_back(prop);
}

void Hierarchy::OnFileDragDrop(std::string_view path)
{
	LoadDataFile(path);
}

void Hierarchy::OnMenuFileNew()
{
	size_t index{ 0 };
	std::wstring name{ DEFAULT_FILE_NAME };
	while (true)
	{
		auto it{ std::ranges::find_if(m_roots, [&name](const auto& root) { return root.first->GetName() == name; }) };
		if (it == m_roots.end())
			break;
		name = std::format(L"{}{}", DEFAULT_FILE_NAME, ++index);
	}

	auto root{ std::make_shared<Resource::Property>() };
	root->SetName(name);
	m_roots.emplace(root, L"");
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

	auto prop{ m_selects.back().lock() };
	if (!prop)
		return;

	auto root{ GetAncestor(prop) };
	if (!m_roots.contains(root))
		return;

	auto& path{ m_roots[root] };
	if (path.empty())
	{
		std::wstring filePath{ root->GetName() };
		filePath.resize(MAX_PATH);

		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
		ofn.lpstrFile = filePath.data();
		ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT;
		ofn.nMaxFile = MAX_PATH;
		if (!::GetSaveFileName(&ofn))
			return;
		path = filePath;
	}
	root->SetName(path.filename().wstring());
	Resource::Save(root, path.wstring());
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

	auto clipboard{ Clipboard::GetInstance() };
	clipboard->Clear();
	clipboard->Copy(targets);
}

void Hierarchy::OnCopy()
{
	std::vector<std::shared_ptr<Resource::Property>> targets;
	for (const auto& select : m_selects)
	{
		if (auto prop{ select.lock() })
			targets.push_back(prop);
	}

	auto clipboard{ Clipboard::GetInstance() };
	clipboard->Clear();
	clipboard->Copy(targets);
}

void Hierarchy::OnPaste()
{
	if (m_selects.size() != 1)
		return;

	auto select{ m_selects.front().lock() };
	if (!select)
		return;

	auto clipboard{ Clipboard::GetInstance() };
	clipboard->Paste(select);
}

void Hierarchy::Shortcut()
{
	if (!ImGui::IsWindowFocused())
		return;

	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N))
		OnMenuFileNew();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		OnMenuFileOpen();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		OnMenuFileSave();
	if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_UpArrow))
	{
		do
		{
			if (m_selects.size() != 1)
				break;

			auto selected{ m_selects.front().lock() };
			if (!selected)
				break;

			auto parent{ GetParent(selected) };
			if (!parent)
				break;

			auto& children{ parent->GetChildren() };
			auto it{ std::ranges::find(children, selected) };
			if (it == children.begin() || it == children.end())
				break;

			std::iter_swap(it, it - 1);
		} while (false);
	}
	if (ImGui::Shortcut(ImGuiModFlags_Alt | ImGuiKey_DownArrow))
	{
		do
		{
			if (m_selects.size() != 1)
				break;

			auto selected{ m_selects.front().lock() };
			if (!selected)
				break;

			auto parent{ GetParent(selected) };
			if (!parent)
				break;

			auto& children{ parent->GetChildren() };
			auto it{ std::ranges::find(children, selected) };
			if (it == children.end() || it == children.end() - 1)
				break;

			std::iter_swap(it, it + 1);
		} while (false);
	}
	if (ImGui::Shortcut(ImGuiModFlags_Ctrl | ImGuiKey_X))
	{
		OnCut();
	}
	if (ImGui::Shortcut(ImGuiModFlags_Ctrl | ImGuiKey_C))
	{
		OnCopy();
	}
	if (ImGui::Shortcut(ImGuiModFlags_Ctrl | ImGuiKey_V))
	{
		OnPaste();
	}
	if (ImGui::IsKeyPressed(ImGuiKey_F2, false))
	{
		auto window{ ImGui::FindWindowByName("Inspector") };
		ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}
	if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
	{
		for (const auto& select : m_selects)
		{
			if (auto prop{ select.lock() })
			{
				App::OnPropertyDelete.Notify(prop);
				Delete(prop);
			}
		}
	}
}

void Hierarchy::DragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (!ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		return;

	if (auto payload{ ImGui::AcceptDragDropPayload("OPENFILE") })
	{
		std::string filePath{ static_cast<const char*>(payload->Data) };
		OnFileDragDrop(filePath);
	}

	ImGui::EndDragDropTarget();
}

void Hierarchy::RenderMenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu(MENU_FILE))
	{
		if (ImGui::MenuItem(MENU_FILE_NEW, "Ctrl+N") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N))
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
	for (const auto& [root, _] : m_roots)
		RenderNode(root);
	ImGui::PopID();
}

void Hierarchy::RenderNode(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::PushID(prop.get());

	bool isRoot{ m_roots.contains(prop) };
	auto selected{ std::ranges::find_if(m_selects, [&prop](const auto& select) { return select.lock() == prop; }) != m_selects.end() };

	if (!isRoot && prop->GetChildren().empty())
	{
		// 루트가 아닌 말단 노드는 Selectable
		auto label{ Util::wstou8s(prop->GetName()) };
		if (ImGui::Selectable(label.c_str(), selected))
			App::OnPropertySelect.Notify(prop);
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("PROPERTY", &prop, sizeof(prop));
			ImGui::Text(label.c_str());
			ImGui::EndDragDropSource();
		}
		RenderNodeContextMenu(prop);
	}
	else
	{
		// 나머지 경우는 TreeNode
		ImVec2 padding{};
		if (isRoot)
			padding = { 0.0f, 5.0f };
		else
			padding = { 0.0f, 2.0f };
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

		ImGuiTreeNodeFlags flag{
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth
		};

		if (selected)
			flag |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(Util::wstou8s(prop->GetName()).c_str(), flag))
		{
			if (ImGui::IsItemClicked())
			{
				if (isRoot)
					OnPropertySelect(prop);
				else
					App::OnPropertySelect.Notify(prop);
			}
			RenderNodeContextMenu(prop);
			for (const auto& [_, child] : *prop)
				RenderNode(child);
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	ImGui::PopID();
}

void Hierarchy::RenderNodeContextMenu(const std::shared_ptr<Resource::Property>& prop)
{
	if (!ImGui::BeginPopupContextItem("CONTEXT"))
		return;

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
		prop->Add(child);
		App::OnPropertyAdd.Notify(child);
	}

	if (ImGui::Selectable("Delete (D)") || ImGui::IsKeyPressed(ImGuiKey_D))
	{
		ImGui::CloseCurrentPopup();
		for (const auto& select : m_selects)
		{
			if (auto prop{ select.lock() })
			{
				App::OnPropertyDelete.Notify(prop);
				Delete(prop);
			}
		}
	}

	ImGui::EndPopup();
}

void Hierarchy::LoadDataFile(const std::filesystem::path& path)
{
}

void Hierarchy::Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func)
{
	for (const auto& [_, child] : *prop)
		Recurse(child, func);
	func(prop);
}

void Hierarchy::Delete(const std::shared_ptr<Resource::Property>& prop)
{
	m_invalids.insert(prop);
}

std::shared_ptr<Resource::Property> Hierarchy::GetParent(const std::shared_ptr<Resource::Property>& prop)
{
	std::shared_ptr<Resource::Property> parent;
	auto name{ prop->GetName() };
	for (const auto& [root, _] : m_roots)
	{
		Recurse(root,
			[&parent, &name](const auto& p)
			{
				if (auto child{ p->Get(name) })
					parent = p;
			});
	}
	return parent;
}

std::shared_ptr<Resource::Property> Hierarchy::GetAncestor(const std::shared_ptr<Resource::Property>& prop)
{
	auto ancestor{ prop };
	auto parent{ prop };
	while (parent = GetParent(parent))
		ancestor = parent;
	return ancestor;
}
