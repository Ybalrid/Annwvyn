/**
 * \file AnnErrorCode.hpp
 * \brief Define a series of hex code as constant keywords to define error codes.
 * \author A. Brainville (Ybalrid)
 */
#ifndef ANN_ERROR_CODE
#define ANN_ERROR_CODE

#define ANN_ERR_CRITIC 0xDEAD60D // Dead God
/*
Indicate that the execution environment of the engine hasn't been about to be initialized
- Check log file
*/
#define ANN_ERR_MEMORY 0xFAA760D // Fa_t God
/*
Indicate that a memory error has been caused inside the engine
- Check if you haven't done something stupid pointers
- Please don't try to delete anything created by the engine, it's not you're problem
- Check log file for more information
*/
#define ANN_ERR_RENDER 0x5ADE60D // Sa_d God
/*
Indicate an irrecoverable error with the renderer. Either during initialization or during rendering
- Check OVR runtime version
- Check hardware (HMD plugged, Graphics card compatible)
- Check other software (Graphics drivers)
- Check log file for more information
*/
#define ANN_ERR_INFILE 0x12F1D31 // Infi-del
/*
Indicate that the engine is unable to understand an Annwvyn file you tried to load
- Check file path in your code
- Check file path referenced in the file
- Check syntax of the file
- Check log file at the moment of crash for more information
*/
#define ANN_ERR_UNKOWN 0x200760D // No_t God
/*
See log. Something gone wrong and don't have a specific error code.
*/

#define ANN_ERR_NOTINIT 0x207760D
/*
See log. Something that should have been initialized beforehand isn't here.
If there isn't a log, you tried to use something relating ogre before Ogre initialization
*/

#define ANN_ERR_CANTHMD 0xB12D60D // BL(i)ND God
/*
Cannot select the wanted HMD
*/

#include <Windows.h>
inline void displayWin32ErrorMessage(LPCWSTR title, LPCWSTR content)
{
	MessageBox(nullptr, content, title, MB_ICONERROR);
}

#endif