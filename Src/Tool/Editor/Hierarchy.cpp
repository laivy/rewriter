#include "Stdafx.h"
#include "Delegates.h"
#include "Hierarchy.h"
#include "Common/Util.h"

namespace
{
	constexpr auto WindowName{ "Hierarchy" };
	constexpr auto MenuFile{ "File" };
	constexpr auto MenuFileNew{ "New" };
	constexpr auto MenuFileOpen{ "Open" };
	constexpr auto MenuFileSave{ "Save" };
	constexpr auto MenuFileSaveAs{ "Save As" };
	constexpr auto DefaultFileName{ L"File" };
	constexpr auto DefaultPropertyName{ L"Property" };
}

Hierarchy::Hierarchy() :
	m_isAnyPropertySelected{ false }
{
	Delegates::OnPropertyAdded.Bind(this, std::bind_front(&Hierarchy::OnPropertyAdded, this));
	Delegates::OnPropertyDeleted.Bind(this, std::bind_front(&Hierarchy::OnPropertyDeleted, this));
	Delegates::OnPropertyModified.Bind(this, std::bind_front(&Hierarchy::OnPropertyModified, this));
	Delegates::OnPropertySelected.Bind(this, std::bind_front(&Hierarchy::OnPropertySelected, this));
}

void Hierarchy::Update(float deltaTime)
{
	std::erase_if(m_contexts, [this](const auto& elem)
	{
		if (!elem.second.isInvalid)
			return false;

		const Resource::ID id{ elem.first };
		std::erase_if(m_roots, [id](const auto& root) { return root.id == id; });
		Resource::Delete(id);
		return true;
	});
	m_isAnyPropertySelected = false;
}

void Hierarchy::Render()
{
	if (ImGui::Begin(WindowName, nullptr, ImGuiWindowFlags_MenuBar))
	{
		Shortcut();
		DragDrop();
		MenuBar();
		TreeView();
	}
	ImGui::End();
	RenderModal();
}

void Hierarchy::OpenTree(Resource::ID id)
{
	m_contexts[id].isOpened = true;
}

void Hierarchy::CloseTree(Resource::ID id)
{
	m_contexts[id].isOpened = false;
}

bool Hierarchy::IsRoot(Resource::ID id) const
{
	return std::ranges::contains(m_roots, id, &Root::id);
}

void Hierarchy::OnPropertyAdded(Resource::ID id)
{
	const auto parentID{ Resource::GetParent(id) };
	if (parentID == Resource::InvalidID)
		return;

	OpenTree(parentID);
	SetModified(id, true);
}

void Hierarchy::OnPropertyDeleted(Resource::ID id)
{
	SetModified(id, true);
}

void Hierarchy::OnPropertyModified(Resource::ID id)
{
	SetModified(id, true);
}

void Hierarchy::OnPropertySelected(Resource::ID id)
{
	m_isAnyPropertySelected = true;

	// 이미 선택된 노드를 선택한 경우 선택 해제 안함
	// 그리고 이미 선택된 노드이기 때문에 컨테이너에 추가 안함
	if (IsSelected(id))
		return;

	// Ctrl키를 누르고 있을 때는 선택 해제 안함
	if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		for (auto& [id, context] : m_contexts)
			context.isSelected = false;
	}
	m_contexts[id].isSelected = true;
}

void Hierarchy::OnMenuFileNew()
{
	const Resource::ID id{ New(Resource::InvalidID) };
	m_roots.emplace_back(id, Resource::GetName(id));
	m_contexts[id] = { .isModified = true };
}

void Hierarchy::OnMenuFileOpen()
{
	// TODO
	//Graphics::ImGui::OpenFileDialog("Open File");

	/*
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
	*/
}

void Hierarchy::OnMenuFileSave()
{
	for (const auto& [id, _] : m_contexts)
	{
		if (!IsSelected(id))
			continue;

		const auto root{ GetRoot(id) };
		if (!IsModified(root.id))
			continue;

		SetModified(root.id, false);
		Resource::SaveToFile(root.id, root.path);
	}
}

void Hierarchy::OnMenuFileSaveAs()
{
}

void Hierarchy::OnCut()
{
	assert(false && "Not Implemented");
}

void Hierarchy::OnCopy()
{
	assert(false && "Not Implemented");
}

void Hierarchy::OnPaste()
{
	assert(false && "Not Implemented");
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
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
	{
		do
		{
			/*
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
			*/
		} while (false);
	}
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyPressed(ImGuiKey_DownArrow, false))
	{
		do
		{
			/*
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
			*/
		} while (false);
	}
	if (ImGui::IsKeyDown(ImGuiKey_F2))
	{
		if (auto window{ ImGui::FindWindowByName("Inspector") })
			ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		for (const auto& [id, _] : m_contexts)
		{
			if (IsSelected(id))
				Delete(id);
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
		std::filesystem::path filePath{ static_cast<const wchar_t*>(payload->Data) };
		if (filePath.extension() == Stringtable::DATA_FILE_EXT)
			LoadDataFile(filePath);
	}

	ImGui::EndDragDropTarget();
}

void Hierarchy::MenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu(MenuFile))
	{
		if (ImGui::MenuItem(MenuFileNew, "Ctrl+N") || (ImGui::IsKeyPressed(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_N)))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileNew();
		}
		if (ImGui::MenuItem(MenuFileOpen, "Ctrl+O") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileOpen();
		}
		if (ImGui::MenuItem(MenuFileSave, "Ctrl+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSave();
		}
		if (ImGui::MenuItem(MenuFileSaveAs, "Ctrl+Shift+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSaveAs();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}

void Hierarchy::TreeView()
{
	std::vector<std::tuple<Resource::ID, Resource::ID, std::size_t>> moveList;
	auto render = [this, &moveList](this auto self, Resource::ID id, std::size_t index) -> void
	{
		ImGui::PushID(std::to_string(id).c_str());

		const bool isRoot{ IsRoot(id) };
		std::string name{ Util::ToU8String(Resource::GetName(id)) };
		if (isRoot)
		{
			// 루트 노드는 폰트 키움
			ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 1.1f);

			// 변경 사항이 있으면 루트 노드 이름 앞에 '*' 추가
			if (IsModified(id))
				name.insert(0, "* ");
		}

		const bool isOpened{ IsOpened(id) };
		ImGui::SetNextItemOpen(isOpened);

		ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DrawLinesToNodes };
		if (IsSelected(id))
			flag |= ImGuiTreeNodeFlags_Selected;
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0.0f);
		const bool isTreeNodeOpened{ ImGui::TreeNodeEx(name.c_str(), flag) };
		ImGui::PopStyleVar();
		//ImGui::DebugDrawItemRect();

		// 폰트 크기 원복
		if (isRoot)
			ImGui::PopFont();

		// 닫혀있어도 클릭 되도록
		if (ImGui::IsItemClicked())
			Delegates::OnPropertySelected.Broadcast(id);

		// 트리 여닫기
		if (ImGui::IsItemToggledOpen())
		{
			if (IsOpened(id))
				CloseTree(id);
			else
				OpenTree(id);
		}

		// 드래그 드랍
		if (!isRoot && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Hierarchy/Property", &id, sizeof(id));
			ImGui::TextUnformatted(name.c_str());
			ImGui::EndDragDropSource();
		}

		// 컨텍스트 메뉴
		ContextMenu(id);

		// 순서 변경을 위한 중간 영역
		const ImVec2 cursorPos{ ImGui::GetCursorPos() };
		const ImVec2 lt{ ImGui::GetItemRectMin() };
		const ImVec2 rb{ ImGui::GetItemRectMax() };
		const float y{ ImGui::GetStyle().ItemSpacing.y };
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0.0f);
		ImGui::InvisibleButton("Between", ImVec2{ rb.x - lt.x - cursorPos.x + ImGui::GetStyle().ItemSpacing.x, y });
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			constexpr float Thickness{ 1.0f };

			ImRect rect{};
			rect.Min = ImGui::GetItemRectMin();
			rect.Max = ImGui::GetItemRectMax();

			const ImVec2 center{ rect.GetCenter() };
			rect.Min.y = center.y - Thickness / 2.0f;
			rect.Max.y = center.y + Thickness / 2.0f;

			ImDrawList* drawList{ ImGui::GetWindowDrawList() };
			drawList->AddRectFilled(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget));

			if (auto payload{ ImGui::AcceptDragDropPayload("Hierarchy/Property", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) })
			{
				const Resource::ID targetID{ *static_cast<Resource::ID*>(payload->Data) };
				moveList.emplace_back(
					targetID,
					isOpened ? id : Resource::GetParent(id),
					isOpened ? 0 : index + 1
				);
			}
			ImGui::EndDragDropTarget();
		}

		// 하위 트리
		if (isTreeNodeOpened)
		{
			for (const auto& [index, child] : Resource::Iterator{ id } | std::views::values | std::views::enumerate)
				self(child, index);
			ImGui::TreePop();
		}
		ImGui::PopID();
	};

	ImGui::PushID("TreeView");
	for (const auto& root : m_roots)
		render(root.id, 0);
	ImGui::PopID();

	// 프로퍼티 이동
	for (const auto& [targetID, parentID, index] : moveList)
		Resource::Move(targetID, parentID, index);

	// 클릭을 했는데 선택된 프로퍼티가 없으면 이미 선택되어 있는 프로퍼티 선택 해제
	if (!m_isAnyPropertySelected && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		for (auto& [_, context] : m_contexts)
			context.isSelected = false;
	}
}

void Hierarchy::ContextMenu(Resource::ID id)
{
	if (!ImGui::BeginPopupContextItem("ContextMenu"))
		return;

	if (!IsSelected(id))
		Delegates::OnPropertySelected.Broadcast(id);

	// 루트 노드 전용 메뉴
	do
	{
		auto selected{ m_contexts | std::views::filter([](const auto& elem) { return elem.second.isSelected; }) };
		if (std::ranges::all_of(selected, [this](const auto& elem) { return !IsRoot(elem.first); }))
			break;

		if (ImGui::MenuItem("Save", "S") || ImGui::IsKeyPressed(ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSave();
		}
		if (ImGui::MenuItem("Save As"))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSaveAs();
		}
		if (ImGui::MenuItem("Close", "C") || ImGui::IsKeyPressed(ImGuiKey_C))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::Separator();
	} while (false);

	// 공통 메뉴
	if (ImGui::MenuItem("Rename", "F2") || ImGui::IsKeyPressed(ImGuiKey_F2))
	{
		ImGui::CloseCurrentPopup();

		if (auto window{ ImGui::FindWindowByName("Inspector") })
			ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}
	if (ImGui::MenuItem("New", "N") || ImGui::IsKeyPressed(ImGuiKey_N))
	{
		ImGui::CloseCurrentPopup();
		New(id);
	}
	if (ImGui::MenuItem("Delete", "D") || ImGui::IsKeyPressed(ImGuiKey_D))
	{
		ImGui::CloseCurrentPopup();
		for (const auto id : m_contexts | std::views::keys)
		{
			if (IsSelected(id))
				Delete(id);
		}
	}
	ImGui::EndPopup();
}

void Hierarchy::RenderModal()
{
}

void Hierarchy::LoadDataFile(const std::filesystem::path& path)
{
	const auto rootID{ Resource::Get(path.wstring()) };
	if (rootID == Resource::InvalidID)
	{
		assert(false && "failed to load data file");
		return;
	}
	Resource::SetName(rootID, path.filename().wstring());
	m_roots.emplace_back(rootID, path);
}

Resource::ID Hierarchy::New(Resource::ID parentID)
{
	Resource::ID id{ Resource::InvalidID };
	// 새로운 파일
	if (parentID == Resource::InvalidID)
	{
		std::wstring name{ std::format(L"{}{}", DefaultFileName, Stringtable::DATA_FILE_EXT) };
		std::size_t index{ 0 };
		while (true)
		{
			if (Resource::Get(name) == Resource::InvalidID)
			{
				id = Resource::New(name);
				break;
			}
			name = std::format(L"{}{}{}", DefaultFileName, ++index, Stringtable::DATA_FILE_EXT);
		}
	}
	// 새로운 프로퍼티
	else
	{
		std::wstring name{ DefaultPropertyName };
		std::size_t index{ 0 };
		while (true)
		{
			if (Resource::Get(parentID, name) == Resource::InvalidID)
			{
				id = Resource::New(parentID, name);
				break;
			}
			name = std::format(L"{}{}", DefaultPropertyName, ++index);
		}
		Delegates::OnPropertyAdded.Broadcast(id);
	}
	return id;
}

void Hierarchy::Delete(Resource::ID id)
{
	m_contexts[id].isInvalid = true;
}

void Hierarchy::SetModified(Resource::ID id, bool modified)
{
	m_contexts[id].isModified = modified;
}

Hierarchy::Root Hierarchy::GetRoot(Resource::ID id) const
{
	Resource::ID rootID{ Resource::InvalidID };
	Resource::ID parentID{ Resource::GetParent(id) };
	do
	{
		rootID = parentID;
		parentID = Resource::GetParent(parentID);
	} while (parentID != Resource::InvalidID);

	auto it{ std::ranges::find_if(m_roots, [rootID](const auto& root) { return root.id == rootID; }) };
	if (it == m_roots.end())
		return Root{ .id = Resource::InvalidID };
	return *it;
}

bool Hierarchy::IsModified(Resource::ID id) const
{
	if (m_contexts.contains(id))
		return m_contexts.at(id).isModified;
	return false;
}

bool Hierarchy::IsOpened(Resource::ID id) const
{
	if (m_contexts.contains(id))
		return m_contexts.at(id).isOpened;
	return false;
}

bool Hierarchy::IsSelected(Resource::ID id) const
{
	if (m_contexts.contains(id))
		return m_contexts.at(id).isSelected;
	return false;
}
