project(Annwvyn)

file(GLOB OculusRenderer Oculus/*)
file(GLOB OpenVRRenderer OpenVR/*)

add_library(AnnOgreOpenVRRenderer SHARED ${OpenVRRenderer})

target_link_libraries(AnnOgreOpenVRRenderer
	Annwvyn
	openvr_api.lib
)

if(unix)
	target_link_libraries(AnnOgreOpenVRRenderer 
	libopenvr_api.so
)
endif(unix)

if(WIN32)
	add_library(AnnOgreOculusRenderer SHARED ${OculusRenderer})

	target_link_libraries(AnnOgreOculusRenderer
		Annwvyn
		optimized $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib
		debug  $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Debug/VS2015/LibOVR.lib

		ws2_32.lib
		Setupapi.lib
)
endif(WIN32)
