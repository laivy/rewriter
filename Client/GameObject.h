#pragma once

enum class Pivot
{
	LEFTTOP,	CENTERTOP,	RIGHTTOP,
	LEFTCENTER,	CENTER,		RIGHTCENTER,
	LEFTBOT,	CENTERBOT,	RIGHTBOT
};

class GameObject abstract
{
public:

public:
	GameObject();
	virtual ~GameObject() = default;
	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const;
	virtual void Destroy();

	void SetPosition(const FLOAT2& position, Pivot pivot = Pivot::LEFTTOP);
	void SetPosition(FLOAT x, FLOAT y, Pivot pivot = Pivot::LEFTTOP);
	void SetSize(const FLOAT2& size);

	INT GetId() const;
	BOOL IsValid() const;
	FLOAT2 GetPosition() const;
	FLOAT2 GetSize() const;

protected:
	BOOL m_isValid;
	FLOAT2 m_position;
	FLOAT2 m_size;

private:
	static INT s_id;
	INT m_id;
};