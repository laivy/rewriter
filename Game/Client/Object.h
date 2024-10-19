#pragma once

enum class Pivot
{
	LEFTTOP, CENTERTOP, RIGHTTOP,
	LEFTCENTER, CENTER, RIGHTCENTER,
	LEFTBOT, CENTERBOT, RIGHTBOT
};

class IObject abstract
{
public:
	IObject() = default;
	virtual ~IObject() = default;

	virtual void Update(float deltaTime) = 0;
	virtual void Render() const = 0;
};

class IObject2D abstract : public IObject
{
public:
	void SetZ(int depth);

	int GetZ() const;

private:
	int m_z; // 클 수록 위에 그려짐
};

class IObject3D abstract : public IObject
{
};
