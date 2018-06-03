cmake_minimum_required(VERSION 3.6.3)

set(CMAKE_CXX_STANDARD 14)

project(Annwvyn)

#Basic build configuration
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(WIN32)
    #Check Annwvyn SDK configuration (This env var should exist)
    if(NOT DEFINED ENV{AnnwvynSDK64})
        message(FATAL_ERROR "The AnnwvynSDK64 environement variable is currently not set. You need to install the Annwvyn SDK,
        or, in case you are attempting to manually build the engine, you need to navigate to your dependencies folder and execute \"RUNME.BAT\"")
    else()
        message("Detected Annwvyn SDK. Will fetch libraries from: " $ENV{AnnwvynSDK64})
    endif()

	#We only support MSVC
    if(MSVC)
        message("Configuring build option in visual studio...")
        #This configure the Annwvyn's header to EXPORT symbols to DLL
        add_definitions(-DDLLDIR_EX)
        #compiler configuration
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /fp:fast /bigobj /Gy /D \"_UNICODE\" /D \"UNICODE\" /D \"NOMINMAX\"")
        string(REPLACE "/O2" "/Ox" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ot /Oi")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    else()
        message(FATAL_ERROR "You should only try to generate a Visual Studio solution on Windows")
    endif()

    #Where to find additional modules
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        $ENV{AnnwvynSDK64}/CMakeModules
        $ENV{AnnwvynSDK64}/Ogre21SDK/CMake
        $ENV{AnnwvynSDK64}/BtOgre21/CMake
        )

    #Set path required for CMake modules
    set(OGRE_HOME $ENV{AnnwvynSDK64}/Ogre21SDK CACHE PATH "Ogre install")
    set(BULLET_ROOT $ENV{AnnwvynSDK64}/bullet3 CACHE PATH "Bullet install")
    set(BtOgre21_ROOT $ENV{AnnwvynSDK64}/BtOgre21/build/bin/ CACHE PATH "BtOgre21 install")
    set(OIS_ROOT $ENV{AnnwvynSDK64}/OIS CACHE PATH "OIS install")
    set(ENV{OPENALDIR} $ENV{AnnwvynSDK64}/AL/openal-soft/build/install CACHE PATH "OpenAL install")
    set(ENV{GLFW_LOCATION} $ENV{AnnwvynSDK64}/glfw/build/install CACHE PATH "GLFW install")
    set(TinyXML2_ROOT $ENV{AnnwvynSDK64}/tinyxml2/build/install CACHE PATH "TinyXML2 install")
    set(SNDFILE_WINDOWS_INSTALL $ENV{AnnwvynSDK64}/libsndfile CACHE PATH "libsndfile install")

#This is a bit hackish to make the engine technically work on Linux. Even when there's no targetable hardware for the engine that is Linux compatible
else(UNIX)
    set(
		CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        /usr/lib/OGRE/cmake
        /usr/local/lib/OGRE/cmake
        /usr/lib/BtOgre21/CMake
        /usr/local/lib/BtOgre21/CMake
        ~/AnnwvynDeps/CMakeModules
        )
endif()

#Find findable packages
find_package(OpenGL REQUIRED)
find_package(OGRE COMPONENTS Overlay HlmsPbs HlmsUnlit REQUIRED)
find_package(Bullet REQUIRED)
find_package(BtOgre21 REQUIRED)
find_package(OIS REQUIRED)
find_package(OpenAL REQUIRED)
find_package(GLFW REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(SNDFILE REQUIRED)

#finding GLEW on windows require the prefix and library path to be modified
if(WIN32)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{AnnwvynSDK64}/glew)
    set(CMAKE_LIBRARY_PATH ${CMAKE_PREFIX_PATH} $ENV{AnnwvynSDK64}/glew/lib/Release/x64)
endif(WIN32)

find_package(GLEW REQUIRED)
#includes from old visual studio solution

include_directories(
    ##VR SDKs
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Src
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext/GL
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVRKernel/Src
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Include
    $ENV{AnnwvynSDK64}/openvr/headers

    ##Engine dependencies
    #Graphics
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_HlmsPbs_INCLUDE_DIRS}
    ${OGRE_HlmsUnlit_INCLUDE_DIRS}
    #Physics
    ${BULLET_INCLUDE_DIRS}
    ${BtOgre21_INCLUDE_DIRS}
    #Input
    ${OIS_INCLUDE_DIRS}
    #OpengL Support
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
    #Audio
    ${OPENAL_INCLUDE_DIR}
    ${SNDFILE_INCLUDE_DIRS}
    #Scripting
    $ENV{AnnwvynSDK64}/chaiscript #!This is an header only library!
    $ENV{HOME}/AnnwvynDeps/chaiscript #UNIX
    $ENV{HOME}/AnnwvynDeps/openvr/headers
    ${TinyXML2_INCLUDE_DIRS}
    #Project own include
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    )


link_directories(
    #VR SDKs
    $ENV{AnnwvynSDK64}/openvr/lib/win64
    $ENV{HOME}/AnnwvynDeps/openvr/lib/linux64/
    )

#List the complete source code
file(GLOB AnnwvynSources src/*.cpp)
file(GLOB AnnwvynHeaders include/*.hpp include/*.h)
if(MSVC)
    file(GLOB AnnwvynPCH pch/*)
endif()
set(AnnwvynCode ${AnnwvynSources} ${AnnwvynHeaders} ${AnnwvynPCH})
add_library(Annwvyn SHARED ${AnnwvynCode} )

add_subdirectory(tests)
add_subdirectory(renderer)


target_link_libraries( Annwvyn
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${BtOgre21_LIBRARIES}
    ${OIS_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${TinyXML2_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    )

if(WIN32)
    target_link_libraries(Annwvyn
        Winmm.lib
        dsound.lib
        )
endif()

#MSVC specific configuration
if(MSVC)
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif()

#Installation
if(WIN32)
    set(Annwvyn_INSTALLABLE ${CMAKE_BINARY_DIR}/Release)
    set(Annwvyn_ARTIFACTS
    ${Annwvyn_INSTALLABLE}/Annwvyn.lib
    ${Annwvyn_INSTALLABLE}/Annwvyn.dll
    ${Annwvyn_INSTALLABLE}/Annwvyn.exp
    ${Annwvyn_INSTALLABLE}/../renderer/Release/AnnOgreOculusRenderer.dll
    ${Annwvyn_INSTALLABLE}/../renderer/Release/AnnOgreOpenVRRenderer.dll)
    install(FILES ${Annwvyn_ARTIFACTS} DESTINATION ${CMAKE_SOURCE_DIR}/lib)

elseif(UNIX)
    install(TARGETS Annwvyn LIBRARY DESTINATION lib)
    install(FILES ${AnnwvynHeaders} DESTINATION include/Annwvyn/)
endif()

if(UNIX)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    message("here's the list of the include flag to put in your .clang_complete")
    foreach(dir ${dirs})
        message("-I${dir}")
    endforeach()
endif(UNIX)


