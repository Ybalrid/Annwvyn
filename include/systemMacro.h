#pragma once
/**
 * \file systemMacro.h
 * \brief file for defining macros used for symbol export regarding the way the OS does things.
 * \author A. Brainville
 */

//PLEASE INCLUDE THIS FILE ON ANY ANNWVYN HEADER

/*This file contains macros and symbols definition that have to be
* parsed by pre-compiler on each file.
*/

//DLL management for visual studio compiler.
#undef DLL
#ifdef DLLDIR_EX //Defined in Annwvyn MSVC project file only
	///To construct a DLL, The macro __declspec(dllexport) have to be called in front of the exported symbol
#define DLL  __declspec(dllexport)   // export DLL information
#else
	///Macro to reference symbols from a DLL the macro __declspec(dllimport) have to be called the same way
#define DLL  __declspec(dllimport)   // import DLL information
#endif

//DO NOT FORGET TO CALL THE DLL MACRO AT ANY CLASS AND GLOBAL FUNCTION DECLARATION!
//e.g. class DLL SomeClass {};

//Thanks to http://stackoverflow.com/a/21265197 for this convenient macro
#ifdef __GNUC__
///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#define DEPRECATED __attribute__((deprecated("This method as been flagged as DEPRECATED. Please check Annwvyn's class header for replacement")))
#elif defined(_MSC_VER)
///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#define DEPRECATED __declspec(deprecated("This method as been flagged as DEPRECATED. Please check Annwvyn's class header for replacement"))
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#define DEPRECATED
#endif
