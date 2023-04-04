# 개요
Direct2D를 이용하여 2D 플랫포머 멀티 게임을 제작한다.

# 코딩 컨벤션
아래 작성되어있는 사항을 제외하고는 기본적으로 C++ 구글 스타일 가이드를 따른다.

## 1. 이름 규칙
### 파일명
- 모든 `.cpp`, `.h` 파일명은 `UpperCamelCase` 를 따른다.
	- ex. `NytApp.cpp`
### 클래스명
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
- 원시 자료형은 되도록 대문자 자료형을 사용한다.
	- ex. `INT`, `FLOAT`

## 3. 프로젝트 클래스 규칙
### class IScene
- `class IScene` 을 상속받아 정의한 클래스들의 생성자와 소멸자에서는 복잡한 일을 해서는 안된다.
	- 해당 객체가 생성, 소멸 될 때 해야할 복잡한 일은 `OnCreate`, `OnDestory` 에 작성한다.
	- 생성자에서 하는 일은 변수들의 초기값 설정 정도가 적합하다.