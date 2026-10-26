@echo off
setlocal
cd /d "%~dp0"
if not exist ".venv\Scripts\python.exe" (
    python -m venv .venv
    if errorlevel 1 goto fail
)
.venv\Scripts\python.exe -c "import matplotlib, reportlab" >nul 2>&1
if errorlevel 1 (
    .venv\Scripts\python.exe -m pip install -r requirements.txt
    if errorlevel 1 goto fail
)
.venv\Scripts\python.exe main.py
if errorlevel 1 goto fail
exit /b 0
:fail
echo.
echo Nao foi possivel iniciar. Instale Python 3.11 ou superior com Tcl/Tk e acesso ao pip.
pause
exit /b 1
