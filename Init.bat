@ECHO OFF

SET ROOT=%~dp0

FOR %%p IN (Debug_Client, Debug_Server, Debug_Tool, Release_Client, Release_Server, Release_Tool) DO (
	:: 기존 링크 삭제
	IF EXIST %ROOT%\Bin\%%p\Data (
		RMDIR /S /Q %ROOT%\Bin\%%p\Data
	)

	:: 링크
	MKLINK /D %ROOT%\Bin\%%p\Data %ROOT%\Data
)

PAUSE