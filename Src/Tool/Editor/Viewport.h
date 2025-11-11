#pragma once

class Viewport : public Singleton<Viewport>
{
public:
	Viewport();
	~Viewport() = default;

	void Update(float deltaTime);
	void Render();

private:
	static constexpr auto WINDOW_NAME{ "Viewport" };
};
