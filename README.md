Annwvyn
=======

Annwvyn Engine is a simple C++ game engine built upon free and open source technologies to easily create applications and games using the Oculus Rift Headset.

It uses Ogre for rendering to the Oculus Rift official runtime in OpenGL, Bullet for simulating physics and collisions and OpenAL for spatialized audio playback.

Please read the DEPEDENCIES file to know what you need to build it.


System requirement
----------------

 - A fairly recent GPU. Support for OpenGL 4.3 is mandatory
 - Graphics driver with "direct driver" for VR implemented. The lattest Nvidia/AMD driver available will do
 - Oculus VR Headset (Rift) CV1 or Dev Kit 2 
 - 64bit Operating system
 - Oculus Runtime 1.3.2 (4/20/16) installed from http://oculus.com/setup

Building on Windows
-------------------

As stated in the DEPEDENCIES file, you should download the SDK from http://annwvyn.org/
Please install Oculus Home and the lattest drivers of your graphics card manufacturer.


Using the engine
----------------

Make a copy of the "template" directory to a convinient location for you, and you are ready to go. It conains a Visual Studio (Game.sln) project allready configured.

**Make sure to put it in release/x64 mode before building it!**

On windows you need to have a environement variable "AnnwvynSDK64" set to the SDK location. This is done automaticly by the SDK installer, but if you encounter problem, you should run again the RUNME.bat script on the SDK installation folder


Updating the engine
-------------------

Run "git pull" on the Annwvyn sub-directory. then open the VS solution in msvc/Annwvyn/Annwyvn.sln

Clean and build the solution.

If you have problems regarding the Oculus SDK, you probably need to install new runtime, then download the newest SDK and put it in place of the older one. Check commit log to see if there is mention of SDK update.

______

If you want more information about the project, please consult the official website (http://annwvyn.org/). It aslo contains documentation about the code.

This project is in it's realy early phases of developpement. If you have any question, feel free to contact me directly at ybalrid@cecmu.org 

Licence MIT.
