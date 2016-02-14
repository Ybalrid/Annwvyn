Annwvyn
=======

Annwvyn Engine is a simple game engine built upon free and open source technologies to easily create applications and games using the Oculus Rift Headset.

Please read the DEPEDENCIES file to know what you need to build it.


System requirement
----------------

 - A fairly recent GPU. Support for OpenGL 4.3 is mandatory
 - Oculus VR Headset (Rift) Dev Kit 2, DK1 will run too but you'll not have positional tracking (since it can't do it)
 - 64bit Operating system
 - Oculus Runtime 

Building on Windows
-------------------

As stated in the DEPEDENCIES file, you should download the SDK from http://annwvyn.org/
Please install Runtime 0.8 and the lattest drivers of your graphics card manufacturer.


Building on Linux (experimental) (unsuported by oculus. This use a fairly outdated SDK...)
-----------------

Install the libraries as stated on the DEPENDENCIES file. Download the OculusSDK and uncompress it and rename it "OculusSDK"on the parent folder of Annwvyn.

Install the udev rules before plugin the headset.

Configure the rift as a rotated 2nd screen. You can use xrandr like so:

```
xrandr --output DVI-I-1 --pos 1920x0 --mode 1080x1920 --rate 75 --rotate left
```

You may need to change the output name to mach your config. (See xrandr man page)

Then just run the following commands:
```
make
make test
sudo make install
```

If you are running the 0.4.4 version of the rift SDK, you have to know that there is a bug crashin the dk at initialization of the IR camera. The quick fix is to restart the ovcvideo module with the parameter quirks=0

```
sudo rmmod uvcvideo
sudo modprobe ovcvideo quirks=0
```

Using the engine
----------------

Make a copy of the "template" directory to a convinient location for you, and you are ready to go. It conains a Visual Studio project allready configured.

**Make sure to put it in release/x64 mode before building it!**

On windows you need to have a environement variable "AnnwvynSDK" set to the SDK location. This is done automaticly by the SDK installer, but if you encounter problem, you should run again the RUNME.bat script on the SDK installation folder


Updating the engine
-------------------

Run "git pull" on the Annwvyn sub-directory. then open the VS solution in msvc/Annwvyn/Annwyvn.sln

Clean and build the solution.

If you have problems regarding the Oculus SDK, you probably need to install new runtime, then download teh newest SDK and put it in place of the older one. Check commit log to see if there is mention of SDK update.

______

If you want more information about the project, please consult the official website (http://annwvyn.org/). It aslo contains documentation about the code.

This project is in it's realy early phases of developpement. If you have any question, feel free to contact me.

Licence MIT.
