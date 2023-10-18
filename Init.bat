@ECHO OFF

:: 프로젝트 폴더 경로로 설정해야 함
SET ROOT=C:\Dev\Rewriter

:: 서버에서도 쓰는 클라이언트 데이터 폴더들 설정
SET COMMON=

:: 클라이언트 데이터 폴더 링크
IF EXIST %ROOT%\Game\Client\Data (
	RMDIR /S /Q %ROOT%\Game\Client\Data
)
MKLINK /D %ROOT%\Game\Client\Data %ROOT%\Data

:: 서버 데이터 폴더에 서버에서 필요한 클라이언트 폴더 링크
FOR %%f IN (%COMMON%) DO (
	IF EXIST %ROOT%\DataSvr\%%f (
		RMDIR /S /Q %ROOT%\DataSvr\%%f
	)
	MKLINK /D %ROOT%\DataSvr\%%f %ROOT%\Data\%%f
)

:: 로그인 서버 데이터 폴더 링크
IF EXIST %ROOT%\Game\LoginServer\DataSvr (
	RMDIR /S /Q %ROOT%\Game\LoginServer\DataSvr
)
MKLINK /D %ROOT%\Game\LoginServer\DataSvr %ROOT%\DataSvr

PAUSE