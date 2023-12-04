#pragma once
#include "Common/Singleton.h"

namespace Hierarchy
{
	class Node;
}

namespace Inspector
{
	class Window : public TSingleton<Window>
	{
	public:
		Window();
		~Window() = default;

		void Render();

		// 하이라키 윈도우에서 노드가 선택될 때 선택된 노드를 전달받는다.
		void OnNodeSelected(Hierarchy::Node* node);

	private:
		static constexpr auto WINDOW_NAME{ "Inspector" };
		static constexpr auto BUFFER_SIZE{ 20 + 1 };
		Hierarchy::Node* m_node;
		std::array<char, BUFFER_SIZE> m_name;
		Resource::Property::Type m_type;
	};
}