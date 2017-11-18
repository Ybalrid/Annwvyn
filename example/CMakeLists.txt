cmake_minimum_required(VERSION 3.6.3)
 
set(CMAKE_CXX_STANDARD 14)
set(ANNWVYN_PROJECT "AnnwvynSamples")

#Name of the project
project(${ANNWVYN_PROJECT})

#Get all the sources and headers (+ PCH files)
file(GLOB ProjSources *.cpp)
file(GLOB ProjHeaders *.hpp)
file(GLOB ProjPCH pch/*)

#copy the necessary files from the sources to the runtime directory
file(GLOB batch *.bat)
file(COPY ${batch} DESTINATION ${PROJECT_BINARY_DIR})
execute_process(COMMAND ${PROJECT_BINARY_DIR}/getLibs.bat ${PROJECT_BINARY_DIR})
file(COPY hlms DESTINATION ${PROJECT_BINARY_DIR})
file(COPY compositor DESTINATION ${PROJECT_BINARY_DIR})
file(COPY media DESTINATION ${PROJECT_BINARY_DIR})
file(COPY level DESTINATION ${PROJECT_BINARY_DIR})
file(COPY script DESTINATION ${PROJECT_BINARY_DIR})

#Todo, handle theses a bit better
file(GLOB scripts *.chai)
file(COPY ${scripts} DESTINATION ${PROJECT_BINARY_DIR})

if(MSVC)
    #Configure compilation parameters
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /fp:fast /bigobj /Gy /D \"_UNICODE\" /D \"UNICODE\" /D \"NOMINMAX\"")
    string(REPLACE "/O2" "/Ox" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ot /Oi")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

#Set path required for CMake modules
set(OGRE_HOME $ENV{AnnwvynSDK64}/Ogre21SDK CACHE PATH "Ogre install")
set(BULLET_ROOT $ENV{AnnwvynSDK64}/bullet3 CACHE PATH "Bullet install")
set(BtOgre21_ROOT $ENV{AnnwvynSDK64}/BtOgre21/build/bin/ CACHE PATH "BtOgre21 install")
set(OIS_ROOT $ENV{AnnwvynSDK64}/OIS CACHE PATH "OIS install")
set(ENV{OPENALDIR} $ENV{AnnwvynSDK64}/AL/openal-soft/build/install CACHE PATH "OpenAL install")
set(ENV{GLFW_LOCATION} $ENV{AnnwvynSDK64}/glfw/build/install CACHE PATH "GLFW install") 
set(TinyXML2_ROOT $ENV{AnnwvynSDK64}/tinyxml2/build/install CACHE PATH "TinyXML install")
set(SNDFILE_WINDOWS_INSTALL $ENV{AnnwvynSDK64}/libsndfile CACHE PATH "TinyXml2 install")
#Where to find additional modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    $ENV{AnnwvynSDK64}/CMakeModules
    $ENV{AnnwvynSDK64}/Ogre21SDK/CMake
    $ENV{AnnwvynSDK64}/BtOgre21/CMake
    )

if(UNIX) 
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        /usr/lib/OGRE/cmake 
        /usr/local/lib/OGRE/cmake 
        /usr/lib/BtOgre21/CMake 
        /usr/local/lib/BtOgre21/CMake
        ~/AnnwvynDeps/CMakeModules
        )
endif()

#Find packages
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
    set(CMAKE_PREFIX_PATH $ENV{AnnwvynSDK64}/glew)
    set(CMAKE_LIBRARY_PATH $ENV{AnnwvynSDK64}/glew/lib/Release/x64)
endif(WIN32)

find_package(GLEW REQUIRED)

include_directories(
    ##VR SDKs
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Src
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext/GL
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVRKernel/Src
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Include
    $ENV{AnnwvynSDK64}/openvr/headers
    $ENV{HOME}/AnnwvynDeps/openvr/headers

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
    $ENV{HOME}/AnnwvynDeps/chaiscript
    ${TinyXML2_INCLUDE_DIRS}
    #Project own include
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include

    #Annwvyn Engine
    $ENV{AnnwvynSDK64}/Annwvyn/include
    #$ENV{HOME}/Annwvyn/include
    /usr/local/include/Annwvyn
    )

link_directories($ENV{AnnwvynSDK64}/Annwvyn/lib/

    $ENV{HOME}/AnnwvynDeps/openvr/lib/linux64/

    )

file(GLOB ProjScripts  ${PROJECT_BINARY_DIR}/*.chai)

message(STATUS "Beware! The files ${ProjScripts} are resources copied inside the binary directory. The COPIED FILES are added to the project. Sale goes the media, compositor and hlms directories.")
message("If you want them to be updated you should re-run cmake")
add_executable(${ANNWVYN_PROJECT} ${ProjSources} ${ProjHeaders} ${ProjPCH} ${ProjScripts})

target_link_libraries(${ANNWVYN_PROJECT}
    Annwvyn
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    )

if(UNIX)
    target_link_libraries(${ANNWVYN_PROJECT}
        ${BULLET_LIBRARIES}

        libopenvr_api.so
        )
endif()


#MSVC specific configuration
if(MSVC)
    set_target_properties(${ANNWVYN_PROJECT} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(${ANNWVYN_PROJECT} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(${ANNWVYN_PROJECT} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
    set_target_properties(${ANNWVYN_PROJECT} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(${ANNWVYN_PROJECT} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif()

