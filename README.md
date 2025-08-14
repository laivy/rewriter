<div align="center">
	<h1>Rewriter✏️</h1>
	<h3>내가 원하는 대로 다시 써가는 이야기</h3>
	<p>2D 횡스크롤 플랫포머 기반의 도트 감성 RPG</p>
</div>

# ⚙️ 빌드 및 실행
<div>
	<img src="https://img.shields.io/badge/Visual%20Studio%202022-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white"/>
	<img src="https://img.shields.io/badge/c++%2023-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"/>
	<img src="https://img.shields.io/badge/DirectX12-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>
	<img src="https://img.shields.io/badge/Direct2D-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>
</div>

1. `Rewriter.sln` 를 Visual Studio 2022로 엶
2. 원하는 구성 선택
	- ex. `Debug_Client`
3. 빌드
4. `Bin` 폴더 아래에 있는 바이너리 파일 실행
	- ex. `Bin/Debug_Client/Client.exe`

# 📁 프로젝트 폴더 구조
## Bin/
바이너리가 출력되는 폴더. 빌드 구성에 따라 다음과 같이 6가지 폴더로 나뉘어서 저장
`Debug_Client`, `Debug_Server`, `Debug_Tool`, `Release_Client`, `Release_Server`, `Release_Tool`

## Data/
클라이언트, 서버, 툴 구동에 필요한 데이터 파일이 모여있는 폴더. 빌드 후 이벤트로 실행되는 `Scripts/MakeDataSymlink.py` 스크립트를 통해 `Bin/XXX/Data` 폴더로 링크됨

## Database/
데이터베이스 테이블 생성 쿼리와 SP 생성 쿼리가 모여있는 폴더

## External/
외부 라이브러리가 모여있는 폴더

## Script/
빌드 할 때 실행되는 스크립트가 모여있는 폴더

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
| 변수 | `camelCase` | `float deltaTime`, `Timer m_timer` | 클래스 멤버 변수는 `m_` 접두사를 붙임
| 상수 | `PascalCase` | `constexpr auto WindowHeight{ 1080 }` | `enum`, `enum class` 도 이와 동일
| 함수 | `PascalCase` | `Update`, `Render` | 람다 함수는 변수이므로 `camelCase` 를 따름
| 네임스페이스 | `PascalCase` | `namespace Graphics::D3D` | -
| 매크로 | `SCREAMING_SNAKE_CASE` | `#define RESOURCE_API __declspec(dllexport)` | -
