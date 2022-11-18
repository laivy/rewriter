#pragma once

class GameObject abstract
{
public:
	GameObject();
	virtual ~GameObject() = default;
	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const;

	void SetPosition(const FLOAT2& position);

	INT GetId() const;
	FLOAT2 GetPosition() const;

protected:
	FLOAT2 m_position;

private:
	static INT s_id;
	INT m_id;
};