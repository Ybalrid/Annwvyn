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

