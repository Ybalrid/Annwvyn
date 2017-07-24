# CONTRIBUTING
This project is an experimental open-source game engine. As an open-source project, contributions are welcome. 

This is mostly code written by some random student in some random engineering graduiate school in some random country (called France), so don't expect this thing to be amazing, okay ? :p

The current goal is is to make a C++ VR content developement framework that permit to develop simple VR experiences that should work on most VR consumer accessible VR systems that plugs into a Personal Computer.

As an experimental project, the codebase try to use as much as it can C++11 and above features. But the roots of it were written at a time where I had no clue about a lot of things. Some things could be changed in the core design, but right now they are unimportant to worry about.

## Places on the internet

- There's a website that you can check out : https://annwvyn.org/
- There's an API reference website that you can check out too : https://api.annwvyn.org/
- And even a Wiki that you can also go browse : https://wiki.annwvyn.org/

## Feature request and/or bug reporting

Everything should be done through the GitHub repo's Issues tab.

## Contact me

Shoot me an email. ybalrid@ybalrid.info is fine for that. I'm also addicted to twitter, so you can mention [@Ybalrid](https://twitter.com/Ybalrid) there. 

## Contributing about what

Here's an example of the things that are more than welcome form anybody that would be interested into helping

- Fixin' the damn spelling. (I'm French, so my english is kinda bad sometimes, and I do a lot of typos)
- Making sure headers have proper Doxygen comments everywhere
- Writing other OgreVRRender child class for other VR systems (Looking at OSVR, if somebody knows their SDK well and has an HDK on hand, this would be super neat!)
- Writing the damn Wiki on https://wiki.annwvyn.org/
- Improving the build system. I havent took much time to look into how CMake actuall works. I just happened to make it spits out valid Visual Studio solutions and Linux Makefiles not too wory too much about it.
- Helping on setting up one of the free CI services for open source project (Things like AppVeyor, and that other one... I forgot the name. These things are neat to check of changeset don't break everything all the time.)
- Retrofiting Unti Tests. This is problematic. I tend to break a lot of stuff. I opened an issue about it. I still need to find the correct framework. It should integrate well enough with visual studio but work on other platforms...
- Writing more demo level in the example programs
- Checking if Annwvyn still compiles with the head of the v2-1 branch of Ogre
- Checking if the Linux build don't break. I work primilarly on Visual Studio. And sometiomes it's easy to fall into some behavior that compiles on VS but not on GCC
- Bugging me when Oculus or OpenVR release a new version of their SDKs...
- Bugging me to go write stuff on [Annwvyn.org](https://Annwvyn.org)
- Report the bad stuff on issues here! :wink:
