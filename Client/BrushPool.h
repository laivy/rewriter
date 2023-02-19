#pragma once

enum class BrushType
{
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
};

class BrushPool : public TSingleton<BrushPool>
{
public:
	BrushPool(const ComPtr<ID2D1DeviceContext2>& renderTarget);
	~BrushPool() = default;

	ComPtr<ID2D1SolidColorBrush> GetBrush(BrushType type);

private:
	std::unordered_map<int, ComPtr<ID2D1SolidColorBrush>> m_pool;
};