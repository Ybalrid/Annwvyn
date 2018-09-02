project(Annwvyn)

file(GLOB OculusRenderer Oculus/*)
file(GLOB OpenVRRenderer OpenVR/*)

set(Annwvyn_Build_OpenVR_Pluign true CACHE BOOL "If you want to build the OpenVR renderer, use that")

if(Annwvyn_Build_OpenVR_Pluign)

	add_library(AnnOgreOpenVRRenderer SHARED ${OpenVRRenderer})

	if(WIN32)
		target_link_libraries(AnnOgreOpenVRRenderer
			Annwvyn
			openvr_api.lib
			)

			install(TARGETS AnnOgreOpenVRRenderer  DESTINATION ${CMAKE_SOURCE_DIR}/lib)
			install(FILES $<TARGET_PDB_FILE:AnnOgreOpenVRRenderer> DESTINATION ${CMAKE_SOURCE_DIR}/lib OPTIONAL)

	endif()
	if(UNIX)
		target_link_libraries(AnnOgreOpenVRRenderer
		libopenvr_api.so
		)

		install(TARGETS AnnOgreOpenVRRenderer LIBRARY DESTINATION lib)
	endif()
	cotire(AnnOgreOpenVRRenderer)

endif()

if(WIN32)


	set(Annwvyn_Build_Oculus_Pluign true CACHE BOOL "If you want to build the Oculus renderer, use that")

	if(Annwvyn_Build_Oculus_Pluign)
		add_library(AnnOgreOculusRenderer SHARED ${OculusRenderer})

		target_link_libraries(AnnOgreOculusRenderer
			Annwvyn
			optimized $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Release/VS2017/LibOVR.lib
			debug  $ENV{AnnwvynSDK64}/OculusSDK/LibOVR/Lib/Windows/x64/Debug/VS2017/LibOVR.lib

			ws2_32.lib
			Setupapi.lib
			)

			install(TARGETS AnnOgreOculusRenderer  DESTINATION ${CMAKE_SOURCE_DIR}/lib)
			install(FILES $<TARGET_PDB_FILE:AnnOgreOculusRenderer> DESTINATION ${CMAKE_SOURCE_DIR}/lib OPTIONAL)


		cotire(AnnOgreOculusRenderer)
	endif()
endif()