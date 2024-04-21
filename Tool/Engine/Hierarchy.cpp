#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "PropInfo.h"
#include "Util.h"

Hierarchy::Hierarchy()
{
	m_onProprtySelect = Global::OnPropertySelect.Add(std::bind_front(&Hierarchy::OnPropertySelect, this));
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Update(float deltaTime)
{
	// 유효하지 않은 프로퍼티 삭제
	std::function<void(const std::shared_ptr<Resource::Property>&)> func =
		[&](const std::shared_ptr<Resource::Property>& prop)
		{
			std::erase_if(prop->children, [](const auto& prop) { return !Global::propInfo[prop].isValid; });
			for (const auto& child : prop->children)
				func(child);
		};

	std::erase_if(Global::properties, [](const auto& prop) { return !Global::propInfo[prop].isValid; });
	for (const auto& prop : Global::properties)
		func(prop);
}

void Hierarchy::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME, NULL, ImGuiWindowFlags_MenuBar))
	{
		DragDrop();
		RenderMenu();
		RenderNode();
	}
	ImGui::End();
	ImGui::PopID();
}

void Hierarchy::RenderMenu()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu(MENU_FILE))
	{
		if (ImGui::MenuItem(MENU_FILE_NEW, "ctrl+n") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileNew();
		}
		if (ImGui::MenuItem(MENU_FILE_OPEN, "ctrl+o") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileOpen();
		}
		if (ImGui::MenuItem(MENU_FILE_SAVE, "ctrl+s") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSave();
		}
		if (ImGui::MenuItem(MENU_FILE_SAVEAS, "ctrl+shift+s") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S))
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
	auto menu = [](std::shared_ptr<Resource::Property> prop)
		{
			if (!ImGui::BeginPopupContextItem("CONTEXT"))
				return;

			if (Global::propInfo[prop].isRoot)
			{
				if (ImGui::Selectable("Save"))
				{
					prop->Save(Global::propInfo[prop].path);
				}
				if (ImGui::Selectable("Save As"))
				{
					std::wstring path(MAX_PATH, L'\0');
					OPENFILENAME ofn{};
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
					ofn.lpstrFile = path.data();
					ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT;
					ofn.nMaxFile = MAX_PATH;
					if (::GetSaveFileName(&ofn))
					{
						prop->Save(path);
						Global::propInfo[prop].path = path;
						prop->SetName(Global::propInfo[prop].path.filename());
					}
				}
			}
			if (ImGui::Selectable("Add"))
			{
				size_t index{ 1 };
				std::wstring name{ DEFAULT_NODE_NAME };
				while (true)
				{
					auto it = std::ranges::find_if(prop->children, [name](const auto& child) { return name == child->name; });
					if (it == prop->children.end())
						break;
					name = std::format(L"{}{}", DEFAULT_NODE_NAME, index);
					++index;
				}

				auto child{ std::make_shared<Resource::Property>() };
				child->SetName(name);
				prop->Add(child);
				Global::propInfo[child].parent = prop;
				Global::OnPropertyAdd.Notify(child);
			}
			if (ImGui::Selectable("Del"))
			{
				Global::propInfo[prop].isValid = false;
				Global::OnPropertyDelete.Notify(prop);
			}
			ImGui::EndPopup();
		};

	std::function<void(const std::shared_ptr<Resource::Property>&)> render = [&](const std::shared_ptr<Resource::Property>& prop)
		{
			ImGui::PushID(prop.get());
			if (!Global::propInfo[prop].isRoot && prop->children.empty())
			{
				if (ImGui::Selectable(Util::wstou8s(prop->GetName()).c_str(), Global::propInfo[prop].isSelected))
					Global::OnPropertySelect.Notify(prop);
				menu(prop);
				ImGui::PopID();
				return;
			}

			ImGuiTreeNodeFlags flag{ 
				ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth
			};
			if (Global::propInfo[prop].isRoot)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 5.0f });
				flag |= ImGuiTreeNodeFlags_FramePadding;
			}
			if (Global::propInfo[prop].isSelected)
				flag |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(Util::wstou8s(prop->GetName()).c_str(), flag))
			{
				if (ImGui::IsItemClicked())
					Global::OnPropertySelect.Notify(prop);

				menu(prop);

				for (const auto& [_, child] : *prop)
					render(child);

				ImGui::TreePop();
			}
			if (Global::propInfo[prop].isRoot)
				ImGui::PopStyleVar();

			ImGui::PopID();
		};

	ImGui::PushID("PROPERTY");
	for (const auto& prop : Global::properties)
		render(prop);
	ImGui::PopID();
}

void Hierarchy::Load(const std::filesystem::path& path)
{
	auto prop{ Resource::Load(path) };
	prop->SetName(std::filesystem::path{ path }.filename());
	Global::properties.push_back(prop);
	Global::propInfo[prop].path = path;
	Global::propInfo[prop].isRoot = true;
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

void Hierarchy::OnFileDragDrop(std::string_view path)
{
	Load(path);
}

void Hierarchy::OnMenuFileNew()
{
	size_t index{ 1 };
	std::wstring name{ DEFAULT_FILE_NAME };
	while (true)
	{
		auto it{ std::ranges::find_if(Global::properties, [&name](const auto& node)
			{
				return name == node->name;
			}) };

		if (it == Global::properties.cend())
			break;

		name = std::format(L"{}{}", DEFAULT_FILE_NAME, index++);
	}

	auto prop{ std::make_shared<Resource::Property>() };
	prop->SetName(name);
	Global::properties.push_back(prop);
	Global::propInfo[prop].isRoot = true;
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
	for (const auto& file : fileNames)
		Load(path / file);
}

void Hierarchy::OnMenuFileSave()
{
	auto inspector{ Inspector::GetInstance() };
	if (!inspector)
		return;

	auto prop{ inspector->GetNode().lock() };
	if (!prop)
		return;

	if (!Global::propInfo[prop].isRoot)
		return;

	if (Global::propInfo[prop].path.empty())
	{
		std::wstring path(MAX_PATH, L'\0');
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
		ofn.lpstrFile = path.data();
		ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT;
		ofn.nMaxFile = MAX_PATH;
		if (::GetSaveFileName(&ofn))
			Global::propInfo[prop].path = path;
	}

	prop->SetName(Global::propInfo[prop].path.filename());
	prop->Save(Global::propInfo[prop].path);
}

void Hierarchy::OnMenuFileSaveAs()
{
}

void Hierarchy::OnPropertySelect(std::shared_ptr<Resource::Property> prop)
{
	std::function<void(const std::shared_ptr<Resource::Property>&)> lambda = 
		[&](const std::shared_ptr<Resource::Property>& p)
		{
			// 선택한 노드 외에 전부 선택 해제
			Global::propInfo[p].isSelected = (p == prop);

			// 재귀
			for (const auto& child : p->children)
				lambda(child);
		};

	// 노드 선택
	// Ctrl키를 누르고 있으면 다른 노드들 선택 해제하지 않음
	Global::propInfo[prop].isSelected = true;
	if (ImGui::GetIO().KeyCtrl)
		return;

	for (const auto& p : Global::properties)
		lambda(p);
}