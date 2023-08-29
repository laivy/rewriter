#include "Stdafx.h"
#include "Button.h"
#include "EditCtrl.h"
#include "LoginWnd.h"
#include "LoginScene.h"
#include "GameScene.h"
#include "SceneManager.h"

LoginWnd::LoginWnd(const INT2& size) : Wnd{ size }
{
	auto editCtrl{ std::make_unique<EditCtrl>(INT2{ 200, 20 }) };
	editCtrl->SetParent(this);
	editCtrl->SetPosition({ size.x / 2, size.y / 2 });
	AddUI(editCtrl.release());

	auto button{ std::make_unique<Button>(INT2{ 200, 20 }) };
	button->SetPosition({ size.x / 2, size.y / 2 + 30 });
	button->SetButtonID(1);
	AddUI(button.release());
}

void LoginWnd::OnButtonClick(ButtonID id)
{
	switch (id)
	{
	case 1:
	{
		SceneManager::GetInstance()->SetFadeOut(0.5f,
		[]()
		{
			if (!GameScene::IsInstanced())
				GameScene::Instantiate();
			auto sm{ SceneManager::GetInstance() };
			sm->SetScene(GameScene::GetInstance());
			sm->SetFadeIn(0.5f);
			LoginScene::Destroy();
		});
		break;
	}
	default:
		break;
	}
}