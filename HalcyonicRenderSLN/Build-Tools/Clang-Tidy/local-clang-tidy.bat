SET mypath=%~dp0
powershell -executionpolicy remotesigned -command "%mypath:~0,-1%\run-clang-tidy.ps1 -dir %mypath:~0,-1%\..\.. -tidy-wae -continue -tidy ""-*,modernize-*"""
pause