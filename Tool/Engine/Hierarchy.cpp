#include "Stdafx.h"
#include "App.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

Hierarchy::Hierarchy()
{
	App::OnPropertySelect.Register(this, std::bind_front(&Hierarchy::OnPropertySelect, this));
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Update(float deltaTime)
{
	// 유효하지 않은 프로퍼티 삭제
}

void Hierarchy::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME, NULL, ImGuiWindowFlags_MenuBar))
	{
		Shortcut();
		DragDrop();
		RenderMenu();
		RenderNode();
	}
	ImGui::End();
	ImGui::PopID();
}

void Hierarchy::OnPropertySelect(std::shared_ptr<Resource::Property> prop)
{
	// Ctrl키를 누르고 있으면 다른 노드들 선택 해제하지 않음
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl))
	{
		g_selectedPropertise.emplace_back(prop);
		return;
	}

	g_selectedPropertise.clear();
	g_selectedPropertise.emplace_back(prop);
}

void Hierarchy::OnFileDragDrop(std::string_view path)
{
	LoadDataFile(path);
}

void Hierarchy::OnMenuFileNew()
{
	size_t index{ 1 };
	std::wstring name{ DEFAULT_FILE_NAME };
	while (true)
	{
		auto it{ std::ranges::find_if(g_roots, [&name](const auto& root) { return root.prop->GetName() == name; }) };
		if (it == g_roots.end())
			break;
		name = std::format(L"{}{}", DEFAULT_FILE_NAME, index++);
	}

	Root root{};
	root.prop = std::make_shared<Resource::Property>();
	root.prop->SetName(name + Stringtable::DATA_FILE_EXT);
	g_roots.push_back(root);
}

void Hierarchy::OnMenuFileOpen()
{
	std::wstring filepath(MAX_PATH, L'\0');

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = filepath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrDefExt = L"dat";
	if (!::GetOpenFileName(&ofn))
		return;

	// 경로 및 파일 이름들 추출
	std::filesystem::path path{ ofn.lpstrFile };
	std::vector<std::wstring> fileNames;
	auto ptr{ ofn.lpstrFile };
	ptr[ofn.nFileOffset - 1] = 0;
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
	if (g_selectedPropertise.empty())
		return;

	auto prop{ g_selectedPropertise.back().lock() };
	if (!prop)
		return;

	auto it{ std::ranges::find_if(g_roots, [&prop](const auto& root) { return root.prop == prop; }) };
	if (it == g_roots.end())
		return;
	
	Root& root{ *it };
	if (root.path.empty())
	{
		std::wstring path{ prop->GetName() };
		path.resize(MAX_PATH);

		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
		ofn.lpstrFile = path.data();
		ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT;
		ofn.nMaxFile = MAX_PATH;
		if (!::GetSaveFileName(&ofn))
			return;
		root.path = path;
	}
	prop->SetName(root.path.filename());
	prop->Save(root.path);
}

void Hierarchy::OnMenuFileSaveAs()
{
}

void Hierarchy::LoadDataFile(const std::filesystem::path& path)
{
	Root root{};
	root.prop = Resource::Get(path.wstring());
	root.prop->SetName(path.filename());
	root.path = path;
	g_roots.push_back(root);
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
			if (g_selectedPropertise.size() != 1)
				break;

			auto selected{ g_selectedPropertise.front().lock() };
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
			if (g_selectedPropertise.size() != 1)
				break;

			auto selected{ g_selectedPropertise.front().lock() };
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
	if (ImGui::IsKeyPressed(ImGuiKey_F2, false))
	{
		auto window{ ImGui::FindWindowByName("Inspector") };
		ImGui::ActivateItemByID(window->GetID("##NAME"));
	}
	if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
	{
		for (const auto& selected : g_selectedPropertise)
		{
			auto prop{ selected.lock() };
			if (!prop)
				continue;

			App::OnPropertyDelete.Notify(prop);

			for (const auto& root : g_roots)
				ForEachProperty(root.prop, [&prop](const auto& p) { std::erase(p->GetChildren(), prop); });
		}
	}
}

void Hierarchy::DragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (!ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		return;

	if (auto payload{ ImGui::AcceptDragDropPayload("DRAGDROP") })
	{
		std::string filePath{ static_cast<const char*>(payload->Data) };
		OnFileDragDrop(filePath);
	}

	ImGui::EndDragDropTarget();
}

void Hierarchy::RenderMenu()
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

void Hierarchy::RenderNode()
{
	auto menu = [this](std::shared_ptr<Resource::Property> prop)
		{
			if (!ImGui::BeginPopupContextItem("CONTEXT"))
				return;

			if (ImGui::Selectable("Add(A)") || ImGui::IsKeyPressed(ImGuiKey_A))
			{
				ImGui::CloseCurrentPopup();

				size_t index{ 1 };
				std::wstring name{ DEFAULT_NODE_NAME };
				while (true)
				{
					const auto& children{ prop->GetChildren() };
					auto it = std::ranges::find_if(children, [name](const auto& prop) { return prop->GetName() == name; });
					if (it == children.end())
						break;
					name = std::format(L"{}{}", DEFAULT_NODE_NAME, index);
					++index;
				}

				auto child{ std::make_shared<Resource::Property>() };
				child->SetName(name);
				prop->Add(child);
				App::OnPropertyAdd.Notify(child);
			}
			if (ImGui::Selectable("Del(D)") || ImGui::IsKeyPressed(ImGuiKey_D))
			{
				ImGui::CloseCurrentPopup();

				App::OnPropertyDelete.Notify(prop);

				if (auto parent = GetParent(prop))
					parent->Delete(prop);
			}
			ImGui::EndPopup();
		};

	std::function<void(const std::shared_ptr<Resource::Property>&)> render = [&](const std::shared_ptr<Resource::Property>& prop)
		{
			ImGui::PushID(prop.get());
			if (!IsRoot(prop) && prop->GetChildren().empty())
			{
				if (ImGui::Selectable(Util::wstou8s(prop->GetName()).c_str(), IsSelected(prop)))
					App::OnPropertySelect.Notify(prop);
				menu(prop);
				ImGui::PopID();
				return;
			}

			ImGuiTreeNodeFlags flag{
				ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding
			};

			ImVec2 padding{};
			if (IsRoot(prop))
				padding = { 0.0f, 5.0f };
			else
				padding = { 0.0f, 2.0f };
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

			if (IsSelected(prop))
				flag |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(Util::wstou8s(prop->GetName()).c_str(), flag))
			{
				if (ImGui::IsItemClicked())
					App::OnPropertySelect.Notify(prop);

				menu(prop);

				for (const auto& child : prop->GetChildren())
					render(child);

				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
			ImGui::PopID();
		};

	ImGui::PushID("PROPERTY");
	for (const auto& root : g_roots)
		render(root.prop);
	ImGui::PopID();
}
