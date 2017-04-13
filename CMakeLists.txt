cmake_minimum_required(VERSION 2.8.0)


project(Annwvyn)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /fp:fast")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

    #compiler configuration

endif()

#includes from old visual studio solution
#

include_directories(
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/include
    $ENV{AnnwvynSDK64}/AL/openal-soft/include/AL
    $ENV{AnnwvynSDK64}/libsndfile/include
    $ENV{AnnwvynSDK64}/AL/openal-soft/OpenAL32/Include
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Src
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext/GL
    $ENV{AnnwvynSDK64}/OculusSDK/3rdParty/glext
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVRKernel/Src
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Include
    $ENV{AnnwvynSDK64}/bullet3/src
    $ENV{AnnwvynSDK64}/Ogre21SDK/include/OGRE
    $ENV{AnnwvynSDK64}/OIS/include
    $ENV{AnnwvynSDK64}/glew/include
    $ENV{AnnwvynSDK64}/glew/include/GL
    $ENV{AnnwvynSDK64}/openvr/headers
    $ENV{AnnwvynSDK64}/chaiscript
    $ENV{AnnwvynSDK64}/tinyxml2/
    $ENV{AnnwvynSDK64}/BtOgre21/include/
    #TODO handle PCH
    ${PROJECT_SOURCE_DIR}/pch
    ${PROJECT_SOURCE_DIR}/include
    )

#link directory form old VS solution
link_directories(
    $ENV{AnnwvynSDK64}/glfw-3.2.1.bin.WIN64/lib-vc2015/
    $ENV{AnnwvynSDK64}/AL/openal-soft/build/Release
    $ENV{AnnwvynSDK64}/bullet3/lib/Release
    $ENV{AnnwvynSDK64}/bullet3/lib
    $ENV{AnnwvynSDK64}/libsndfile/lib
    $ENV{AnnwvynSDK64}/OIS/OIS_ReleaseDll
    $ENV{AnnwvynSDK64}/Ogre21SDK/lib/Release/opt
    $ENV{AnnwvynSDK64}/Ogre21SDK/lib/Release
    $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/
    $ENV{AnnwvynSDK64}/glew/lib/Release/x64
    $ENV{AnnwvynSDK64}/openvr/lib/win64
    $ENV{AnnwvynSDK64}/tinyxml2/tinyxml2/bin/tinyxml2/x64-Release-Lib/
    $ENV{AnnwvynSDK64}/BtOgre21/build/Release
    )

file(GLOB AnnwvynSources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/pch/stdafx.cpp)
file(GLOB AnnwvynHeaders ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/include/*.h) 

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

ADD_MSVC_PRECOMPILED_HEADER( pch/stdafx.h pch/stdafx.cpp AnnwvynSources)

add_library(Annwvyn SHARED ${AnnwvynSources} ${AnnwvynHeaders})

#linker input from old VS solution
target_link_libraries( Annwvyn
    glfw3.lib
    opengl32.lib
    glew32.lib
    ws2_32.lib
    OgreMain.lib
    OgreOverlay.lib
    OgreHlmsPbs.lib
    OgreHlmsUnlit.lib
    OIS.lib
    Setupapi.lib
    LibOVR.lib
    openvr_api.lib
    Winmm.lib
    BulletCollision.lib
    BulletDynamics.lib
    LinearMath.lib
    OpenAL32.lib
    libsndfile-1.lib
    tinyxml2.lib
    BtOgre21.lib
    )


set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
set_target_properties(Annwvyn PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
