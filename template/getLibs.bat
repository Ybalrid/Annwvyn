@ECHO off
CLS
@ECHO Fetching runtime libraries

@ECHO Location of the Annwvyn SDK : %ANNWVYNSDK64%
@ECHO If this doesn't look right, you should go to the intended folder and run the "RUNME.bat" script ;-)
@ECHO .

SET Annwvyn=%ANNWVYNSDK64%\Annwvyn\

ECHO ANNWYVN main DLL
COPY %Annwvyn%\lib\Annwvyn.dll .

ECHO ANNWYVN Oculus Renderer DLL
COPY %Annwvyn%\lib\AnnOgreOculusRenderer.dll .
ECHO ANNWYVN OpenVR Renderer DLL
COPY %Annwvyn%\lib\AnnOgreOpenVRRenderer.dll .
ECHO ANNWVYN OpenHMD Renderer DLL
COPY %Annwvyn%\lib\AnnOgreOpenHMDRenderer.dll .

ECHO OGRE main DLL
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreMain.dll .
ECHO OGRE OpenGL 3+ Rendering Subsystem
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\RenderSystem_GL3Plus.dll .
ECHO OGRE Overlay component
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreOverlay.dll .
ECHO OGRE Hlms Pbs
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreHlmsPbs.dll .
ECHO OGRE Hlms Unlit
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreHlmsUnlit.dll .
ECHO GL Extension Wrangler DLL
COPY %ANNWVYNSDK64%\glew\bin\Release\x64\glew32.dll .
ECHO OpenAL runtime DLL 
COPY %ANNWVYNSDK64%\AL\openal-soft\build\install\bin\OpenAL32.dll .
ECHO LibSndFile DLL
COPY %ANNWVYNSDK64%\libsndfile\bin\libsndfile-1.dll
ECHO Object-oriented Input System dll
COPY %ANNWVYNSDK64%\OIS\OIS_ReleaseDll\OIS.dll .
ECHO OpenVR api - runtime DLL
COPY %ANNWVYNSDK64%\openvr\bin\win64\openvr_api.dll . 

@ECHO done 

