#pragma once

class BrushPool : public TSingleton<BrushPool>
{
public:
	enum Type
	{
		BLACK, WHITE, RED, GREEN, BLUE
	};

public:
	BrushPool();
	~BrushPool() = default;

	ID2D1SolidColorBrush* GetBrush(Type type) const;

private:
	std::unordered_map<int, ComPtr<ID2D1SolidColorBrush>> m_pool;
};