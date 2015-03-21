Annwvyn
=======

Annwvyn Engine is a simple game engine built upon free and open source technologies to easily create applications and games using the Oculus Rift Headset.

Please read the DEPEDENCIES file to know what you need to build it.



Building on Windows
-------------------

As stated in the DEPEDENCIES file, please download the SDK from http://annwvyn.org/


Building on Linux (experimental)
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

If you want more information about the project, please consult the official website (http://annwvyn.org/). It aslo contains documentation about the code.

This project is in it's realy early phases of developpement. If you have any question, feel free to contact me.

Licence MIT.

