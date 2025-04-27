#pragma once

class Viewport : public TSingleton<Viewport>
{
public:
	Viewport() = default;
	~Viewport() = default;

	void Update(float deltaTime);
	void Render();

private:
	static constexpr auto WINDOW_NAME{ "Viewport" };
};
