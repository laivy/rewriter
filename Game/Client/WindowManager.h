#pragma once

class IModal;
class IWindow;

class WindowManager : 
	public IObserver,
	public TSingleton<WindowManager>
{
public:
	WindowManager();
	~WindowManager() = default;

	void Update(float deltaTime);
	void Render() const;

	void Register(std::unique_ptr<IModal> modal);
	void Register(std::unique_ptr<IWindow> window);

private:
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseEvent(UINT message, int x, int y);

private:
	std::vector<std::unique_ptr<IModal>> m_modals;
	std::vector<std::unique_ptr<IWindow>> m_windows;
	IWindow* m_focusWindow;
};