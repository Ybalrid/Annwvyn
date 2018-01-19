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

//AnnDllExport management for visual studio compiler.
#ifdef _WIN32
#	ifdef DLLDIR_EX							   //Defined in Annwvyn MSVC project file only
												   ///To construct a dll, The macro __declspec(dllexport) have to be called in front of the exported symbol
#		define AnnDllExport __declspec(dllexport) // export AnnDllExport information
#	else
///Macro to reference symbols from a AnnDllExport the macro __declspec(dllimport) have to be called the same way
#		define AnnDllExport __declspec(dllimport) // import AnnDllExport information
#	endif
#else
#	define AnnDllExport
#endif

//DO NOT FORGET TO CALL THE AnnDllExport MACRO AT ANY CLASS AND GLOBAL FUNCTION DECLARATION!
//e.g. class AnnDllExport SomeClass {};

//Thanks to http://stackoverflow.com/a/21265197 for this convenient macro
#ifdef __GNUC__
	///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#	define DEPRECATED __attribute__((deprecated("This method as been flagged as DEPRECATED. Please check Annwvyn's class header for replacement")))
#elif defined(_MSC_VER)
	///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#	define DEPRECATED __declspec(deprecated("This method as been flagged as DEPRECATED. Please check Annwvyn's class header for replacement"))
#else
#	pragma message("WARNING: You need to implement DEPRECATED for this compiler")
	///Mark the symbol as Deprecated. The comments on it should help you use the correct method
#	define DEPRECATED
#endif
