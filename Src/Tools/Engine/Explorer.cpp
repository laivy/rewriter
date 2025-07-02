#include "Stdafx.h"
#include "Explorer.h"
#include "Common/Util.h"

namespace
{
	std::vector<std::string> SplitString(std::string_view string, const float width)
	{
		std::vector<std::string> lines;
		size_t i{ 1 };
		while (!string.empty())
		{
			if (ImGui::CalcTextSize(string.data(), string.data() + i).x > width)
			{
				std::string_view line{ string.substr(0, i - 1) };
				lines.emplace_back(line);
				string = string.substr(i - 1);
				i = 1;

				// 3줄까지만 표시
				constexpr auto lineMax{ 3ULL };
				if (lines.size() >= lineMax - 1)
				{
					// 남은 문자열의 길이가 길이 제한에 걸리지 않으면 줄 추가하고 끝
					if (ImGui::CalcTextSize(string.data(), string.data() + string.size()).x <= width)
					{
						lines.emplace_back(string);
						return lines;
					}

					// 말줄임표를 표시할 수 있을만큼 문자열 뒷부분을 자르고 줄 추가
					const ImVec2 ellipsisTextSize{ ImGui::CalcTextSize("...") };
					size_t j{ string.size() };
					while (true)
					{
						if (ImGui::CalcTextSize(string.data(), string.data() + j).x + ellipsisTextSize.x <= width)
						{
							std::string ellipsized{ string.substr(0, j) };
							ellipsized += "...";
							lines.emplace_back(std::move(ellipsized));
							break;
						}
						if (j == 0)
						{
							break;
						}
						--j;
					}
					return lines;
				}
				continue;
			}
			if (i == string.size())
			{
				lines.emplace_back(string);
				break;
			}
			++i;
		}
		return lines;
	}

	bool IconButton(const std::shared_ptr<Graphics::ImGui::Texture>& icon, std::string_view label)
	{
		// wrapWidth를 기준으로 줄로 나눔
		constexpr float wrapWidth{ 80.0f };
		const std::vector<std::string> lines{ SplitString(label, wrapWidth) };

		// 너비는 고정, 높이는 가변
		constexpr float itemWidth{ 100.0f };
		constexpr ImVec2 iconSize{ 60.0f, 60.0f };
		const ImVec2 itemSpacing{ ImGui::GetStyle().ItemSpacing };
		const float fontHeight{ ImGui::GetFontSize() };
		const ImVec2 itemSize{ itemWidth, iconSize.y + itemSpacing.y * (lines.size() + 1) + fontHeight * lines.size() };

		// 줄바꿈
		if (ImGui::GetContentRegionMax().x < ImGui::GetCursorPosX() + itemSpacing.x + itemSize.x)
			ImGui::Spacing();

		// 선택된 아이템 하이라이트
		static ImGuiID selectedItemID{ 0 };
		if (selectedItemID == ImGui::GetID(label.data()))
		{
			const ImVec2 startCursorScreenPos{ ImGui::GetCursorScreenPos() };
			const ImVec2 lt{ startCursorScreenPos };
			const ImVec2 rb{ startCursorScreenPos.x + itemSize.x, startCursorScreenPos.y + itemSize.y };
			ImDrawList* drawList{ ImGui::GetWindowDrawList() };
			drawList->AddRectFilled(lt, rb, IM_COL32(80, 80, 80, 255));
			drawList->AddRect(lt, rb, IM_COL32(195, 195, 195, 255));
		}

		ImGui::BeginGroup();

		// 아이콘
		const ImVec2 startCursorPos{ ImGui::GetCursorPos() };
		ImGui::SetCursorPosX(startCursorPos.x + (itemWidth - iconSize.x) / 2.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemSpacing.y);
		Graphics::ImGui::Image(icon, iconSize);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - itemSpacing.y);

		// 파일 또는 폴더 이름
		for (const auto& line : lines)
		{
			ImVec2 textSize{ ImGui::CalcTextSize(line.data(), line.data() + line.size()) };
			ImGui::SetCursorPosX(startCursorPos.x + (itemWidth - textSize.x) / 2.0f);
			ImGui::TextUnformatted(line.data(), line.data() + line.size());
		}
		ImGui::EndGroup();

		// 전체를 덮는 투명 버튼
		bool isClicked{ false };
		ImGui::SetCursorPos(startCursorPos);
		if (ImGui::InvisibleButton(label.data(), itemSize))
		{
			ImGuiID itemID{ ImGui::GetItemID() };
			if (selectedItemID == itemID)
			{
				selectedItemID = 0;
				isClicked = true;
			}
			else
			{
				selectedItemID = itemID;
			}
		}
		return isClicked;
	}

	// ImGui::TreeNodeBehavior 함수 기반
	// <펼침 버튼이 눌렸는지, 이름 부분이 눌렸는지>
	std::pair<bool, bool> IconTreeNode(const std::shared_ptr<Graphics::ImGui::Texture>& icon, std::string_view label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None)
	{
		ImGuiID id = ImGui::GetID(label.data());

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return std::make_pair(false, false);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));
		const ImVec2 label_size = ImGui::CalcTextSize(label.data());

		constexpr ImVec2 icon_size{ 16.0f, 16.0f };
		const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2) + (icon ? icon_size.x + padding.x : 0.0f); // Collapser arrow width + Spacing + icon
		const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset); // Latch before ItemSize changes it
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f) + (icon ? icon_size.x + padding.x * 2 : 0.0f); // Include collapser and icon

		// We vertically grow up to current line height up the typical widget height.
		const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
		const bool span_all_columns = (flags & ImGuiTreeNodeFlags_SpanAllColumns) != 0 && (g.CurrentTable != NULL);
		const bool span_all_columns_label = (flags & ImGuiTreeNodeFlags_LabelSpanAllColumns) != 0 && (g.CurrentTable != NULL);
		ImRect frame_bb;
		frame_bb.Min.x = span_all_columns ? window->ParentWorkRect.Min.x : (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = span_all_columns ? window->ParentWorkRect.Max.x : (flags & ImGuiTreeNodeFlags_SpanLabelWidth) ? window->DC.CursorPos.x + text_width + padding.x : window->WorkRect.Max.x;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame)
		{
			const float outer_extend = IM_TRUNC(window->WindowPadding.x * 0.5f); // Framed header expand a little outside of current limits
			frame_bb.Min.x -= outer_extend;
			frame_bb.Max.x += outer_extend;
		}

		ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
		ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect interact_bb = frame_bb;
		if ((flags & (ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanLabelWidth | ImGuiTreeNodeFlags_SpanAllColumns)) == 0)
			interact_bb.Max.x = frame_bb.Min.x + text_width + (label_size.x > 0.0f ? style.ItemSpacing.x * 2.0f : 0.0f);

		// Compute open and multi-select states before ItemAdd() as it clear NextItem data.
		ImGuiID storage_id = (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasStorageID) ? g.NextItemData.StorageId : id;
		bool is_open = ImGui::TreeNodeUpdateNextOpen(storage_id, flags);

		bool is_visible;
		if (span_all_columns || span_all_columns_label)
		{
			// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackgroundChannel for every Selectable..
			const float backup_clip_rect_min_x = window->ClipRect.Min.x;
			const float backup_clip_rect_max_x = window->ClipRect.Max.x;
			window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
			window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
			is_visible = ImGui::ItemAdd(interact_bb, id);
			window->ClipRect.Min.x = backup_clip_rect_min_x;
			window->ClipRect.Max.x = backup_clip_rect_max_x;
		}
		else
		{
			is_visible = ImGui::ItemAdd(interact_bb, id);
		}
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		g.LastItemData.DisplayRect = frame_bb;

		// If a NavLeft request is happening and ImGuiTreeNodeFlags_NavLeftJumpsToParent enabled:
		// Store data for the current depth to allow returning to this node from any child item.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// It will become tempting to enable ImGuiTreeNodeFlags_NavLeftJumpsToParent by default or move it to ImGuiStyle.
		bool store_tree_node_stack_data = false;
		if ((flags & ImGuiTreeNodeFlags_DrawLinesMask_) == 0)
			flags |= g.Style.TreeLinesFlags;
		const bool draw_tree_lines = (flags & (ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes)) && (frame_bb.Min.y < window->ClipRect.Max.y) && (g.Style.TreeLinesSize > 0.0f);
		if (!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			store_tree_node_stack_data = draw_tree_lines;
			if ((flags & ImGuiTreeNodeFlags_NavLeftJumpsToParent) && !g.NavIdIsAlive)
				if (g.NavMoveDir == ImGuiDir_Left && g.NavWindow == window && ImGui::NavMoveRequestButNoResultYet())
					store_tree_node_stack_data = true;
		}

		const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
		if (!is_visible)
		{
			if ((flags & ImGuiTreeNodeFlags_DrawLinesToNodes) && (window->DC.TreeRecordsClippedNodesY2Mask & (1 << (window->DC.TreeDepth - 1))))
			{
				ImGuiTreeNodeStackData* parent_data = &g.TreeNodeStack.Data[g.TreeNodeStack.Size - 1];
				parent_data->DrawLinesToNodesY2 = ImMax(parent_data->DrawLinesToNodesY2, window->DC.CursorPos.y); // Don't need to aim to mid Y position as we are clipped anyway.
				if (frame_bb.Min.y >= window->ClipRect.Max.y)
					window->DC.TreeRecordsClippedNodesY2Mask &= ~(1 << (window->DC.TreeDepth - 1)); // Done
			}
			//if (is_open && store_tree_node_stack_data)
			//	ImGui::TreeNodeStoreStackData(flags, text_pos.x - text_offset_x); // Call before TreePushOverrideID()
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
				ImGui::TreePushOverrideID(id);
			IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
			return std::make_pair(is_open, false);
		}

		if (span_all_columns || span_all_columns_label)
		{
			ImGui::TablePushBackgroundChannel();
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasClipRect;
			g.LastItemData.ClipRect = window->ClipRect;
		}

		ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
		if ((flags & ImGuiTreeNodeFlags_AllowOverlap) || (g.LastItemData.ItemFlags & ImGuiItemFlags_AllowOverlap))
			button_flags |= ImGuiButtonFlags_AllowOverlap;
		if (!is_leaf)
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
		const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
		const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);

		const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
		if (is_multi_select) // We absolutely need to distinguish open vs select so _OpenOnArrow comes by default
			flags |= (flags & ImGuiTreeNodeFlags_OpenOnMask_) == 0 ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_OpenOnArrow;

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_PressedOnClick;
		else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		else
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease;
		if (flags & ImGuiTreeNodeFlags_NoNavFocus)
			button_flags |= ImGuiButtonFlags_NoNavFocus;

		bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool was_selected = selected;

		// Multi-selection support (header)
		if (is_multi_select)
		{
			// Handle multi-select + alter button flags for it
			ImGui::MultiSelectItemHeader(id, &selected, &button_flags);
			if (is_mouse_x_over_arrow)
				button_flags = (button_flags | ImGuiButtonFlags_PressedOnClick) & ~ImGuiButtonFlags_PressedOnClickRelease;
		}
		else
		{
			if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
				button_flags |= ImGuiButtonFlags_NoKeyModsAllowed;
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
		bool toggled = false;
		if (!is_leaf)
		{
			if (pressed && g.DragDropHoldJustPressedId != id)
			{
				//if ((flags & ImGuiTreeNodeFlags_OpenOnMask_) == 0 || (g.NavActivateId == id && !is_multi_select))
				//	toggled = true; // Single click
				//if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
					toggled |= is_mouse_x_over_arrow && !g.NavHighlightItemUnderNav; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
				//if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
				//	toggled = true; // Double click
			}
			else if (pressed && g.DragDropHoldJustPressedId == id)
			{
				IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
				if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
					toggled = true;
				else
					pressed = false; // Cancel press so it doesn't trigger selection.
			}

			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open)
			{
				toggled = true;
				ImGui::NavClearPreferredPosForAxis(ImGuiAxis_X);
				ImGui::NavMoveRequestCancel();
			}
			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
			{
				toggled = true;
				ImGui::NavClearPreferredPosForAxis(ImGuiAxis_X);
				ImGui::NavMoveRequestCancel();
			}

			if (toggled)
			{
				is_open = !is_open;
				window->DC.StateStorage->SetInt(storage_id, is_open);
				g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}

		// Multi-selection support (footer)
		if (is_multi_select)
		{
			bool pressed_copy = pressed && !toggled;
			ImGui::MultiSelectItemFooter(id, &selected, &pressed_copy);
			if (pressed)
				ImGui::SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, interact_bb);
		}

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		{
			const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
			ImGuiNavRenderCursorFlags nav_render_cursor_flags = ImGuiNavRenderCursorFlags_Compact;
			if (is_multi_select)
				nav_render_cursor_flags |= ImGuiNavRenderCursorFlags_AlwaysDraw; // Always show the nav rectangle
			if (display_frame)
			{
				// Framed type
				const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
				ImGui::RenderNavCursor(frame_bb, id, nav_render_cursor_flags);
				if (span_all_columns && !span_all_columns_label)
					ImGui::TablePopBackgroundChannel();
				if (flags & ImGuiTreeNodeFlags_Bullet)
					ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
				else if (!is_leaf)
					ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ((flags & ImGuiTreeNodeFlags_UpsideDownArrow) ? ImGuiDir_Up : ImGuiDir_Down) : ImGuiDir_Right, 1.0f);
				else // Leaf without bullet, left-adjusted text
					text_pos.x -= text_offset_x - padding.x;
				if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
					frame_bb.Max.x -= g.FontSize + style.FramePadding.x;
				if (g.LogEnabled)
					ImGui::LogSetNextTextDecoration("###", "###");
			}
			else
			{
				// Unframed typed for tree nodes
				if (hovered || selected)
				{
					const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
					ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
				}
				ImGui::RenderNavCursor(frame_bb, id, nav_render_cursor_flags);
				if (span_all_columns && !span_all_columns_label)
					ImGui::TablePopBackgroundChannel();
				if (flags & ImGuiTreeNodeFlags_Bullet)
					ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
				else if (!is_leaf)
					ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ((flags & ImGuiTreeNodeFlags_UpsideDownArrow) ? ImGuiDir_Up : ImGuiDir_Down) : ImGuiDir_Right, 0.70f);
				if (icon)
				{
					ImVec2 lt{ text_pos.x - padding.x - icon_size.x, text_pos.y };
					ImVec2 rb{ text_pos.x - padding.x, text_pos.y + icon_size.y };
					ImDrawList* drawList{ ImGui::GetWindowDrawList() };
					drawList->AddImage(icon->GetImGuiTextureID(), lt, rb);
				}
				if (g.LogEnabled)
					ImGui::LogSetNextTextDecoration(">", NULL);
				ImGui::RenderText(text_pos, label.data(), nullptr, false);
			}

			if (draw_tree_lines)
				ImGui::TreeNodeDrawLineToChildNode(ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.5f));

			// Label
			if (display_frame)
				ImGui::RenderTextClipped(text_pos, frame_bb.Max, label.data(), label.data() + label.size(), &label_size);
			else
				ImGui::RenderText(text_pos, label.data(), label.data() + label.size(), false);

			if (span_all_columns_label)
				ImGui::TablePopBackgroundChannel();
		}

		//if (is_open && store_tree_node_stack_data)
		//	ImGui::TreeNodeStoreStackData(flags, text_pos.x - text_offset_x); // Call before TreePushOverrideID()
		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id); // Could use TreePush(label) but this avoid computing twice

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return std::make_pair(is_open, pressed && !toggled);
	}
}

Explorer::Explorer() :
	m_scrollAddressBarToRight{ false }
{
	SetPath(std::filesystem::current_path());
}

void Explorer::Update(float deltaTime)
{
}

void Explorer::Render()
{
	if (ImGui::Begin(WINDOW_NAME))
	{
		RenderFileTree();
		ImGui::BeginGroup();
		RenderAddressBar();
		ImGui::Separator();
		RenderFileViewer();
		ImGui::EndGroup();
	}
	ImGui::End();
}

void Explorer::RenderFileTree()
{
	if (!ImGui::BeginChild("FileTree", ImVec2{ 175.0f, 0.0f }, ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::EndChild();
		return;
	}

	DWORD bufferSize{ ::GetLogicalDriveStrings(0, nullptr) };
	std::wstring buffer(bufferSize, L'\0');
	if (!::GetLogicalDriveStrings(bufferSize, buffer.data()))
	{
		ImGui::EndChild();
		return;
	}

	std::vector<std::filesystem::path> drives;
	while (true)
	{
		size_t pos{ buffer.find(L'\0') };
		if (pos == 0 || pos == std::wstring::npos)
			break;

		drives.push_back(buffer.substr(0, pos));
		buffer.erase(0, pos + 1);
	}

	for (int i{ 0 }; const auto& drive : drives)
	{
		ImGui::PushID(i++);
		std::function<void(const std::filesystem::path&)> recrusive = [this, &recrusive](const std::filesystem::path& path)
		{
			static const auto folderIcon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/Folder.png") };
			static const auto fileIcon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/File.png") };

			const bool isDirectory{ std::filesystem::is_directory(path) };
			const auto icon{ isDirectory ? folderIcon : fileIcon };
			const std::string label{ (path == path.root_path()) ? Util::u8stou8s(path.root_name().u8string()) : Util::u8stou8s(path.filename().u8string()) };
			const ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_SpanFullWidth | (isDirectory ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf) };

			const auto [isOpened, isPressed] { IconTreeNode(icon, label, flags) };
			if (isDirectory)
			{
				if (isOpened)
				{
					for (const auto& entry : std::filesystem::directory_iterator{ path, std::filesystem::directory_options::skip_permission_denied })
					{
						if (entry.is_directory())
							recrusive(entry);
					}
					for (const auto& entry : std::filesystem::directory_iterator{ path, std::filesystem::directory_options::skip_permission_denied })
					{
						if (entry.is_regular_file())
							recrusive(entry);
					}
					ImGui::TreePop();
				}
				if (isPressed)
				{
					SetPath(path);
				}
			}
			else
			{
				if (isOpened)
				{
					ImGui::TreePop();
				}
				if (isPressed)
				{
					auto log{ std::format(L"File: {}\n", path.wstring()) };
					::OutputDebugString(log.c_str());
				}
			}
		};
		recrusive(drive);
		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();
}

void Explorer::RenderAddressBar()
{
	if (!ImGui::BeginChild("AddressBar", ImVec2{ 0.0f, 23.0f }))
	{
		ImGui::EndChild();
		return;
	}

	// 디스크 드라이브 콤보 박스
	ImGui::SetNextItemWidth(50.0f);
	if (ImGui::BeginCombo("##DiskDrive", reinterpret_cast<const char*>(m_path.root_name().u8string().c_str())))
	{
		DWORD bufferSize{ ::GetLogicalDriveStrings(0, nullptr) };
		std::wstring buffer(bufferSize, L'\0');
		if (!::GetLogicalDriveStrings(bufferSize, buffer.data()))
		{
			ImGui::EndChild();
			return;
		}

		std::vector<std::filesystem::path> drives;
		while (true)
		{
			size_t pos{ buffer.find(L'\0') };
			if (pos == 0 || pos == std::wstring::npos)
				break;

			drives.push_back(buffer.substr(0, pos));
			buffer.erase(0, pos + 1);
		}

		for (const auto& drive : drives)
		{
			if (ImGui::Selectable(reinterpret_cast<const char*>(drive.root_name().u8string().c_str())))
			{
				SetPath(drive);
				break;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 3.0f, ImGui::GetStyle().ItemSpacing.y });
	std::filesystem::path newPath;
	std::filesystem::path temp{ m_path.root_path() };
	for (const auto& entry : m_path)
	{
		if (entry.has_root_name() || entry.has_root_directory())
			continue;

		ImGui::PushID(reinterpret_cast<const char*>(temp.u8string().c_str()));
		temp /= entry;

		ImGui::SameLine();
		if (ImGui::Button(entry))
		{
			newPath = temp;
			ImGui::PopID();
			break;
		}

		// 하위 폴더가 있으면 ">" 버튼 표시
		bool hasSubfolder{ false };
		std::vector<std::filesystem::path> subfolders;
		for (const auto& e : std::filesystem::directory_iterator{ temp, std::filesystem::directory_options::skip_permission_denied })
		{
			if (!e.is_directory())
				continue;
			hasSubfolder = true;
			subfolders.push_back(e);
		}
		if (hasSubfolder)
		{
			ImGui::SameLine();
			if (ImGui::Button(">"))
				ImGui::OpenPopup("FolderList");
			if (ImGui::BeginPopup("FolderList"))
			{
				for (const auto& e : subfolders)
				{
					if (ImGui::Selectable(e.filename().u8string()))
					{
						newPath = e;
						break;
					}
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopID();
	}
	ImGui::PopStyleVar();

	if (!newPath.empty())
		SetPath(newPath);

	// 경로가 바뀌지 않았을 때 갱신해야 제대로 작동함
	if (newPath.empty() && m_scrollAddressBarToRight)
	{
		ImGui::SetScrollHereX(1.0f);
		m_scrollAddressBarToRight = false;
	}
	ImGui::EndChild();
}

void Explorer::RenderFileViewer()
{
	if (!ImGui::BeginChild("FILE_VIEWER", ImVec2{}, false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::EndChild();
		return;
	}

	// 폴더
	for (const auto& entry : std::filesystem::directory_iterator{ m_path, std::filesystem::directory_options::skip_permission_denied })
	{
		if (!entry.is_directory())
			continue;

		static const auto icon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/Folder.png") };
		std::wstring name{ entry.path().filename() };
		if (IconButton(icon, Util::wstou8s(name)))
			SetPath(std::filesystem::canonical(m_path / name));
		ImGui::SameLine();
	}

	// 파일
	for (const auto& entry : std::filesystem::directory_iterator{ m_path, std::filesystem::directory_options::skip_permission_denied })
	{
		if (!entry.is_regular_file())
			continue;

		if (entry.path().extension() != Stringtable::DATA_FILE_EXT)
			continue;

		static const auto icon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/File.png") };
		std::string name{ Util::u8stou8s(entry.path().filename().u8string()) };
		IconButton(icon, name);
		if (ImGui::BeginDragDropSource())
		{
			std::wstring fullPath{ entry.path() };
			fullPath.push_back(L'\0');
			ImGui::SetDragDropPayload("EXPLORER/OPENFILE", fullPath.data(), fullPath.size() * sizeof(std::wstring::value_type));
			ImGui::Text(name.c_str());
			ImGui::EndDragDropSource();
		}
	}
	ImGui::EndChild();
}

void Explorer::SetPath(const std::filesystem::path& path)
{
	m_path = path;
	m_scrollAddressBarToRight = true;
}
