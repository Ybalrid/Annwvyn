cmake_minimum_required(VERSION 3.6.3)


project(Annwvyn)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
if(WIN32)
    #Annwvyn SDK check
    if(NOT DEFINED ENV{AnnwvynSDK64})
        message(FATAL_ERROR "You need to install the Annwvyn SDK. RUNME.bat will set the AnnwvynSDK64 env var that will stop this error!")
    else()
        message("Annwvyn SDK installed at " $ENV{AnnwvynSDK64})
    endif()

    if(NOT MSVC)
        #TODO The SDK only support MSVC binaries right now due to the format of the VR SDK for rift and vive
        message(FATAL_ERROR "You should only try to generate a VS solution")
    else()
        message("Check for MSVC : passed!!")
        #This configure the Annwvyn's header to EXPORT symbols to DLL
        add_definitions(-DDLLDIR_EX)
        #compiler configuration
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /fp:fast /bigobj /Gy /D \"_UNICODE\" /D \"UNICODE\" /D \"NOMINMAX\"")
        string(REPLACE "/O2" "/Ox" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ot /Oi")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    endif()

    #Where to find additional modules
    set(CMAKE_MODULE_PATH 
        $ENV{AnnwvynSDK64}/CMakeModules
        $ENV{AnnwvynSDK64}/Ogre21SDK/CMake
        $ENV{AnnwvynSDK64}/BtOgre21/CMake
        )

    #Set path required for CMake modules
    set(OGRE_HOME $ENV{AnnwvynSDK64}/Ogre21SDK)
    set(BULLET_ROOT $ENV{AnnwvynSDK64}/bullet3)
    set(BtOgre21_ROOT $ENV{AnnwvynSDK64}/BtOgre21/build/bin/)
    set(OIS_ROOT $ENV{AnnwvynSDK64}/OIS)
    set(ENV{OPENALDIR} $ENV{AnnwvynSDK64}/AL/openal-soft/build/install)
else(UNIX) 
    set(CMAKE_MODULE_PATH /usr/lib/OGRE/cmake /usr/local/lib/OGRE/cmake /usr/lib/BtOgre21/CMake /usr/local/lib/BtOgre21/CMake)
endif(WIN32)

#Find findable packages
find_package(OpenGL REQUIRED)
find_package(OGRE COMPONENTS Overlay HlmsPbs HlmsUnlit REQUIRED)
find_package(Bullet REQUIRED)
find_package(BtOgre21 REQUIRED)
find_package(OIS REQUIRED)
find_package(OpenAL REQUIRED)
if(WIN32)
    set(CMAKE_PREFIX_PATH $ENV{AnnwvynSDK64}/glew)
    set(CMAKE_LIBRARY_PATH $ENV{AnnwvynSDK64}/glew/lib/Release/x64)
endif(WIN32)
find_package(GLEW REQUIRED)

#includes from old visual studio solution
#

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
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/include
    #Audio 
    ${OPENAL_INCLUDE_DIR}
    $ENV{AnnwvynSDK64}/libsndfile/include
    #Scripting
    $ENV{AnnwvynSDK64}/chaiscript #!This is an header only library!
    $ENV{AnnwvynSDK64}/tinyxml2/
    #Project own include
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include
    )

#link directory form old VS solution
link_directories(
    #VR SDKs
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/
    $ENV{AnnwvynSDK64}/openvr/lib/win64
    
    ##Other
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/lib-vc2015/
    $ENV{AnnwvynSDK64}/AL/openal-soft/build/Release
    $ENV{AnnwvynSDK64}/libsndfile/lib
    $ENV{AnnwvynSDK64}/tinyxml2/tinyxml2/bin/tinyxml2/x64-Release-Lib/
    )

#List the complete source code
file(GLOB AnnwvynSources src/*.cpp)
file(GLOB AnnwvynHeaders include/*.hpp include/*.h)
file(GLOB AnnwvynPCH pch/*)
set(AnnwvynCode ${AnnwvynSources} ${AnnwvynHeaders} ${AnnwvynPCH})

#Set target for project
add_library(Annwvyn SHARED ${AnnwvynCode} )

#linker input from old VS solution
target_link_libraries( Annwvyn
    ##VR SDKS (manually added)
    LibOVR.lib
    openvr_api.lib
    ##Engine dependencies
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${BtOgre21_LIBRARIES}
    ${OIS_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}

    #Additional libraries (not using cmake find mechanism)
    glfw3.lib
    libsndfile-1.lib
    tinyxml2.lib

    ##Windows own libraries
    ws2_32.lib
    Setupapi.lib
    Winmm.lib
    )

#MSVC specific configuration
if(MSVC)
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif()
