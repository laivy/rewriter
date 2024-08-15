<div align="center">
	<h1>Rewriter✏️</h1>
	<h3>내가 원하는 대로 다시 써가는 이야기</h3>
	<p>2D 횡스크롤 플랫포머 기반의 도트 감성 RPG</p>
</div>

# ⚙️ 빌드 및 실행
<img src="https://img.shields.io/badge/Visual%20Studio%202022-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white">
<img src="https://img.shields.io/badge/c++%2020-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white">
<img src="https://img.shields.io/badge/DirectX12-0078D6?style=for-the-badge&logo=windows&logoColor=white">
<img src="https://img.shields.io/badge/Direct2D-0078D6?style=for-the-badge&logo=windows&logoColor=white">

1. `Rewriter.sln` 를 Visual Studio 2022로 엶
2. 원하는 프로젝트와 구성을 선택
	- ex. `Debug_Client` 구성으로 `Client` 프로젝트 선택
3. 빌드
4. `Init.bat` 파일을 관리자 권한으로 실행
	- ex. `Bin/Debug_Client/Data` 폴더가 생성됨
5. `Bin` 폴더 아래에 있는 바이너리 파일 실행


# 📁 프로젝트 폴더 구조
## Bin/
빌드 결과로 생성된 바이너리 파일들이 위치하는 폴더이다. 빌드 구성에 따라 다음과 같이 6가지 폴더로 나뉘어서 저장된다.
`Debug_Client`, `Debug_Server`, `Debug_Tool`, `Release_Client`, `Release_Server`, `Release_Tool`

## Common/
하나 이상의 프로젝트에서 같이 쓰는 소스 코드가 들어있는 폴더이다. 주의할 것은 특정 프로젝트에만 포함되어 있는게 아니기 때문에 헤더 파일에 주의해야 한다.
`Common/Stdafx.h` 에 포함되어 있는 헤더가 아니라면 직접 `include` 해서 사용해야 한다.

## Data/
클라이언트, 서버 구동에 필요한 데이터 파일이 모여있는 폴더이다.
이 폴더를 `Init.bat` 파일을 통해 심볼릭 링크를 만들어서 `Bin` 폴더에서도 이 폴더에 접근할 수 있도록 한다.

## External/
외부 라이브러리 등이 모여있는 폴더이다.

## Game/
클라이언트, 서버 프로젝트들이 모여있는 폴더이다.

## Module/
클라이언트, 서버, 툴에서 사용하는 `dll` 프로젝트가 모여있는 폴더이다.
외부 프로젝트에서 `dll` 프로젝트를 포함하여 빌드할 때 필요한 헤더 파일은 `Include/Lib.h` 하나로 통일한다.

## Tool/
개발에 필요한 툴 프로젝트들이 모여있는 폴더이다.
