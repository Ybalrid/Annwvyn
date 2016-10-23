Annwvyn
=======

Annwvyn Engine is a simple C++ game engine built upon free and open source technologies to easily create applications and games using Virtual Reality HMD.
Freely distributed under the MIT license.

Currently the engine supports :
 - Oculus Rift through the Oculus SDK
 - OpenVR
     - HTC Vive
     - Oculus Rift (You can use the Oculus Rift with SteamVR, but the "native" way is recommended)
     - Any headset officially supported by SteamVR


It uses Ogre for rendering to OpenGL buffers that are given to each VR libraries, Bullet for simulating physics and collisions and OpenAL for spatialized audio playback.

Please read the DEPEDENCIES file to know what you need to build it.


System requirement
----------------

 - A fairly recent GPU. Support for OpenGL 4.3 is recommended. Tested on 4.0 GPU in "noVR" mode only
 - Graphics driver with "direct driver" for VR implemented. The lattes Nvidia/AMD driver available will do
 - Oculus VR Headset (Rift) CV1 or Dev Kit 2 OR HTC Vive
 - 64bit Operating system
 - Oculus Runtime 1.9 (10/20/16) installed from http://oculus.com/setup OR SteamVR installed through Steam http://steampowered.com/
 - Microsoft Visual Studio 2015 Community (free to download and use from Microsoft)
 
Installation
------------

First, please install Visual Studio 2015 (the community version is free) from Microsoft.

Then, go get the SDK installer here http://annwvyn.org/?static1/download and run it. 
If near the end of the installer, a CMD window open asking about setting up environment variables, say yes.

After that, if you need to update the engine to the current master branch, see the "Updating the engine" section below.

Building on Windows
-------------------

As stated in the DEPEDENCIES file, you should download the SDK from http://annwvyn.org/
Please install Oculus Home and the lattes drivers of your graphics card manufacturer.

But, if you really want to build the engine yourself, here's a simple guide how to do it

- Download the "dependency package" from here http://annwvyn.org/?static1/download
- Extract it in a folder
- Clone this repo in the same folder
- Run the "RUNME.BAT" script from this folder
- Open Annwvyn/msvc/Annwvyn/Annwvyn.sln with Visual studio
- Make sure the IDE is set to build in "release" "x64" mode
- hit Ctrl+Shift+B
- Open Annwvyn/msvc/test/test.sln
- hit Ctrl+Shift+B
- You need to copy a bunch of .dll in the "example" (and template) directories. You can do this automatically by launching the "getLibs.bat" scripts
    - Ogre  
        - OgreMain.dll
        - OgreOverlay.dll  
        - Plugin_OctreeSceneManager.dll
        - RenderSystem_GL.dll
        - OIS.dll
    - OpenAL
        - OpenAL32.dll
    - GLEW
        - glew32.dll        
    - sndfile
        - libsndfile-1.dll
    - OpenVR API client
        - openvr_api.dll

If you have an Oculus Rift plugged in (or in Debug HMD mode), or a Vive with SteamVR open you can launch example/test.exe now


Using the engine
----------------

Make a copy of the "template" directory to a convenient location for you, and you are ready to go. It contains a Visual Studio (Game.sln) project already configured.

**Make sure to put Visual Studio build in release/x64 mode before building!**

On windows you need to have a environment variable "AnnwvynSDK64" set to the SDK location. This is done automatically by the SDK installer, but if you encounter problem, you should run again the RUNME.bat script on the SDK installation folder


Updating the engine
-------------------

Run "git pull" on the Annwvyn sub-directory. then open the VS solution in msvc/Annwvyn/Annwyvn.sln

Clean and build the solution.

If you have build errors, you should download the newest SDK and put it in place of the older one. You can also replace the content of your SDK folder (except the Annwvyn directory) with the dependency package from http://annwvyn.org/?static1/download

______

If you want more information about the project, please consult the official website (http://annwvyn.org/). It also contains documentation about the code.

This project is in it's really early phases of development. If you have any question, feel free to contact me directly at ybalrid@ybalrid.info 

License MIT.
