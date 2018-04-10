project(Annwvyn)

#OpenVR

file(GLOB OpenVRRenderer OpenVR/*)
add_library(AnnOgreOpenVRRenderer SHARED ${OpenVRRenderer})

if(WIN32)
target_link_libraries(AnnOgreOpenVRRenderer
	Annwvyn
	openvr_api.lib
)
endif()

if(UNIX)
	target_link_libraries(AnnOgreOpenVRRenderer
	libopenvr_api.so)

    install(TARGETS AnnOgreOpenVRRenderer LIBRARY DESTINATION lib)

endif(UNIX)

#End OpenVR

#Oculus (Windows only)
if(WIN32)
    file(GLOB OculusRenderer Oculus/*)
	add_library(AnnOgreOculusRenderer SHARED ${OculusRenderer})

	target_link_libraries(AnnOgreOculusRenderer
		Annwvyn
		optimized $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib
		debug  $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Debug/VS2015/LibOVR.lib

		ws2_32.lib
		Setupapi.lib
)
endif(WIN32)
#End Oculus

#OpenHMD
file(GLOB OpenHMDRenderer OpenHMD/*)
add_library(AnnOgreOpenHMDRenderer SHARED ${OpenHMDRenderer})

target_link_libraries(AnnOgreOpenHMDRenderer
    Annwvyn
    )

#End OpenHMD
