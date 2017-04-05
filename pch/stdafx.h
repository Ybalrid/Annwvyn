#pragma once

//Remove some visual studio stupidity
#pragma warning (disable : 4244)

//C STDLIB for C++
#include <cmath>
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

//C++ STL and STDLIB
#include <iostream>
#include <ostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <array>
#include <thread>
#include <mutex>
#include <random>
#include <valarray>
#include <functional>

//Object-Oriented Graphical Rendering Engine
#include <OgrePrerequisites.h>
#include <Ogre.h>
#include <OgreLogManager.h>
#include <OgreVector3.h>
#include <OgreHardwareBufferManager.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#include <OgreSimpleRenderable.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLight.h>
#include <OgreMatrix3.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspaceDef.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <Compositor/OgreCompositorNode.h>
#include <Compositor/OgreCompositorNodeDef.h>
#include <Compositor/Pass/PassClear/OgreCompositorPassClearDef.h>
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <Hlms/Pbs/OgreHlmsPbs.h>
#include <Hlms/Unlit/OgreHlmsUnlit.h>
#include <OgreHlmsManager.h>
#include <OgreHlms.h>
#include <OgreMesh.h>
#include <OgreMesh2.h>
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>

//Object-Oriented Input System
#include <OIS.h>

//Bullet
#include <BtOgre.hpp>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

//OpenAl
#include <al.h>
#include <alc.h>

//libsndfile
#include <sndfile.h>

//Oculus VR API
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

//Chaisript interpretor
#pragma warning (disable : 4602)
#include <chaiscript.hpp>
#include <chaiscript_defines.hpp>
#include <chaiscript_stdlib.hpp>
#include <chaiscript_threading.hpp>
#pragma warning (default: 4602)

//OpenVR (Valve/HTC Vive) API
#include <openvr.h>
#include <openvr_capi.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

//OpenGL stuff :

//GL Extension Wrangler
#include <glew.h>

//GL FrameWork
#include <GLFW/glfw3.h>

//Native windows access
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NAVIVE_WGL
#endif
#include <GLFW/glfw3native.h>