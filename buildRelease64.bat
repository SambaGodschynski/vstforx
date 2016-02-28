ECHO OFF
SET CLIBS=/Users/samba/clibs
SET SAMBA_USELOG=0
SET WORKSPACE_LOC=c:\workspace64
SET CLIBS_LOC=%WORKSPACE_LOC%\clibs
SET CMAKE_TARGET="Visual Studio 9 2008 Win64"
SET SAMBAG_LOC=%WORKSPACE_LOC%\clibs\sambag
SET VSTFORX_LOC=%WORKSPACE_LOC%\VST\Apps\vstforx
SET NEWEST=true
SET SAMBAG_BRANCH=master
SET VSTFORX_BRANCH=DISCO
SET SSH_USER=samba
SET SSH_SERVER=johanness-mini.fritz.box
SET SAMBAG_REMOTE_LOC=%SSH_USER%@%SSH_SERVER%:workspace/sambag
SET VSTFORX_REMOTE_LOC=%SSH_USER%@%SSH_SERVER%:workspace/vstforx
SET TARGET=%SSH_USER%@%SSH_SERVER%:workspace/VSTForxBuilds

call "%VS90COMNTOOLS%\vsvars32.bat"

REM Prepare sambag
cd %SAMBAG_LOC%

IF NOT "%NEWEST%" == "true" GOTO END
git stash
git checkout %SAMBAG_BRANCH%
git pull %SAMBAG_REMOTE_LOC% %SAMBAG_BRANCH%
del CMakeCache.txt
cd scripts
python cmakeWalker.py
cd ..
:END

sh build.sh -i %CLIBS_LOC% -g %CMAKE_TARGET%

REM build sambag
IF "%NEWEST%" == "true" (
  msbuild sambag.sln /t:Rebuild /p:Configuration=Release
) ELSE (
  msbuild sambag.sln /p:Configuration=Release
)

REM Prepare vstforx
cd %VSTFORX_LOC%
IF NOT "%NEWEST%" == "true" GOTO END
git stash
git checkout %VSTFORX_BRANCH%
git pull %VSTFORX_REMOTE_LOC% %VSTFORX_BRANCH%
del CMakeCache.txt
cd scripts
python cmakeWalker.py
CALL createResourcesMeta.bat
cd ..
:END

sh build.sh -i %CLIBS_LOC% -g %CMAKE_TARGET%

REM Build vstforx
IF "%NEWEST%" == "true" (
  msbuild VSTForx-%VSTFORX_BRANCH%.sln /t:Rebuild /p:Configuration=Release
) ELSE (
  msbuild VSTForx-%VSTFORX_BRANCH%.sln /p:Configuration=Release
)


cd builds

del /F /Q win\*

sh packLastBuild.sh win

scp win/*.* %TARGET%

ECHO ON