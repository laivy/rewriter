#pragma once

class Desktop : public Singleton<Desktop>
{
private:
	struct Config
	{
		std::string theme;
		std::string fontName;
		float fontSize;
	};

public:
	Desktop();
	~Desktop();

	void Update(float deltaSeconds);
	void Render();

private:
	void SaveConfig();
	void LoadConfig();

	void SetTheme(std::string_view theme);
	void SetFontName(std::string_view fontName);
	void SetFontSize(float size);

	void MainMenuBar();

private:
	Config m_config;
};
