/**
 * \file systemMacro.h
 * \brief file that permit to handle differencies between operating systemps
 *        (Windows and GNU/Linux)
 * \author A. Brainville
 */

//PLEASE INCLUDE THIS FILE ON ANY ANNWVYN HEADER

/*This file conrain macros and symbols definition that have to be
* parsed by precompiler on each file.
* They handle diferencies between compilers
*/

//-------------------------DLL EXPORT -------------------------------//
//DLL management for visual studio compiller.
#undef DLL
#ifdef DLLDIR_EX //Defined in Annwvyn MSVC project file only
//To contruct a DLL, The macro __declspec(dllexport) have to be called in front of the exported symbol
   #define DLL  __declspec(dllexport)   // export DLL information
#else
//To reference symbols from a DLL the macro __declspec(dllimport) have to be called the same way
   #define DLL  __declspec(dllimport)   // import DLL information
#endif

//DO NOT FORGET TO CALL THE DLL MACRO AT ANY CLASS AND GLOBAL FUNCTION DECLARATION!
//e.g. class DLL someClass {};

//Bypass on GNU/Linux : 
#ifdef __gnu_linux__ //All that stuff is handeled by the makefile and the way g++ is called. DLL macros have to be empty but declared.
	#undef DLL	
	#define DLL
#endif
//--------------------------------------------------------------------//

