@echo off
cd /d "%~dp0"
chcp 65001 >nul

echo === Что изменилось: ===
git status --short
echo.

set /p MSG=Сообщение коммита (Enter = отмена): 
if "%MSG%"=="" (
    echo Отменено.
    pause
    exit /b
)

git add .
git commit -m "%MSG%"
git push
echo.
echo === Готово ===
pause