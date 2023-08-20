# 개요
DirectX12, Direct2D를 이용하여 2D 플랫포머 멀티 게임을 제작한다.

# 코딩 컨벤션
아래 작성되어있는 사항을 제외하고는 기본적으로 C++ 구글 스타일 가이드를 따른다.

## 1. 이름 규칙
### 파일명
- 모든 `.cpp`, `.h` 파일명은 `UpperCamelCase` 를 따른다.
	- ex. `GameApp.cpp`
### 클래스명
- 기본적으로 `UpperCamelCase` 를 따른다.
- `abstract` 클래스는 앞에 `Interface` 를 뜻하는 `I` 를 붙힌다.
	- ex. `class IGameObject`
- 템플릿 클래스의 경우 앞에 `Template` 를 뜻하는 `T` 를 붙힌다.
	- ex. `class TSingleton`
### 변수명
- 짧은 범위에서 사용하는 변수는 약자를 사용해도 된다.
- 넓은 범위에서 사용하는 변수는 되도록 풀네임으로 한다.
- 클래스의 멤버 변수의 경우 `m_` 으로 시작하고 `lowerCamelCase` 를 따른다.
- 전역 변수의 경우 `g_` 로 시작하고 `lowerCamelCase` 를 따른다.
### 함수명
- 기본적으로 `UpperCamelCase` 를 따른다.
- `BOOL m_isValid` 과 같이 `BOOL` 자료형의 클래스 멤버 변수의 게터, 세터를 만들 경우 `IsValid`, `SetValid` 처럼 이름짓는다.

## 2. 자료형 규칙
- 프리미티브 자료형을 사용할 때는 되도록 대문자 자료형을 사용하지 않는다.
	- 나쁜 예) `INT`, `FLOAT`
	- 좋은 예) `int`, `float`

# 프로젝트 코딩 사양
프로젝트에 관련되어 있는 규칙에 대해 작성한다.

## class TSingleton
- `class TSingleton` 을 상속받는 클래스는 생성자와 소멸자에서 복잡한 일을 해서는 안된다.
	- 해당 객체가 생성, 소멸 될 때 해야할 복잡한 일은 `OnCreate`, `OnDestory` 에 작성한다.
	- 생성자에서 하는 일은 변수들의 초기값 설정 정도가 적절하다.

## 좌표
### 월드 좌표계
좌측 하단이 (0, 0), 우측 상단이 (width, height) 이다. 아래 항목들의 좌표들은 모두 이 좌표계 기준이다.
- 게임오브젝트
	- class IGameObject를 상속받는 모든 클래스는 이 항목에 해당한다.
	- GetPosition 함수를 통해 반환받는 좌표는 해당 게임오브젝트의 중앙이다.
### 화면 좌표계
좌측 상단이 (0, 0), 우측 하단이 (width, height) 이다. 아래 항목들의 좌표들은 모두 이 좌표계 기준이다.
- 윈도우
	- class Wnd를 상속받는 모든 클래스는 이 항목에 해당한다.
	- GetPosition 함수를 통해 반환받는 좌표는 해당 윈도우의 중앙이다.
- UI
	- class Wnd를 상속받는 모든 클래스를 제외하고 class IUserInterface를 상속받는 모든 클래스는 이 항목에 해당한다.
	- 이 UI를 갖고 있는 윈도우의 좌측 상단이 (0, 0), 우측 하단이 (width, height)인 좌표계이다.
	- GetPosition 함수를 통해 반환받는 좌표는 해당 UI의 중앙이다.