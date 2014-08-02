//PLEASE INCLUDE THIS FILE ON ANY ANNWVYN HEADER

/*This file conrain macro and symbols definition that have to be
* parsed by precompiler on each file.
* They handle diferencies between compilers
*/


//DLL management for visual studio
#undef DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   // export DLL information
#else
   #define DLL  __declspec(dllimport)   // import DLL information
#endif
//Bypass on GNU/Linux : 
#ifdef __gnu_linux__
	#undef DLL	
	#define DLL
#endif

//Main definition : 
#undef AnnMain()

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//On windows : 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#define AnnMain() INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)

#else
//On others :
#define AnnMain() int main(int argc, char** argv)
#endif


