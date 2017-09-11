cmake_minimum_required(VERSION 3.6.3)

set(CMAKE_CXX_STANDARD 14)

project(Annwvyn)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
    set(ENV{GLFW_LOCATION} $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/ CACHE PATH "GLFW install") 
    set(TinyXML2_ROOT $ENV{AnnwvynSDK64}/tinyxml2 CACHE PATH "TinyXML2 install")
    set(SNDFILE_WINDOWS_INSTALL $ENV{AnnwvynSDK64}/libsndfile CACHE PATH "libsndfile install")

else(UNIX) 
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        /usr/lib/OGRE/cmake 
        /usr/local/lib/OGRE/cmake 
        /usr/lib/BtOgre21/CMake 
        /usr/local/lib/BtOgre21/CMake
        ~/AnnwvynDeps/CMakeModules
        )
endif(WIN32)

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
    ${GLFW_INCLUDE_DIR}
    #Audio 
    ${OPENAL_INCLUDE_DIR}
    ${SNDFILE_INCLUDE_DIRS}
    #Scripting
    $ENV{AnnwvynSDK64}/chaiscript #!This is an header only library!
    $ENV{HOME}/AnnwvynDeps/chaiscript #UNIX
    ${TinyXML2_INCLUDE_DIRS}
    #Project own include
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    )

if(UNIX)    
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    message("here's the list of the include flag to put in your .clang_complete")
    foreach(dir ${dirs})
        message("-I${dir}")
    endforeach()
endif(UNIX)


#link directory form old VS solution
link_directories(
    #VR SDKs
    $ENV{AnnwvynSDK64}/openvr/lib/win64
    )

#List the complete source code
file(GLOB AnnwvynSources src/*.cpp)
file(GLOB AnnwvynHeaders include/*.hpp include/*.h)
if(MSVC)
    file(GLOB AnnwvynPCH pch/*)
endif()
set(AnnwvynCode ${AnnwvynSources} ${AnnwvynHeaders} ${AnnwvynPCH})

file(GLOB TestCode tests/*.cpp tests/*.hpp)
if(MSVC)
    file(GLOB TestPCH tests/pch/*)
endif()

#Set target for project
add_library(Annwvyn SHARED ${AnnwvynCode} )
add_executable(AnnwvynUnitTest ${TestCode} ${TestPCH})

set (LIBS 
    ##Engine dependencies
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    ${BULLET_LIBRARIES}
    optimized ${BtOgre21_LIBRARIES}
    debug ${BtOgre21_LIBRARIES_DEBUG}
    ${OIS_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${TinyXML2_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    )

#linker input from old VS solution
target_link_libraries( Annwvyn
    ${LIBS}
    )

target_link_libraries(AnnwvynUnitTest
    Annwvyn
    ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES}
    ${OGRE_Overlay_LIBRARIES}
    )


if(WIN32)

    set(WIN_ADDITIONAL_LIBS
        ##VR SDKS (manually added)
        optimized $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib          #TODO WIN32 specific
        debug  $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Debug/VS2015/LibOVR.lib
        openvr_api.lib      #TODO WIN32 specific

        ##Windows own libraries
        ws2_32.lib      #TODO WIN32 specific
        Setupapi.lib    #TODO WIN32 specific
        Winmm.lib       #TODO WIN32 specific
        dsound.lib
        )

    target_link_libraries(Annwvyn
        ${WIN_ADDITIONAL_LIBS}
        )


endif()

#MSVC specific configuration
if(MSVC)
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
    set_target_properties(Annwvyn PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
    set_target_properties(AnnwvynUnitTest PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
    set_source_files_properties(tests/pch/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif()

#Installation
if(WIN32)
    set(Annwvyn_INSTALLABLE ${CMAKE_BINARY_DIR}/Release)
    set(Annwvyn_ARTIFACTS ${Annwvyn_INSTALLABLE}/Annwvyn.lib ${Annwvyn_INSTALLABLE}/Annwvyn.dll ${Annwvyn_INSTALLABLE}/Annwvyn.exp)
    install(FILES ${Annwvyn_ARTIFACTS} DESTINATION ${CMAKE_SOURCE_DIR}/lib)
endif()

if(UNIX)
    install(TARGETS Annwvyn LIBRARY DESTINATION lib)
    install(FILES ${AnnwvynHeaders} DESTINATION include/Annwvyn/)
endif()

