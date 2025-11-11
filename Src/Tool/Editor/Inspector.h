#pragma once

class Inspector :
	public IDelegate::Listener,
	public Singleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaSeconds);
	void Render();

private:
	Resource::ID m_targetID;
};
