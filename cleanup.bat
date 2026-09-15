@echo off
cd /d "%~dp0"
del /q "Source\KazakhKhanate\MyMyCharacter.h"
del /q "Source\KazakhKhanate\MyMyCharacter.cpp"
rmdir /s /q Binaries
rmdir /s /q Intermediate
del /q KazakhKhanate.sln
echo Done. Now right-click .uproject -^> Generate Visual Studio project files
pause