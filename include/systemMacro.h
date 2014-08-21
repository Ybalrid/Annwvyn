//PLEASE INCLUDE THIS FILE ON ANY ANNWVYN HEADER

/*This file conrain macro and symbols definition that have to be
* parsed by precompiler on each file.
* They handle diferencies between compilers
*/

//-------------------------DLL EXPORT -------------------------------//
//DLL management for visual studio compiller.
#undef DLL
#ifdef DLLDIR_EX //Defined in Annwvyn MSVC project file
//To contruct a DLL, The macro __declspec(dllexport) have to be called in front of the exported symbol
   #define DLL  __declspec(dllexport)   // export DLL information
#else
//To reference symbols from a DLL the macro __declspec(dllimport) have to be called the same way
   #define DLL  __declspec(dllimport)   // import DLL information
#endif
//Bypass on GNU/Linux : 
#ifdef __gnu_linux__ //All that stuff is handeled by the makefile and the way g++ is called. DLL macros have to be empty but declared.
	#undef DLL	
	#define DLL
#endif
//--------------------------------------------------------------------//

//--------------------Application Entrypoint definition--------------//
//Main definition :
/*
 *	For more simplicity, Program start by a "AnnMain" function at the library user
 *	side. This allow to select proper entry proint for the aplication, and maybe 
 *	add pre-starting treatements here.
 */
#undef AnnMain

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#define AnnMain() INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)

#else
//On others :
#define AnnMain() int main(int argc, char** argv)
#endif
//---------------------------------------------------------------------//

