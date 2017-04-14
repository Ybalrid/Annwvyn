cmake_minimum_required(VERSION 3.6.3)


project(Annwvyn)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 

#Annwvyn SDK check
if(NOT DEFINED ENV{AnnwvynSDK64})
    message(FATAL_ERROR "You need to install the Annwvyn SDK")
else()
    message("Annwvyn SDK installed at " $ENV{AnnwvynSDK64})
endif()

if(NOT MSVC)
    message(FATAL_ERROR "You should only try to generate a VS solution")
else()
    message("Check for MSVC : passed!!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /fp:fast /bigobj /D \"_UNICODE\" /D \"UNICODE\"")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

    add_definitions(-DDLLDIR_EX)

    #compiler configuration

endif()

#Where to find modules
set(CMAKE_MODULE_PATH 
    $ENV{AnnwvynSDK64}/CMakeModules
    $ENV{AnnwvynSDK64}/Ogre21SDK/CMake
    $ENV{AnnwvynSDK64}/BtOgre21/CMake
    )

#Set path required for CMake modules
set(OGRE_HOME $ENV{AnnwvynSDK64}/Ogre21SDK)
set(BULLET_ROOT $ENV{AnnwvynSDK64}/bullet3)
set(BtOgre21_ROOT $ENV{AnnwvynSDK64}/BtOgre21/build/bin/)

#Find findable packages
find_package(OGRE COMPONENTS Overlay HlmsPbs HlmsUnlit REQUIRED)
find_package(Bullet REQUIRED)
find_package(BtOgre21 REQUIRED)

#includes from old visual studio solution
#

include_directories(
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_HlmsPbs_INCLUDE_DIRS}
    ${OGRE_HlmsUnlit_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${BtOgre21_INCLUDE_DIRS}
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/include
    $ENV{AnnwvynSDK64}/AL/openal-soft/include/AL
    $ENV{AnnwvynSDK64}/libsndfile/include
    $ENV{AnnwvynSDK64}/AL/openal-soft/OpenAL32/Include
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Src
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext/GL
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVRKernel/Src
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Include
    $ENV{AnnwvynSDK64}/OIS/include
    $ENV{AnnwvynSDK64}/glew/include
    $ENV{AnnwvynSDK64}/glew/include/GL
    $ENV{AnnwvynSDK64}/openvr/headers
    $ENV{AnnwvynSDK64}/chaiscript
    $ENV{AnnwvynSDK64}/tinyxml2/
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include
    )

#link directory form old VS solution
link_directories(
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/lib-vc2015/
    $ENV{AnnwvynSDK64}/AL/openal-soft/build/Release
    $ENV{AnnwvynSDK64}/libsndfile/lib
    $ENV{AnnwvynSDK64}/OIS/OIS_ReleaseDll
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/
    $ENV{AnnwvynSDK64}/glew/lib/Release/x64
    $ENV{AnnwvynSDK64}/openvr/lib/win64
    $ENV{AnnwvynSDK64}/tinyxml2/tinyxml2/bin/tinyxml2/x64-Release-Lib/
    )

#List the complete source code
file(GLOB AnnwvynSources src/*.cpp)
file(GLOB AnnwvynHeaders include/*.hpp include/*.h)
file(GLOB AnnwvynPCH pch/*)
set(AnnwvynCode ${AnnwvynSources} ${AnnwvynHeaders} ${AnnwvynPCH})


add_library(Annwvyn SHARED ${AnnwvynCode} )


#linker input from old VS solution
target_link_libraries( Annwvyn
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${BtOgre21_LIBRARIES}
    glfw3.lib
    opengl32.lib
    glew32.lib
    ws2_32.lib
    OIS.lib
    Setupapi.lib
    LibOVR.lib
    openvr_api.lib
    Winmm.lib
    OpenAL32.lib
    libsndfile-1.lib
    tinyxml2.lib
    )

if(MSVC)
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif()
