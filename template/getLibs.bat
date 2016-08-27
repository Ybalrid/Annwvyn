@ECHO off
CLS
@ECHO Fetching runtime libraries

@ECHO Location of the Annwvyn SDK : %ANNWVYNSDK64%
@ECHO If this doesn't look right, you should go to the intended folder and run the "RUNME.bat" script ;-)
@ECHO .

SET Annwvyn=%ANNWVYNSDK64%\Annwvyn\

ECHO ANNWYVN main DLL
COPY %Annwvyn%\lib\Annwvyn.dll .
ECHO OGRE main DLL
COPY %ANNWVYNSDK64%\OgreSDK\bin\release\OgreMain.dll .
ECHO OGRE OpenGL Rendering Subsystem
COPY %ANNWVYNSDK64%\OgreSDK\bin\release\RenderSystem_GL.dll .
ECHO OGRE Overlay component
COPY %ANNWVYNSDK64%\OgreSDK\bin\release\OgreOverlay.dll .
ECHO OGRE Plugin OctreeSceneManager
COPY %ANNWVYNSDK64%\OgreSDK\bin\release\Plugin_OctreeSceneManager.dll .
ECHO GL Extension Wrangler DLL
COPY %ANNWVYNSDK64%\glew\bin\Release\x64\glew32.dll .
ECHO OpenAL runtime DLL 
COPY %ANNWVYNSDK64%\AL\openal-soft\build\Release\OpenAL32.dll .
ECHO LibSndFile DLL
COPY %ANNWVYNSDK64%\libsndfile\bin\libsndfile-1.dll
ECHO Object-oriented Input System dll
COPY %ANNWVYNSDK64%\OIS\lib\OIS.dll .
ECHO OpenVR api - runtime DLL
COPY %ANNWVYNSDK64%\openvr\bin\win64\openvr_api.dll . 

@ECHO done 
PAUSE
