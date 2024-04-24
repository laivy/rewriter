#pragma once

class IObject abstract
{
public:
	virtual void Update(float deltaTime) = 0;
	virtual void Render() const = 0;
};

class IObject2D abstract : public IObject
{
public:
	virtual void Update(float deltaTime) override = 0;
	virtual void Render() const override = 0;

protected:
	int m_layer; // 클 수록 위에 그려짐
};

class IObject3D abstract : public IObject
{
public:
	virtual void Update(float deltaTime) override = 0;
	virtual void Render() const override = 0;

protected:
	int m_layer; // 클 수록 위에 그려짐
};