@ECHO OFF

SET FROM=%1
SET TO=%2
SET NAME=%3

:: 기존 링크 삭제
IF EXIST %TO%\%NAME% (
	RMDIR /S /Q %TO%\%NAME%
)

:: 링크
MKLINK /D %TO%\%NAME% %FROM%
