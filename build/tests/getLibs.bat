@ECHO off
CLS
@ECHO Fetching runtime libraries

@ECHO Location of the Annwvyn SDK : %ANNWVYNSDK64%
@ECHO If this doesn't look right, you should go to the intended folder and run the "RUNME.bat" script ;-)
@ECHO .

SET Annwvyn=%ANNWVYNSDK64%\Annwvyn\

ECHO ANNWYVN main DLL
COPY %Annwvyn%\lib\Annwvyn.dll .
COPY %Annwvyn%\lib\Annwvyn_d.dll .
COPY %Annwvyn%\lib\Annwvyn_d.pdb .

ECHO ANNWYVN Oculus Renderer DLL
COPY %Annwvyn%\lib\AnnOgreOculusRenderer.dll .
COPY %Annwvyn%\lib\AnnOgreOculusRenderer_d.dll .
COPY %Annwvyn%\lib\AnnOgreOculusRenderer_d.pdb .

ECHO ANNWYVN OpenVR Renderer DLL
COPY %Annwvyn%\lib\AnnOgreOpenVRRenderer.dll .
COPY %Annwvyn%\lib\AnnOgreOpenVRRenderer_d.dll .
COPY %Annwvyn%\lib\AnnOgreOpenVRRenderer_d.pdb .

ECHO OGRE main DLL
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreMain.dll .
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\debug\OgreMain_d.dll .

ECHO OGRE OpenGL 3+ Rendering Subsystem
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\RenderSystem_GL3Plus.dll .
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\debug\RenderSystem_GL3Plus_d.dll .

ECHO OGRE Overlay component
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreOverlay.dll .
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\debug\OgreOverlay_d.dll .

ECHO OGRE Hlms Pbs
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreHlmsPbs.dll .
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\debug\OgreHlmsPbs_d.dll .

ECHO OGRE Hlms Unlit
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\debug\OgreHlmsUnlit_d.dll .
COPY %ANNWVYNSDK64%\Ogre21SDK\bin\release\OgreHlmsUnlit.dll .

ECHO Ogre_glTF plugin
COPY %ANNWVYNSDK64%\Ogre_glTF\build\output\bin\Ogre_glTF.dll .
COPY %ANNWVYNSDK64%\Ogre_glTF\build\output\bin\Ogre_glTF_d.dll .

ECHO ogre-procedural runtime DLL
copy %ANNWVYNSDK64%\ogre-procedural\bin\Release\OgreProcedural.dll .
copy %ANNWVYNSDK64%\ogre-procedural\bin\Debug\OgreProcedural_d.dll .

ECHO GL Extension Wrangler DLL
COPY %ANNWVYNSDK64%\glew\bin\Release\x64\glew32.dll .

ECHO OpenAL runtime DLL 
COPY %ANNWVYNSDK64%\AL\openal-soft\build\install\bin\OpenAL32.dll .

ECHO LibSndFile DLL
COPY %ANNWVYNSDK64%\libsndfile\bin\libsndfile-1.dll

ECHO Object-oriented Input System dll
COPY %ANNWVYNSDK64%\OIS\OIS_ReleaseDll\OIS.dll .
ECHO Object-oriented Input System dll
COPY %ANNWVYNSDK64%\OIS\OIS_DebugDll\OIS_d.dll .

ECHO OpenVR api - runtime DLL
COPY %ANNWVYNSDK64%\openvr\bin\win64\openvr_api.dll . 

@ECHO done 

