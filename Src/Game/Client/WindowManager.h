#pragma once

class IModal;
class IWindow;

class WindowManager :
	public IObserver,
	public Singleton<WindowManager>
{
public:
	WindowManager();
	~WindowManager() = default;

	void Update(float deltaTime);
	void Render() const;

	void Register(const std::shared_ptr<IModal>& modal);
	void Register(const std::shared_ptr<IWindow>& window);

private:
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseEvent(UINT message, int x, int y);

	void UpdateMouseOverWindow(int x, int y);
	void UpdateFocusWindow(int x, int y);

private:
	std::vector<std::shared_ptr<IModal>> m_modals;
	std::vector<std::shared_ptr<IWindow>> m_windows;
	std::weak_ptr<IWindow> m_mouseOverWindow;
	std::weak_ptr<IWindow> m_focusWindow;
};
