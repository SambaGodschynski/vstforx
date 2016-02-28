setlocal EnableDelayedExpansion
SET CLIBS=/Users/samba/clibs
SET SAMBA_USELOG=0
SET NEWEST=true
SET SAMBAG_BRANCH=master
SET VSTFORX_BRANCH=DISCO
SET SSH_USER=samba
SET SSH_SERVER=johanness-mini.fritz.box
SET SAMBAG_REMOTE_LOC=!%SSH_USER%@%SSH_SERVER%:workspace/sambag!
SET VSTFORX_REMOTE_LOC=!%SSH_USER%@%SSH_SERVER%:workspace/vstforx!
SET TARGET=!%SSH_USER%@%SSH_SERVER%:owncloud/nightly!

call "%VS90COMNTOOLS%\vsvars32.bat"

REM Prepare sambag
cd ..\sambag

IF NOT "%NEWEST%" == "true" GOTO END
git stash
git checkout %SAMBAG_BRANCH% %SAMBAG_BRANCH%
git pull %SAMBAG_REMOTE_LOC%
del CMakeCache.txt
cd scripts
python cmakeWalker.py
cd ..
:END

REM build sambag
IF "%NEWEST%" == "true" (
  msbuild sambag.sln /t:Rebuild /p:Configuration=Release
) ELSE (
  msbuild sambag.sln /p:Configuration=Release
)

REM Prepare vstforx
cd ..\vstforx
IF NOT "%NEWEST%" == "true" GOTO END
git stash
git checkout %VSTFORX_BRANCH%
git pull %VSTFORX_REMOTE_LOC% %VSTFORX_BRANCH%
del CMakeCache.txt
cd scripts
python cmakeWalker.py
createResourcesMeta.bat
cd ..
:END

REM Build vstforx
IF "%NEWEST%" == "true" (
  msbuild VSTForx-%VSTFORX_BRANCH%.sln /t:Rebuild /p:Configuration=Release
) ELSE (
  msbuild VSTForx-%VSTFORX_BRANCH%.sln /p:Configuration=Release
)


cd builds
FOR /F %%x IN ('cat currVersion.txt | grep -o '[0-9A-Za-z.]*'') DO SET V=%%x

del -rf win/*

sh pack.sh win %V%

scp win/* %TARGET%

endlocal
