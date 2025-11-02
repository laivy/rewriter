#include "Pch.h"
#include "Delegates.h"
#include "Hierarchy.h"
#include "Common/Util.h"

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
		const auto& [id, context] { elem };
		if (!context.isInvalid)
			return false;

		std::erase_if(m_roots, [id](const auto& root) { return root.id == id; });
		std::erase(m_selectedIDs, id);
		Resource::Delete(id);
		return true;
	});
	m_isAnyPropertySelected = false;
}

void Hierarchy::Render()
{
	if (ImGui::Begin("편집기", nullptr, ImGuiWindowFlags_MenuBar))
	{
		Shortcut();
		DragDrop();
		MenuBar();
		TreeView();
	}
	ImGui::End();
}

void Hierarchy::Delete(Resource::ID id)
{
	m_contexts[id].isInvalid = true;
	if (!IsRoot(id))
		Delegates::OnPropertyDeleted.Broadcast(id);
}

void Hierarchy::SetModified(Resource::ID id, bool modified)
{
	if (auto root{ GetRoot(id) })
		m_contexts[root->id].isModified = modified;
}

void Hierarchy::SetOpened(Resource::ID id, bool opened)
{
	m_contexts[id].isOpened = opened;
}

void Hierarchy::SetSelected(Resource::ID id, bool selected)
{
	if (selected)
	{
		if (!std::ranges::contains(m_selectedIDs, id))
			m_selectedIDs.push_back(id);
	}
	else
	{
		std::erase(m_selectedIDs, id);
	}
}

Hierarchy::Root* Hierarchy::GetRoot(Resource::ID id)
{
	auto it{ std::ranges::find(m_roots, id, &Root::id) };
	if (it != m_roots.end())
		return &*it;

	Resource::ID rootID{ id };
	while (true)
	{
		const Resource::ID temp{ Resource::GetParent(rootID) };
		if (temp == Resource::InvalidID)
			break;
		rootID = temp;
	}
	it = std::ranges::find(m_roots, rootID, &Root::id);
	if (it != m_roots.end())
		return &*it;
	return nullptr;
}

bool Hierarchy::IsRoot(Resource::ID id) const
{
	return std::ranges::contains(m_roots, id, &Root::id);
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
	return std::ranges::contains(m_selectedIDs, id);
}

void Hierarchy::OnPropertyAdded(Resource::ID id)
{
	Resource::ID parentID{ Resource::GetParent(id) };
	while (parentID != Resource::InvalidID)
	{
		SetOpened(parentID, true);
		parentID = Resource::GetParent(parentID);
	}
	SetOpened(id, true);
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

	// 키보드로 선택된 경우
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow) || ImGui::IsKeyDown(ImGuiKey_DownArrow))
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
		{
			SetSelected(id, true);
		}
		else
		{
			m_selectedIDs.clear();
			SetSelected(id, true);
		}
	}
	// 마우스로 선택된 경우
	else
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			SetSelected(id, !IsSelected(id));
		}
		else
		{
			if (IsSelected(id))
				return;
			m_selectedIDs.clear();
			SetSelected(id, true);
		}
	}
}

void Hierarchy::OnMenuFileNew()
{
	New(Resource::InvalidID);
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
	const BOOL isSuccess{ ::GetOpenFileName(&ofn) };
	ImGui::GetIO().ClearInputKeys();
	if (!isSuccess)
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
	for (Resource::ID id : m_selectedIDs)
	{
		auto root{ GetRoot(id) };
		if (!IsModified(root->id))
			continue;

		if (root->filePath.empty())
		{
			std::wstring filePath(MAX_PATH, L'\0');
			OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
			ofn.lpstrFile = filePath.data();
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
			ofn.lpstrDefExt = L"dat";
			const BOOL isSuccess{ ::GetSaveFileName(&ofn) };
			ImGui::GetIO().ClearInputKeys();
			if (!isSuccess)
				continue;
			root->filePath = ofn.lpstrFile;
		}

		Resource::SaveToFile(root->id, root->filePath);
		SetModified(root->id, false);
	}
}

void Hierarchy::OnMenuFileSaveAs()
{
	if (m_selectedIDs.empty())
		return;

	auto root{ GetRoot(m_selectedIDs.back()) };
	if (!root)
		return;

	std::wstring filePath(MAX_PATH, L'\0');
	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = filePath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.lpstrDefExt = L"dat";
	const BOOL isSuccess{ ::GetSaveFileName(&ofn) };
	ImGui::GetIO().ClearInputKeys();
	if (!isSuccess)
		return;

	Resource::SaveToFile(root->id, filePath);
}

void Hierarchy::OnMenuFileSaveAll()
{
	for (auto& root : m_roots)
	{
		if (!IsModified(root.id))
			continue;

		if (root.filePath.empty())
		{
			std::wstring filePath(MAX_PATH, L'\0');
			OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
			ofn.lpstrFile = filePath.data();
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
			ofn.lpstrDefExt = L"dat";
			const BOOL isSuccess{ ::GetSaveFileName(&ofn) };
			ImGui::GetIO().ClearInputKeys();
			if (!isSuccess)
				continue;
			root.filePath = ofn.lpstrFile;
		}

		Resource::SaveToFile(root.id, root.filePath);
		SetModified(root.id, false);
	}
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

	//if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyDown(ImGuiKey_N))
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N))
		OnMenuFileNew();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		OnMenuFileOpen();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		OnMenuFileSave();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_X))
		OnCut();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C))
		OnCopy();
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
		OnPaste();
	if (ImGui::Shortcut(ImGuiKey_F2))
	{
		auto selectedIDs{ m_selectedIDs | std::views::filter([this](auto id) { return !IsRoot(id); }) };
		if (!selectedIDs.empty())
			SetRenamePopup(selectedIDs.back(), true);
	}
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		for (Resource::ID id : m_selectedIDs)
			Delete(id);
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
		if (filePath.extension() == Stringtable::DataFileExtension)
			LoadFromFile(filePath);
	}
	ImGui::EndDragDropTarget();
}

void Hierarchy::MenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("파일"))
	{
		if (ImGui::MenuItem("새 파일", "Ctrl+N") || (ImGui::IsKeyPressed(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_N)))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileNew();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("파일 열기", "Ctrl+O") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileOpen();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("저장", "Ctrl+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSave();
		}
		if (ImGui::MenuItem("다른 이름으로 저장", "Ctrl+Shift+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSaveAs();
		}
		if (ImGui::MenuItem("모두 저장", "Ctrl+Alt+S") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S))
		{
			ImGui::CloseCurrentPopup();
			OnMenuFileSaveAll();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("편집"))
	{
		if (ImGui::MenuItem("잘라내기", "Ctrl+X") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_X))
		{
			ImGui::CloseCurrentPopup();
			OnCut();
		}
		if (ImGui::MenuItem("복사", "Ctrl+C") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C))
		{
			ImGui::CloseCurrentPopup();
			OnCopy();
		}
		if (ImGui::MenuItem("붙여넣기", "Ctrl+V") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V))
		{
			ImGui::CloseCurrentPopup();
			OnPaste();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("찾기", "Ctrl+F") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_F))
		{
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("바꾸기", "Ctrl+H") || ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_H))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}

void Hierarchy::TreeView()
{
	struct ReorderInfo
	{
		std::vector<Resource::ID> targetIDs;
		Resource::ID destID{ Resource::InvalidID };
		std::size_t destIndex{ 0 };
	};

	struct MultiSelectInfo
	{
		Resource::ID from{ Resource::InvalidID };
		Resource::ID to{ Resource::InvalidID };
	};

	std::vector<Resource::ID> treeIDs;
	ReorderInfo reorderInfo;
	MultiSelectInfo multiSelectInfo;
	multiSelectInfo.from = m_selectedIDs.empty() ? Resource::InvalidID : m_selectedIDs.back();

	auto popup = [this](Resource::ID id)
	{
		if (!ImGui::BeginPopupContextItem("PopupMenu"))
			return;

		if (!IsSelected(id))
			Delegates::OnPropertySelected.Broadcast(id);

		// 파일 메뉴
		do
		{
			if (std::ranges::any_of(m_selectedIDs, [this](auto id) { return !IsRoot(id); }))
				break;

			if (ImGui::MenuItem("새 프로퍼티", "N") || ImGui::IsKeyPressed(ImGuiKey_N))
			{
				ImGui::CloseCurrentPopup();
				New(id);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("저장", "S") || ImGui::IsKeyPressed(ImGuiKey_S))
			{
				ImGui::CloseCurrentPopup();
				OnMenuFileSave();
			}
			if (ImGui::MenuItem("다른 이름으로 저장"))
			{
				ImGui::CloseCurrentPopup();
				OnMenuFileSaveAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("닫기", "C") || ImGui::IsKeyPressed(ImGuiKey_C))
			{
				ImGui::CloseCurrentPopup();
				if (IsModified(id))
					SetWarningModal(id, true);
				else
					Delete(id);
			}
		} while (false);

		// 프로퍼티 메뉴
		do
		{
			if (std::ranges::any_of(m_selectedIDs, [this](auto id) { return IsRoot(id); }))
				break;

			if (ImGui::MenuItem("새 프로퍼티", "N") || ImGui::IsKeyPressed(ImGuiKey_N))
			{
				ImGui::CloseCurrentPopup();
				New(id);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("이름 바꾸기", "F2") || ImGui::Shortcut(ImGuiKey_F2))
			{
				ImGui::CloseCurrentPopup();
				SetRenamePopup(id, true);
			}
			if (ImGui::MenuItem("삭제", "D") || ImGui::IsKeyPressed(ImGuiKey_D))
			{
				ImGui::CloseCurrentPopup();
				for (const auto id : m_selectedIDs)
					Delete(id);
			}
		} while (false);
		ImGui::EndPopup();
	};
	auto reorder = [this, &reorderInfo](Resource::ID id, std::size_t index, bool isOpened)
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

		ImRect rect{};
		rect.Min = ImGui::GetItemRectMin();
		rect.Max = ImGui::GetItemRectMax();

		constexpr float Thickness{ 1.0f };
		const ImVec2 center{ rect.GetCenter() };
		rect.Min.y = center.y - Thickness / 2.0f;
		rect.Max.y = center.y + Thickness / 2.0f;

		ImDrawList* drawList{ ImGui::GetWindowDrawList() };
		drawList->AddRectFilled(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget));

		if (auto payload{ ImGui::AcceptDragDropPayload("Hierarchy/Property", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) })
		{
			std::ranges::copy(m_selectedIDs, std::back_inserter(reorderInfo.targetIDs));
			if (isOpened)
			{
				reorderInfo.destID = id;
				reorderInfo.destIndex = 0;
			}
			else
			{
				reorderInfo.destID = Resource::GetParent(id);
				reorderInfo.destIndex = index + 1;
			}
		}
		ImGui::EndDragDropTarget();
	};
	auto render = [this, &treeIDs, &multiSelectInfo, &popup, &reorder](this auto self, Resource::ID id, std::size_t index) -> void
	{
		ImGui::PushID(std::to_string(id).c_str());
		treeIDs.push_back(id);

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

		ImGuiTreeNodeFlags flag{
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_DrawLinesToNodes |
			ImGuiTreeNodeFlags_NavLeftJumpsToParent
		};
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
			SetOpened(id, !isOpened);

		// 키보드, 마우스로 노드 선택 처리
		static ImGuiID lastFocusID{ 0 };
		const ImGuiID currentFocusID{ ImGui::GetFocusID() };
		if (lastFocusID != currentFocusID && ImGui::IsItemFocused())
		{
			lastFocusID = currentFocusID;
			Delegates::OnPropertySelected.Broadcast(id);
		}
		if (ImGui::IsItemClicked())
		{
			if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
				multiSelectInfo.to = id;

			const ImGuiID currentID{ ImGui::GetItemID() };
			ImGui::SetFocusID(currentID, ImGui::GetCurrentWindow());
			ImGui::SetNavCursorVisibleAfterMove();
			lastFocusID = currentID;
			Delegates::OnPropertySelected.Broadcast(id);
		}

		// 드래그 드랍
		if (!isRoot && ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Hierarchy/Property", &id, sizeof(id));
			ImGui::TextUnformatted(name.c_str());
			ImGui::EndDragDropSource();
		}

		// 이름 변경
		if (IsRenamePopupOpened(id))
		{
			if (!isTreeNodeOpened)
				ImGui::Indent();
			ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
			if (!isTreeNodeOpened)
				ImGui::Unindent();
			ImGui::OpenPopup("Rename");
			SetRenamePopup(id, false);
		}
		if (ImGui::BeginPopup("Rename", ImGuiWindowFlags_NoNav))
		{
			ImGui::SetKeyboardFocusHere();
			std::string buffer{ name };
			if (ImGui::InputText("##Rename", &buffer, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::wstring newName{ Util::ToWString(buffer) };
				if (Resource::SetName(id, newName))
					Delegates::OnPropertyModified.Broadcast(id);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::IsKeyDown(ImGuiKey_Escape))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		// 파일 닫기 경고
		if (IsWarningModalOpened(id))
		{
			ImGui::OpenPopup("경고");
			SetWarningModal(id, false);
		}
		if (ImGui::BeginPopupModal("경고", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::TextUnformatted("변경 사항이 저장되지 않았습니다.\n파일을 닫으시겠습니까?");
			ImGui::Separator();
			if (ImGui::Button("예", ImVec2{ 80.0f, 0.0f }) || ImGui::IsKeyDown(ImGuiKey_Enter))
			{
				ImGui::CloseCurrentPopup();
				Delete(id);
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("아니요", ImVec2{ 80.0f, 0.0f }) || ImGui::IsKeyDown(ImGuiKey_Escape))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
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
	if (!reorderInfo.targetIDs.empty())
	{
		std::ranges::sort(reorderInfo.targetIDs, [&treeIDs](auto lhs, auto rhs)
		{
			return
				std::distance(treeIDs.begin(), std::ranges::find(treeIDs, lhs)) <
				std::distance(treeIDs.begin(), std::ranges::find(treeIDs, rhs));
		});
		for (auto targetID : reorderInfo.targetIDs | std::views::reverse)
			Resource::Move(targetID, reorderInfo.destID, reorderInfo.destIndex);
	}

	// Shift키 다중 선택
	do
	{
		if (multiSelectInfo.from == Resource::InvalidID || multiSelectInfo.to == Resource::InvalidID)
			break;

		auto from{ std::ranges::find(treeIDs, multiSelectInfo.from) };
		auto to{ std::ranges::find(treeIDs, multiSelectInfo.to) };
		if (from == treeIDs.end() || to == treeIDs.end())
			break;

		auto [begin, end] { std::minmax(from, to) };
		for (auto it{ begin }; it <= end; ++it)
			SetSelected(*it, true);
	} while (false);

	// 클릭을 했는데 선택된 프로퍼티가 없으면 이미 선택되어 있는 프로퍼티 선택 해제
	do
	{
		if (m_isAnyPropertySelected)
			break;
		if (!ImGui::IsWindowHovered())
			break;
		if (!ImGui::GetCurrentWindowRead()->InnerRect.Contains(ImGui::GetMousePos())) // 메뉴바 영역 제외
			break;
		if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			break;
		m_selectedIDs.clear();
	} while (false);
}

void Hierarchy::LoadFromFile(const std::filesystem::path& filePath)
{
	if (std::ranges::contains(m_roots, filePath, &Root::filePath))
		return;

	const auto id{ Resource::LoadFromFile(filePath, L"") };
	if (id == Resource::InvalidID)
	{
		assert(false && "failed to load data file");
		return;
	}
	m_roots.emplace_back(id, filePath);
	SetOpened(id, true);
}

Resource::ID Hierarchy::New(Resource::ID parentID)
{
	Resource::ID id{ Resource::InvalidID };
	if (parentID == Resource::InvalidID)
	{
		// 새로운 파일
		std::wstring filePath(MAX_PATH, L'\0');
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
		ofn.lpstrFile = filePath.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
		ofn.lpstrDefExt = L"dat";
		const BOOL isSuccess{ ::GetSaveFileName(&ofn) };
		ImGui::GetIO().ClearInputKeys();
		if (!isSuccess)
			return Resource::InvalidID;

		std::ranges::replace(filePath, std::filesystem::path::preferred_separator, Stringtable::DataPathSeperator.front());
		std::wstring name{ filePath.substr(filePath.rfind(Stringtable::DataPathSeperator) + 1) };
		id = Resource::New(name);
		m_roots.emplace_back(id, filePath);
		SetModified(id, false);
		SetOpened(id, true);
		Delegates::OnPropertyAdded.Broadcast(id);
	}
	else
	{
		// 새로운 프로퍼티
		constexpr auto DefaultPropertyName{ L"Property" };
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

void Hierarchy::SetWarningModal(Resource::ID id, bool opened)
{
	m_contexts[id].openWarningModal = opened;
}

void Hierarchy::SetRenamePopup(Resource::ID id, bool opened)
{
	m_contexts[id].openRenamePopup = opened;
}

bool Hierarchy::IsWarningModalOpened(Resource::ID id) const
{
	if (m_contexts.contains(id))
		return m_contexts.at(id).openWarningModal;
	return false;
}

bool Hierarchy::IsRenamePopupOpened(Resource::ID id) const
{
	if (m_contexts.contains(id))
		return m_contexts.at(id).openRenamePopup;
	return false;
}
