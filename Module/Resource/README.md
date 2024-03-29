# Resource
## 1. 규칙
- `Stdafx.h`에는 `Common/Stdafx.h` 만 포함한다.
	- `Stdafx.h` 에 이 프로젝트에서만 사용하는 헤더 파일을 `include` 해서 사용할 경우, `Resource/Include` 폴더만 `include` 해서는 컴파일이 안될 수 있다.
	- 따라서 `Common/Stdafx.h` 외의 필요한 헤더 파일들은 직접 `include` 해서 사용한다.
- 모든 코드는 `namespace Resource` 안에 정의되어야한다.
	- 이 프로젝트는 다른 프로젝트에서 사용되어야함으로, 해당 프로젝트 코드임을 명확히 하기 위해 해당 `namespace Resource` 안에 정의해야한다.
	- `concept is_property_data_type_v` 은 다른 프로젝트에서도 쉽게 접근할 수 있도록 전역 스코프에 선언했다.