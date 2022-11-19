# 개요
Direct2D를 이용하여 2D 플랫포머 멀티 게임을 제작한다.
# 코딩 컨벤션
아래 작성되어있는 사항을 제외하고는 기본적으로 C++ 구글 스타일 가이드를 따른다. 
## 파일명
모든 `.cpp`, `.h` 파일은 `UpperCamelCase` 를 따른다.
## 변수명
클래스의 멤버 변수의 경우는 `m_` 으로 시작하고 `lowerCamelCase` 를 따른다.
## 함수명
`UpperCamelCase` 를 따른다. 만약 `BOOL m_isValid` 과 같이 `BOOL` 자료형의 클래스 멤버 변수의 게터, 세터를 만들 경우 `IsValid`, `SetValid` 처럼 이름짓는다.
# 각 클래스 별 설명
## class GameObject
화면에 그려지는 모든 객체는 `class GameObject` 를 상속받는다. 이 클래스는 멤버 변수와 함수가 있지만 `인터페이스` 라고 생각해도 무방하다. 하위 클래스에서 `Update`, `Render` 함수를 오버라이딩해서 사용한다.
## class UI
위의 `class GameObject` 를 상속받는 클래스로 UI를 대표하는 클래스이다. 이 클래스도 멤버 변수와 함수가 있지만 `인터페이스` 라고 생각해도 무방하다. 모든 UI 객체는 `class UI` 를 상속받는다.
# 좌표계
윈도우 좌표계와 동일하게 좌측 상단을 (0, 0) 으로 취급한다. UI 객체의 마우스 이벤트의 경우 `class WndManager` 가 자신이 관리하고 있는 `class Wnd` 객체들에게 현재 마우스 좌표를 각 `class Wnd` 좌표계로 변경해서 `OnMouseEvent` 함수의 파라미터로 넘겨준다. 따라서 `Wnd::OnMouseEvent` 함수에서는 현재 이 윈도우가 어디에 위치해있는지 상관없이 필요하다면 하드코딩 할 수 있게 설계했다.