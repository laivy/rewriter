#pragma once

class Desktop : public TSingleton<Desktop>
{
public:
	Desktop() = default;
	~Desktop() = default;

	void Render();

private:
	void RenderMainMenuBar();
	void RenderMainDockSpace();
};
