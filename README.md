<div align="center">
	<h1>Rewriter✏️</h1>
	<h3>내가 원하는 대로 다시 써가는 이야기</h3>
	<p>2D 횡스크롤 플랫포머 기반의 도트 감성 RPG</p>
</div>

# ⚙️ 빌드
1. 원하는 프리셋으로 프로젝트 구성
- `debug-client`: 클라이언트 디버그 빌드
- `release-client`: 클라이언트 릴리즈 빌드
- `debug-server`: 서버 디버그 빌드
- `release-server`: 서버 릴리즈 빌드
- `debug-tool`: 툴 디버그 빌드
- `release-tool`: 툴 릴리즈 빌드
```powershell
cmake --preset debug-tool
```

2. 빌드
```powershell
cmake --build Build/debug-tool
```

3. 실행
```powershell
.\Bin\Tool\Debug\Editor.exe
```

# 📁 프로젝트 폴더 구조
## Bin/
바이너리가 출력되는 폴더. 아래와 같이 바이너리 종류와 빌드 구성으로 나뉘어 저장됨.
```
Bin/
	Client/
		Debug/
		Release/
	Server/
		Debug/
		Release/
	Tool/
		Debug/
		Release/
```

## Data/
클라이언트, 서버, 툴 구동에 필요한 데이터 파일이 모여있는 폴더. 빌드 후 이벤트로 Bin 폴더 아래에 링크됨.

## Database/
데이터베이스 테이블 생성 쿼리와 SP 생성 쿼리가 모여있는 폴더

## External/
외부 라이브러리가 모여있는 폴더

## Src/
소스 코드가 모여있는 폴더

### Common/
하나 이상의 프로젝트에서 같이 쓰는 소스 코드가 들어있는 폴더

### Game/
클라이언트, 서버 프로젝트들이 모여있는 폴더

### Library/
클라이언트, 서버, 툴에서 사용하는 `dll` 프로젝트가 모여있는 폴더

### Tool/
개발에 필요한 툴들이 모여있는 폴더

# 📏 코딩 컨벤션
## 네이밍
| 항목 | 규칙 | 예시 | 비고 |
|--|--|--|--|
| 파일 | `PascalCase` | `App.h`, `App.cpp` | 헤더 파일은 `.h`, 소스 파일은 `.cpp` 확장자를 사용
| 타입 | `PascalCase` | `class App` | -
| 변수 | `camelCase` | `float deltaTime`, `Timer m_timer` | 클래스의 `private` 멤버 변수는 `m_`, 전역 변수는 `g_` 접두사를 붙임
| 상수 | `PascalCase` | `constexpr auto WindowHeight{ 1080 }` | `enum`, `enum class` 도 이와 동일
| 함수 | `PascalCase` | `Update`, `Render` | 람다 함수는 변수이므로 `camelCase` 를 따름
| 네임스페이스 | `PascalCase` | `namespace Graphics::D3D` | -
| 매크로 | `SCREAMING_SNAKE_CASE` | `#define RESOURCE_API __declspec(dllexport)` | -

## 헤더 파일 포함 규칙
### 순서
헤더 파일을 포함할 때는 아래 순서로 한다.
그 안에서의 순서는 가능하면 알파벳 순서로 한다.
1. C++ 표준 라이브러리
2. 외부 라이브러리(`Windows.h`, `d3d12.h`, `External/Imgui/imgui.h`)
3. 내부 라이브러리(`Src/Library`)
4. 프로젝트 헤더 파일(`App.h`)
### 큰 따옴표와 꺾쇠 괄호
큰 따옴표는 현재 파일 기준으로 포함할 때, 꺾쇠 괄호는 추가 포함 디렉토리를 기준으로 포함할 때 사용한다.
```cpp
// 꺾쇠 괄호를 사용하는 경우
#include <string>
#include <Windows.h>
#include <Common/Delegate.h>
#include <External/DirectX/d3dx12.h>
#include <Resource/Resource.h>

// 큰 따옴표를 사용하는 경우
#include "App.h" // 현재 디렉토리에 있는 파일은 큰 따옴표 사용
```
