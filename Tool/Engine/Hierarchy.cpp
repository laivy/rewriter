#include "Stdafx.h"
#include "App.h"
#include "Clipboard.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

Hierarchy::IModal::IModal() :
	m_isValid{ true }
{
}

void Hierarchy::IModal::Close()
{
	m_isValid = false;
}

bool Hierarchy::IModal::IsValid() const
{
	return m_isValid;
}

Hierarchy::Hierarchy()
{
	App::OnPropertyAdd.Register(this, std::bind_front(&Hierarchy::OnPropertyAdd, this));
	App::OnPropertyDelete.Register(this, std::bind_front(&Hierarchy::OnPropertyDelete, this));
	App::OnPropertyModified.Register(this, std::bind_front(&Hierarchy::OnPropertyModified, this));
	App::OnPropertySelected.Register(this, std::bind_front(&Hierarchy::OnPropertySelected, this));
}

void Hierarchy::Update(float deltaTime)
{
	// 유효하지 않은 프로퍼티 삭제
	for (const auto& invalid : m_invalids)
	{
		auto prop{ invalid.lock() };
		if (!prop)
			continue;

		if (m_roots.contains(prop))
		{
			Resource::Unload(m_roots[prop].path.wstring());
			m_roots.erase(prop);
			continue;
		}

		for (const auto& root : m_roots | std::views::keys)
			Recurse(root, [&prop](const auto& p) { p->Delete(prop); });
	}
	m_invalids.clear();

	// 유효하지 않은 모달 삭제
	std::erase_if(m_modals, [](const auto& m) { return !m->IsValid(); });
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
	RenderModal();
	ImGui::PopID();
}

void Hierarchy::OpenTree(const std::shared_ptr<Resource::Property>& prop)
{
	if (!IsOpened(prop))
		m_opens.emplace_back(prop);
}

void Hierarchy::CloseTree(const std::shared_ptr<Resource::Property>& prop)
{
	std::erase_if(m_opens, [&prop](const auto& p) { return p.lock() == prop; });
}

bool Hierarchy::IsRoot(const std::shared_ptr<Resource::Property>& prop) const
{
	return m_roots.contains(prop);
}

void Hierarchy::OnPropertyAdd(const std::shared_ptr<Resource::Property>& prop)
{
	if (auto parent{ prop->GetParent() })
		OpenTree(parent);
	SetModified(prop, true);
}

void Hierarchy::OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop)
{
	SetModified(prop, true);
}

void Hierarchy::OnPropertyModified(const std::shared_ptr<Resource::Property>& prop)
{
	SetModified(prop, true);
}

void Hierarchy::OnPropertySelected(const std::shared_ptr<Resource::Property>& prop)
{
	// 이미 선택된 노드를 선택한 경우 선택 해제 안함
	// 그리고 이미 선택된 노드이기 때문에 컨테이너에 추가 안함
	if (IsSelected(prop))
		return;

	// Ctrl키를 누르고 있을 때는 선택 해제 안함
	if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		m_selects.clear();

	m_selects.push_back(prop);
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
		if (auto prop{ select.lock() })
			Save(prop);
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
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyPressed(ImGuiKey_UpArrow, false))
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
	if (ImGui::IsKeyDown(ImGuiMod_Alt) && ImGui::IsKeyPressed(ImGuiKey_DownArrow, false))
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

	// 변경 사항이 있으면 '*' 추가
	auto name{ Util::wstou8s(prop->GetName()) };
	if (isRoot && IsModified(prop))
		name.insert(0, "* ");

	bool isOpened{ IsOpened(prop) };
	if (isOpened)
		ImGui::SetNextItemOpen(true);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, isRoot ? ImVec2{ 0.0f, 5.0f } : ImVec2{ 0.0f, 2.0f });
	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth };
	if (isSelected)
		flag |= ImGuiTreeNodeFlags_Selected;
	if (!isRoot && prop->GetChildren().empty())
		flag |= ImGuiTreeNodeFlags_Leaf;
	bool isTreeNodeOpen{ ImGui::TreeNodeEx(name.c_str(), flag) };
	ImGui::PopStyleVar();

	// 루트 노드와 동일한 인덴트
	if (isRoot)
		ImGui::Unindent();

	// 닫혀있어도 클릭 되도록
	if (ImGui::IsItemClicked())
		App::OnPropertySelected.Notify(prop);

	// 트리 여닫기
	if (ImGui::IsItemToggledOpen())
	{
		if (isOpened)
			CloseTree(prop);
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

	if (isRoot)
		ImGui::Indent();

	ImGui::PopID();
}

void Hierarchy::RenderNodeContextMenu(const std::shared_ptr<Resource::Property>& prop)
{
	if (!ImGui::BeginPopupContextItem("CONTEXT"))
		return;

	App::OnPropertySelected.Notify(prop);

	// 루트 노드 전용 메뉴
	if (std::ranges::all_of(m_selects,
		[this](const auto& select)
		{
			if (auto p{ select.lock() })
				return IsRoot(p);
			return false;
		}))
	{
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

			class FileCloseModal final : public IModal
			{
			public:
				FileCloseModal(const std::shared_ptr<Resource::Property>& p) :
					m_prop{ p }
				{
				}
				~FileCloseModal() = default;

				void Run() override
				{
					auto p{ m_prop.lock() };
					if (!p)
					{
						Close();
						return;
					}

					ImGui::OpenPopup("Hierarchy##FileCloseModal");

					auto center{ ImGui::GetMainViewport()->GetCenter() };
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal("Hierarchy##FileCloseModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::TextUnformatted(Util::wstou8s(std::format(L"변경 내용을 {}에 저장하시겠습니까?", p->GetName())).c_str());
						if (ImGui::Button("저장"))
						{
							ImGui::CloseCurrentPopup();
							Hierarchy::GetInstance()->Save(p);
							Close();
						}
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("저장하지 않음"))
						{
							ImGui::CloseCurrentPopup();
							Hierarchy::GetInstance()->Delete(p);
							Close();
						}
						ImGui::SameLine();
						if (ImGui::Button("취소"))
						{
							ImGui::CloseCurrentPopup();
							Close();
						}
						ImGui::EndPopup();
					}
				}
			private:
				std::weak_ptr<Resource::Property> m_prop;
			};

			for (const auto& select : m_selects)
			{
				auto p{ select.lock() };
				if (!p)
					continue;

				if (!IsModified(p))
				{
					Delete(p);
					continue;
				}

				auto modal{ std::make_unique<FileCloseModal>(p) };
				m_modals.push_back(std::move(modal));
			}
		}
		ImGui::Separator();
	}

	// 공통 메뉴
	if (ImGui::MenuItem("Rename", "F2") || ImGui::IsKeyPressed(ImGuiKey_F2))
	{
		ImGui::CloseCurrentPopup();

		auto window{ ImGui::FindWindowByName("Inspector") };
		ImGui::ActivateItemByID(window->GetID("##INSPECTOR/NAME"));
	}

	if (ImGui::MenuItem("Add", "A") || ImGui::IsKeyPressed(ImGuiKey_A))
	{
		ImGui::CloseCurrentPopup();

		size_t index{ 0 };
		std::wstring name{ DEFAULT_PROPERTY_NAME };
		while (true)
		{
			if (auto child{ prop->Get(name) })
			{
				name = std::format(L"{}{}", DEFAULT_PROPERTY_NAME, ++index);
				continue;
			}
			break;
		}

		auto child{ std::make_shared<Resource::Property>() };
		child->SetName(name);
		Add(prop, child);
	}

	if (ImGui::MenuItem("Delete", "D") || ImGui::IsKeyPressed(ImGuiKey_D))
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

void Hierarchy::RenderModal()
{
	if (!m_modals.empty())
		m_modals.back()->Run();
}

void Hierarchy::LoadDataFile(const std::filesystem::path& path)
{
	auto root{ Resource::Get(path.wstring()) };
	root->SetName(path.filename().wstring());
	m_roots.emplace(root, Root{ .path = path });
}

void Hierarchy::Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func)
{
	for (const auto& [_, child] : *prop)
		Recurse(child, func);
	func(prop);
}

void Hierarchy::Add(const std::shared_ptr<Resource::Property>& parent, const std::shared_ptr<Resource::Property>& child)
{
	child->SetParent(parent);
	parent->Add(child);
	App::OnPropertyAdd.Notify(child);
	App::OnPropertySelected.Notify(child);
}

void Hierarchy::Delete(const std::shared_ptr<Resource::Property>& prop)
{
	m_invalids.emplace_back(prop);
	App::OnPropertyDelete.Notify(prop);
}

void Hierarchy::Save(const std::shared_ptr<Resource::Property>& prop)
{
	auto root{ GetRoot(prop) };
	if (!root)
		return;

	SetModified(root, false);

	auto& info{ m_roots.at(root) };
	if (info.path.empty())
	{
		std::wstring filePath{ root->GetName() };
		filePath.resize(MAX_PATH);

		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"Data File(*.dat)\0*.dat\0";
		ofn.lpstrFile = filePath.data();
		ofn.lpstrDefExt = Stringtable::DATA_FILE_EXT.data();
		ofn.nMaxFile = MAX_PATH;
		if (!::GetSaveFileName(&ofn))
			return;

		info.path = filePath;
		ImGui::GetIO().ClearInputKeys();
	}
	root->SetName(info.path.filename().wstring());
	Resource::Save(root, info.path.wstring());
}

void Hierarchy::SetModified(const std::shared_ptr<Resource::Property>& prop, bool modified)
{
	if (auto root{ GetRoot(prop) })
		m_roots[root].isModified = modified;
}

std::shared_ptr<Resource::Property> Hierarchy::GetRoot(const std::shared_ptr<Resource::Property>& prop) const
{
	auto root{ prop };
	auto parent{ prop };
	while (parent = parent->GetParent())
		root = parent;
	return IsRoot(root) ? root : nullptr;
}

bool Hierarchy::IsModified(const std::shared_ptr<Resource::Property>& prop) const
{
	auto root{ GetRoot(prop) };
	return root ? m_roots.at(root).isModified : false;
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
