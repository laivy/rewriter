#include "Stdafx.h"
#include "Button.h"
#include "EditCtrl.h"
#include "LoginWnd.h"
#include "LoginScene.h"
#include "GameScene.h"
#include "SceneManager.h"

LoginWnd::LoginWnd(FLOAT width, FLOAT height) : Wnd{ width, height }
{
	EditCtrl* editCtrl{ new EditCtrl{ 200.0f, 20.0f } };
	editCtrl->SetPosition(FLOAT2{ 150.0f, 135.0f }, Pivot::CENTER);
	AddUI(editCtrl);

	EditCtrl* editCtrl2{ new EditCtrl{ 200.0f, 20.0f } };
	editCtrl2->SetPosition(FLOAT2{ 150.0f, 165.0f }, Pivot::CENTER);
	AddUI(editCtrl2);

	Button* loginBtn{ new Button{ 200.0f, 20.0f } };
	loginBtn->SetPosition(FLOAT2{ width / 2.0f, height * 0.9f }, Pivot::CENTER);
	loginBtn->SetCallback(
		[]()
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
		});
	AddUI(loginBtn);
}