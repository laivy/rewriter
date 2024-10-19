#pragma once

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
	void SetZ(int z);

	int GetZ() const;

private:
	int m_z; // 클 수록 위에 그려짐
};

class IObject3D abstract : public IObject
{
};
