#pragma once

class UIEditor : public TSingleton<UIEditor>
{
public:
	void Render();

private:
	static constexpr auto WINDOW_NAME{ "UIEditor" };
};
