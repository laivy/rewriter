@ECHO OFF

SET ROOT=%~dp0

IF EXIST %ROOT%\Bin\Debug\Data (
	RMDIR /S /Q %ROOT%\Bin\Debug\Data
)
MKLINK /D %ROOT%\Bin\Debug\Data %ROOT%\Data

IF EXIST %ROOT%\Bin\Release\Data (
	RMDIR /S /Q %ROOT%\Bin\Release\Data
)
MKLINK /D %ROOT%\Bin\Release\Data %ROOT%\Data

PAUSE