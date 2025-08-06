module;
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"

module rewriter.tool.engine.hierarchy;

import std;
import rewriter.tool.engine.delegates;
import rewriter.common.stringtable;
import rewriter.common.util;
import rewriter.library.graphics.direct3d;
import rewriter.library.resource;

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
	Delegates::OnPropAdded.Register(this, std::bind_front(&Hierarchy::OnPropAdded, this));
	Delegates::OnPropDeleted.Register(this, std::bind_front(&Hierarchy::OnPropDeleted, this));
	Delegates::OnPropModified.Register(this, std::bind_front(&Hierarchy::OnPropModified, this));
	Delegates::OnPropSelected.Register(this, std::bind_front(&Hierarchy::OnPropSelected, this));
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

		/*
		for (const auto& root : m_roots | std::ranges::views::keys)
			Recurse(root, [&prop](const auto& p) { p->Delete(prop); });
		*/
	}
	m_invalids.clear();

	// 만료된 프로퍼티 삭제
	std::erase_if(m_selects, [](const auto& p) { return p.expired(); });
	std::erase_if(m_opens, [](const auto& p) { return p.expired(); });

	// 유효하지 않은 모달 삭제
	std::erase_if(m_modals, [](const auto& m) { return !m->IsValid(); });
}

void Hierarchy::Render()
{
	if (ImGui::Begin(WindowName, nullptr, ImGuiWindowFlags_MenuBar))
	{
		Shortcut();
		DragDrop();
		RenderMenuBar();
		RenderPropertyTree();

		if (Graphics::ImGui::BeginFileDialog("Open File"))
		{
			ImGui::EndPopup();
		}
	}
	ImGui::End();
	RenderModal();
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

void Hierarchy::OnPropAdded(const std::shared_ptr<Resource::Property>& prop)
{
	/*
	if (auto parent{ prop->GetParent() })
		OpenTree(parent);
	*/
	SetModified(prop, true);
}

void Hierarchy::OnPropDeleted(const std::shared_ptr<Resource::Property>& prop)
{
	SetModified(prop, true);
}

void Hierarchy::OnPropModified(const std::shared_ptr<Resource::Property>& prop)
{
	SetModified(prop, true);
}

void Hierarchy::OnPropSelected(const std::shared_ptr<Resource::Property>& prop)
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
	auto name{ std::format(L"{}{}", DefaultFileName, Stringtable::DATA_FILE_EXT) };
	while (true)
	{
		auto it{ std::ranges::find_if(m_roots, [&name](const auto& root) { return Resource::GetName( root.first ) == name; }) };
		if (it == m_roots.end())
			break;
		name = std::format(L"{}{}{}", DefaultFileName, ++index, Stringtable::DATA_FILE_EXT);
	}

	auto root{ Resource::NewProperty() };
	Resource::SetName(root, name);
	m_roots.emplace(root, Root{ .isModified = true });
}

void Hierarchy::OnMenuFileOpen()
{
	Graphics::ImGui::OpenFileDialog("Open File");
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

	/*
	if (auto clipboard{ Clipboard::GetInstance() })
		clipboard->Copy(targets);
	*/
}

void Hierarchy::OnCopy()
{
	std::vector<std::shared_ptr<Resource::Property>> targets;
	for (const auto& select : m_selects)
	{
		if (auto prop{ select.lock() })
			targets.push_back(prop);
	}

	/*
	if (auto clipboard{ Clipboard::GetInstance() })
		clipboard->Copy(targets);
	*/
}

void Hierarchy::OnPaste()
{
	if (m_selects.size() != 1)
		return;

	auto select{ m_selects.front().lock() };
	if (!select)
		return;

	/*
	if (auto clipboard{ Clipboard::GetInstance() })
		clipboard->Paste(select);
	*/
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
		std::filesystem::path filePath{ static_cast<const wchar_t*>(payload->Data) };
		if (filePath.extension() == Stringtable::DATA_FILE_EXT)
			LoadDataFile(filePath);
	}

	ImGui::EndDragDropTarget();
}

void Hierarchy::RenderMenuBar()
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

void Hierarchy::RenderPropertyTree()
{
	ImGui::PushID("TreeNode");
	for (const auto& root : m_roots | std::views::keys)
		RenderProperty(root);
	ImGui::PopID();
}

void Hierarchy::RenderProperty(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::PushID(prop.get());

	const bool isRoot{ IsRoot(prop) };
	std::string name{ Util::wstou8s(Resource::GetName(prop)) };
	if (isRoot)
	{
		// 자식 노드에 변경 사항이 있으면 루트 노드 이름 앞에 '*' 추가
		if (IsModified(prop))
			name.insert(0, "* ");

		// 루트 노드는 폰트 키움
		ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 1.1f);
	}

	const bool isOpened{ IsOpened(prop) };
	ImGui::SetNextItemOpen(isOpened);

	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth };
	if (IsSelected(prop))
		flag |= ImGuiTreeNodeFlags_Selected;
	const bool isTreeNodeOpen{ ImGui::TreeNodeEx(name.c_str(), flag) };

	// 자식 노드 시작을 루트 노드와 동일한 인덴트로, 폰트 크기 원복
	if (isRoot)
	{
		ImGui::PopFont();
		ImGui::Unindent();
	}

	// 닫혀있어도 클릭 되도록
	if (ImGui::IsItemClicked())
		Delegates::OnPropSelected.Notify(prop);

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
		for (const auto& child : Resource::Iterator{ prop } | std::ranges::views::values)
			RenderProperty(child);
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

	if (!IsSelected(prop))
		Delegates::OnPropSelected.Notify(prop);

	// 루트 노드 전용 메뉴
	if (std::ranges::all_of(m_selects, [this](const auto& select) { return IsRoot(select.lock()); }))
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
						//ImGui::TextUnformatted(Util::wstou8s(std::format(L"변경 내용을 {}에 저장하시겠습니까?", p->GetName())).c_str());
						ImGui::TextUnformatted("Save?");
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
		std::wstring name{ DefaultPropertyName };
		while (true)
		{
			if (auto child{ Resource::Get(prop, name) })
			{
				name = std::format(L"{}{}", DefaultPropertyName, ++index);
				continue;
			}
			break;
		}

		auto child{ Resource::NewProperty() };
		Resource::SetName(child, name);
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
	/*
	if (auto root{ Resource::Get(path.wstring()) })
	{
		root->SetName(path.filename().wstring());
		m_roots.emplace(root, Root{ .path = path });
	}
	*/
}

void Hierarchy::Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func)
{
	/*
	for (const auto& [_, child] : *prop)
		Recurse(child, func);
	func(prop);
	*/
}

void Hierarchy::Add(const std::shared_ptr<Resource::Property>& parent, const std::shared_ptr<Resource::Property>& child)
{
	/*
	child->SetParent(parent);
	*/
	Resource::AddChild(parent, child);
	Delegates::OnPropAdded.Notify(child);
	Delegates::OnPropSelected.Notify(child);
}

void Hierarchy::Delete(const std::shared_ptr<Resource::Property>& prop)
{
	m_invalids.emplace_back(prop);
	Delegates::OnPropDeleted.Notify(prop);
}

void Hierarchy::Save(const std::shared_ptr<Resource::Property>& prop)
{
	auto root{ GetRoot(prop) };
	if (!root)
		return;

	if (!IsModified(root))
		return;

	/*
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
	*/
	SetModified(root, false);
}

void Hierarchy::SetModified(const std::shared_ptr<Resource::Property>& prop, bool modified)
{
	if (auto root{ GetRoot(prop) })
		m_roots[root].isModified = modified;
}

std::shared_ptr<Resource::Property> Hierarchy::GetRoot(const std::shared_ptr<Resource::Property>& prop) const
{
	/*
	auto root{ prop };
	auto parent{ prop };
	while (parent = parent->GetParent())
		root = parent;
	return IsRoot(root) ? root : nullptr;
	*/
	return nullptr;
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
