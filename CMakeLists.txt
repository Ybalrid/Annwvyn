cmake_minimum_required(VERSION 2.8.0)


project(Annwvyn)

#Annwvyn SDK check
if(NOT DEFINED ENV{AnnwvynSDK64})
    message(FATAL_ERROR "You need to install the Annwvyn SDK")
else()
    message("Annwvyn SDK installed at " $ENV{AnnwvynSDK64})
endif()

if(NOT MSVC)
    message(FATAL_ERROR "You should only try to generate a VS solution")
endif()

#includes from old visual studio solution
#..\..\..\glfw-3.2.1.bin.WIN64\include;..\..\..\AL\openal-soft\include\AL;..\..\..\libsndfile\include;..\..\..\AL\openal-soft\OpenAL32\Include;..\..\..\OculusSDK\LibOVR\Src;..\..\..\OculusSDK\3rdParty\glext\GL;..\..\..\OculusSDK\3rdParty\glext;..\..\..\OculusSDK\LibOVRKernel\Src;..\..\..\OculusSDK\LibOVR\Include;..\..\..\bullet3\src;..\..\..\Ogre21SDK\include\OGRE;..\..\..\OIS\include;..\..\..\glew\include;..\..\..\glew\include\GL;..\..\pch;..\..\..\openvr\headers;..\..\..\chaiscript;..\..\..\tinyxml2\;..\..\..\BtOgre21\include\;

#link directory form old VS solution
#..\..\..\glfw-3.2.1.bin.WIN64\lib-vc2015\;..\..\..\AL\openal-soft\build\Release;..\..\..\bullet3\lib\Release;..\..\..\bullet3\lib;..\..\..\libsndfile\lib;..\..\..\OIS\OIS_ReleaseDll;..\..\..\Ogre21SDK\lib\Release\opt;..\..\..\Ogre21SDK\lib\Release;..\..\..\OculusSDK\LibOVR\Lib\Windows\x64\Release\VS2015\;..\..\..\glew\lib\Release\x64;..\..\..\openvr\lib\win64;..\..\..\tinyxml2\tinyxml2\bin\tinyxml2\x64-Release-Lib\;..\..\..\BtOgre21\build\Release;

#linker input from old VS solution
#glfw3.lib;opengl32.lib;glew32.lib;ws2_32.lib;OgreMain.lib;OgreOverlay.lib;OgreHlmsPbs.lib;OgreHlmsUnlit.lib;OIS.lib;Setupapi.lib;LibOVR.lib;openvr_api.lib;Winmm.lib;BulletCollision.lib;BulletDynamics.lib;LinearMath.lib;OpenAL32.lib;libsndfile-1.lib;tinyxml2.lib;BtOgre21.lib;


