#pragma once

class Inspector :
	public IDelegate::Listener,
	public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaSeconds);
	void Render();

private:
	Resource::ID m_targetID;
};
