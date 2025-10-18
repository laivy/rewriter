#include "Pch.h"
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
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (IsSelected(id))
		{
			m_contexts[id].isSelected = false;
			return;
		}
	}
	else
	{
		for (auto& context : m_contexts | std::views::values)
			context.isSelected = false;
	}
	m_contexts[id].isSelected = true;
}

void Hierarchy::OnMenuFileNew()
{
	const Resource::ID id{ New(Resource::InvalidID) };
	m_roots.emplace_back(id, *Resource::GetName(id));
	m_contexts[id] = { .isModified = true };
}

void Hierarchy::OnMenuFileOpen()
{
	std::wstring filePath(MAX_PATH, L'\0');

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
	const wchar_t* temp{ ofn.lpstrFile + ofn.nFileOffset };
	do
	{
		std::wstring_view fileName{ temp };
		fileNames.emplace_back(fileName);
		temp += fileName.size() + 1;
	} while (*temp);

	// 로드
	if (fileNames.size() == 1)
	{
		LoadFromFile(path);
	}
	else
	{
		for (const auto& fileName : fileNames)
			LoadFromFile(path / fileName);
	}
}

void Hierarchy::OnMenuFileSave()
{
	for (Resource::ID id : m_contexts | std::views::keys)
	{
		if (!IsSelected(id))
			continue;

		const auto root{ GetRoot(id) };
		if (!IsModified(root.id))
			continue;

		SetModified(root.id, false);
		Resource::SaveToFile(root.id, root.filePath);
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
	if (ImGui::IsKeyDown(ImGuiKey_F2))
	{
		if (auto window{ ImGui::FindWindowByName("Inspector") })
			ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		for (Resource::ID id : m_contexts | std::views::keys)
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
			LoadFromFile(filePath);
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
	struct MoveInfo
	{
		std::vector<Resource::ID> targetIDs;
		Resource::ID destID;
		std::size_t destIndex;
	};
	MoveInfo moveInfo{};

	auto popup = [this](Resource::ID id)
	{
		if (!ImGui::BeginPopupContextItem("PopupMenu"))
			return;

		if (!IsSelected(id))
			Delegates::OnPropertySelected.Broadcast(id);

		// 루트 노드 전용 메뉴
		do
		{
			auto selected{ m_contexts | std::views::keys | std::views::filter([this](auto id) { return IsSelected(id); }) };
			if (std::ranges::all_of(selected, [this](auto id) { return !IsRoot(id); }))
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
	};
	auto reorder = [this, &moveInfo](Resource::ID id, std::size_t index, bool isOpened)
	{
		const ImVec2 lt{ ImGui::GetItemRectMin() };
		const ImVec2 rb{ ImGui::GetItemRectMax() };
		const ImVec2 cursorPos{ ImGui::GetCursorPos() };
		const ImVec2 itemSpacing{ ImGui::GetStyle().ItemSpacing };
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0.0f);
		ImGui::InvisibleButton("Between", ImVec2{ rb.x - lt.x - cursorPos.x + itemSpacing.x, itemSpacing.y });
		ImGui::PopStyleVar();

		if (!ImGui::BeginDragDropTarget())
			return;

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
			std::ranges::copy_if(m_contexts | std::views::keys, std::back_inserter(moveInfo.targetIDs), [this](Resource::ID id)
			{
				return IsSelected(id);
			});
			moveInfo.destID = isOpened ? id : Resource::GetParent(id);
			moveInfo.destIndex = isOpened ? 0 : index + 1;
		}
		ImGui::EndDragDropTarget();
	};
	auto render = [this, &popup, &reorder](this auto self, Resource::ID id, std::size_t index) -> void
	{
		ImGui::PushID(std::to_string(id).c_str());

		const bool isRoot{ IsRoot(id) };
		std::string name{ Util::ToU8String(*Resource::GetName(id)) };
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

		// 폰트 크기 원복
		if (isRoot)
			ImGui::PopFont();

		// 트리 여닫기
		if (ImGui::IsItemToggledOpen())
		{
			if (isOpened)
				CloseTree(id);
			else
				OpenTree(id);
		}

		// 닫혀있어도 클릭 되도록
		if (ImGui::IsItemClicked())
			Delegates::OnPropertySelected.Broadcast(id);

		// 드래그 드랍
		if (!isRoot && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Hierarchy/Property", &id, sizeof(id));
			ImGui::TextUnformatted(name.c_str());
			ImGui::EndDragDropSource();
		}

		// 컨텍스트 메뉴
		popup(id);

		// 순서 변경을 위한 중간 영역
		reorder(id, index, isOpened);

		// 하위 트리
		if (isTreeNodeOpened)
		{
			for (const auto& [i, child] : Resource::Iterator{ id } | std::views::values | std::views::enumerate)
				self(child, i);
			ImGui::TreePop();
		}
		ImGui::PopID();
	};

	ImGui::PushID("TreeView");
	for (const auto& root : m_roots)
		render(root.id, 0);
	ImGui::PopID();

	// 프로퍼티 이동
	for (auto targetID : moveInfo.targetIDs | std::views::reverse)
		Resource::Move(targetID, moveInfo.destID, moveInfo.destIndex);

	// 클릭을 했는데 선택된 프로퍼티가 없으면 이미 선택되어 있는 프로퍼티 선택 해제
	if (!m_isAnyPropertySelected && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		for (auto& ctx : m_contexts | std::views::values)
			ctx.isSelected = false;
	}
}

void Hierarchy::RenderModal()
{
}

void Hierarchy::LoadFromFile(const std::filesystem::path& filePath)
{
	const auto id{ Resource::LoadFromFile(filePath, L"")};
	if (id == Resource::InvalidID)
	{
		assert(false && "failed to load data file");
		return;
	}
	m_roots.emplace_back(id, filePath);
	Resource::SetName(id, filePath.filename());
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
	id = GetRoot(id).id;
	m_contexts[id].isModified = modified;
}

Hierarchy::Root Hierarchy::GetRoot(Resource::ID id) const
{
	const Root Invalid{ .id = Resource::InvalidID };
	if (IsRoot(id))
	{
		auto it{ std::ranges::find(m_roots, id, &Root::id) };
		if (it == m_roots.end())
			return Invalid;
		return *it;
	}

	Resource::ID rootID{ Resource::GetParent(id) };
	while (true)
	{
		const Resource::ID temp{ Resource::GetParent(rootID) };
		if (temp == Resource::InvalidID)
			break;
		rootID = temp;
	}
	if (IsRoot(rootID))
	{
		auto it{ std::ranges::find(m_roots, rootID, &Root::id) };
		if (it == m_roots.end())
			return Invalid;
		return *it;
	}
	return Invalid;
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
