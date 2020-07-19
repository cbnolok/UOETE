"c:\Qt\5.15.0\mingw81_32\bin\windeployqt.exe" --force --compiler-runtime --no-translations --no-opengl-sw --no-system-d3d-compiler --no-quick-import --no-angle --no-webkit2 UOETE.exe
copy /Y "c:\Qt\tools\mingw810_32\bin\libstdc++-6.dll" .
copy /Y "c:\Qt\tools\mingw810_32\bin\libgcc_s_dw2-1.dll" .
REM copy /Y "c:\Qt\tools\mingw810_64\bin\libgcc_s_seh-1.dll" .
copy /Y "c:\Qt\tools\mingw810_32\bin\libwinpthread-1.dll" .

pause