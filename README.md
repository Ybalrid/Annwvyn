# Annwvyn [![Build status](https://ci.appveyor.com/api/projects/status/dxbjamqjeuivkgkf?svg=true)](https://ci.appveyor.com/project/Ybalrid/annwvyn)

![Screnshot](AnnwvynScreenshot.png)

Annwvyn is a simple C++ game engine built upon free and open source technologies to easily create applications and games tailored for Virtual Reality HMD.

This project is licensed under the terms of the [MIT](LICENSE) licence agreement.

For any questions, shout me a mail at developer@annwvyn.org.

Currently the engine supports :
 - Oculus Rift through the Oculus SDK (OVR) *Windows Only*
 - OpenVR *Windows and Linux*
     - HTC Vive
     - Oculus Rift (You can use the Oculus Rift with SteamVR, but the "native" way is recommended)
     - Any headset officially supported by SteamVR

Annwyn uses the [OGRE](https://www.ogre3d.org/) rendering engine (version 2.1) in OpenGL mode, and send the rendered frames to the native SDKs of multiple types of VR systems. The engine simulate physics via [Bullet](http://bulletphysics.org), and generate spatial 3D audio via [OpenAL](https://www.openal.org/)

Please read the DEPENDENCIES file to know what you need to build it.

## System and Software requirement

 - A VR capable GPU. Support for OpenGL **4.3** is **required** for Annwvyn, and it needs to be compatible with your VR system
 - Oculus VR Headset (Rift) CV1 or Dev Kit 2 OR HTC Vive, or other hardware supported by OpenVR
 - Usable with hand controllers (touch, vive wands...) 
 - 64bit Operating system
 - Up to date runtime software: 
    - Oculus Home app installed from http://oculus.com/setup *Windows only*
    - SteamVR installed through Steam http://steampowered.com/ 
    - For Linux users, you will need to have Steam installed, the games will need to run within the Steam runtime, and you need to install Vulkan libraries for it to actually work.
 - Windows build : Microsoft Visual Studio 2017 Community (free to download and use from Microsoft)
 - Linux build : GCC or Clang, Steam and Steam VR installed, Vulkan developement package. 
 
Specifically, the code needs a C++14 compliant compiler, and the dependency package is built with the lattest version of Visual Studio 2017 available
 
## Installation

First, please install Visual Studio 2017 (the community version is free) from Microsoft.

Then, go get the SDK installer here https://www.annwvyn.org/download/ and run it. 
If near the end of the installer, a CMD window open asking about setting up environment variables, say yes.

The installer needs to set the location of Annwvyn into an environement variable to easilly find everything with CMake.

After that, if you need to update the engine to the current master branch, see the "Updating the engine" section below.

### Building on Windows

**Please install Oculus Home, or SteamVR and the lattest drivers from your graphics card manufacturer.**

As stated in the DEPEDENCIES file, you should download the SDK from https://annwvyn.org/
The SDK has everything to **build** Annwwvyn itself, so you can hack into it by this way to. It's a Source+Binary package.

But, if you *really* want to build and setup the engine yourself, here's a simple guide how to do it

- Download the "dependency package" from here https://www.annwvyn.org/download/
- Extract it in a folder
- Clone this repo in the same folder
- Run the `RUNME.BAT` script from this folder (found in the dependency package you extracted earlier)
- Generate in a "build" directirory a VS2017 x64 solution via CMake
- Make sure the IDE is set to build in "Release" "x64" mode
- hit Ctrl+Shift+B to build the whole solution
- In the solution explorer, right click on the `INSTALL` project, and build it.
- Do the same (without the `INSTALL` part) in the example subdirectory
- You need to copy a bunch of .dll in the "example" (and template) build directories. You can do this automatically by launching the `getLibs.bat` script *(you really should)*. But here's the list if you want to check
    - Ogre  
        - OgreMain.dll
        - OgreOverlay.dll 
        - OgreHlmsPbs.dll
        - OgreHlmsUnlit.dll
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

If you have an Oculus Rift plugged in (or in Debug HMD mode), or a Vive with SteamVR open you can launch the example program via the HMD_launcher.bat scripts in the example directory. (using the one that correspond to your hardware)

### Building on Linux

The only supported renderer on Linux is the OpenVR one. You should install Steam, and SteamVR on your box before doing anything.

You need to have the libraries described in the DEPENDENCIES file, and every libraries needed to build Ogre 2.1 with json, zip, and OpenGL support.

Everything regarding building Ogre, BtOgre21, Annwvyn, and Annwvyn powered projects is done via CMake.*

If you are with CMake, in a terminal, just navigate to a source directory that contains an `CMakeLists.txt` file and do the following:

```bash
#if a "build" directory doesn't exist, create one 
mkdir build
cd build
cmake ..
make 
#if you need to "install" the software
sudo make install
```

You will need to build Ogre yourself. Once you have Ogre 2.1 and Bullet Physics installed on your system, you need to build and install [BtOgre21](https://github.com/Ybalrid/BtOgre21).

Currently, some of the libraries that are either distributed in binary for only, and the CMakeScripts are available in this repository that needs to be clonned inside your home directory : https://github.com/Ybalrid/AnnwvynDeps

#### Some words on the linux support

Annwvyn works (has been tested) with the following configuration:
 - OpenVR renderer, linked to OpenVR 1.0.13 and an HTC vive, installed and configurerd [correctly](https://github.com/ValveSoftware/SteamVR-for-Linux)
 - Game ran throug the Steam VR [runtime](https://github.com/ValveSoftware/SteamVR-for-Linux#runtime-requirements)
 - With Vulkan libraries installed (On ArchLinux, you need the `vulkan-devel` metapackage. 

## Using the engine

Make a copy of the "template" directory to a convenient location for you, and you are basically ready to go. It contains a CMakeLists.txt that will permit you to generate build files.

Inside your build directory, you need to copy over some libraries, there are `getLibs.bat` for windows and `linuxGetLibs.sh` for linux scripts that will do that for you if everything was installed correctly.

**Make sure to put Visual Studio build in Release/x64 mode before building!** The debug mode target will work but you need ot copy over debug-compatible version of the libraries. They aren't packaged conviniently for that yet. (comming soon).

On windows you need to have a environment variable "AnnwvynSDK64" set to the SDK location. This is done automatically by the SDK installer, but if you encounter problem, you should run again the RUNME.bat script on the SDK installation folder.

You should check out the [quickstart](https://wiki.annwvyn.org/doku.php?id=quickstart) page on the Wiki


## Updating the engine

Run "git pull" on the Annwvyn sub-directory. then open the VS solution in msvc/Annwvyn/Annwyvn.sln
(You may also need to re-build Ogre v2-1 from sources and put it inside your dependency package...)

Clean and build the solution.

If you have build errors, you should download the newest SDK and put it in place of the older one. You can also replace the content of your SDK folder (except the Annwvyn directory) with the dependency package from https://www.annwvyn.org/download/

If you are getting error regarding functions with names that start's with "VR", you need to upgrade the OpenVR package. Just git pull from the OpenVR directory and copy over the dll.

If you are getting errors regarding functions with names that starts with "ovr_", you need to upgrade the OculusSDK. This require you to rebuild the libtray with /MD(d) runtime linkage, but I'll recomedn you to get the new dependency package.
______

If you want more information about the project, please consult the official website (https://annwvyn.org/). It also contains documentation about the code.

This project is in it's really early phases of development. If you have any question, feel free to contact me directly at ybalrid@ybalrid.info 

License MIT.

______

