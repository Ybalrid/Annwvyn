#pragma once

#include "systemMacro.h"

//Include the correct header for the OS
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <memory> //std::unique_ptr

namespace Annwvyn
{
	///Class that handle the lifetime of a DLL. Automatically call FreeLibrary/dlclose for you. Intended to be used inside an unique_ptr
	class AnnDllExport AnnDynamicLibraryHolder
	{
	public:
		///Deleted copy operation
		AnnDynamicLibraryHolder(AnnDynamicLibraryHolder&) = delete;
		///Deleted copy operation
		void operator=(AnnDynamicLibraryHolder&) = delete;

		///Move constructor
		AnnDynamicLibraryHolder(AnnDynamicLibraryHolder&& other)
		{
			holdedLibrary		= other.holdedLibrary;
			other.holdedLibrary = NULL;
		}

		///Move-assign operator
		AnnDynamicLibraryHolder& operator=(AnnDynamicLibraryHolder&& other)
		{
			holdedLibrary		= other.holdedLibrary;
			other.holdedLibrary = NULL;
			return *this;
		}

		///Abstract the type of what we are keeping. This type is either HMODULE or void* depending on the host system. Represed a loaded dynamic library
#ifdef _WIN32
		using AnnDynLib = HMODULE;
#else
		using AnnDynLib = void*;
#endif

		///Construct the holder
		AnnDynamicLibraryHolder(AnnDynLib lib)
		{
			holdedLibrary = lib;
		}

		///Destruct the holder, will FreeLibrary/dlclose the library handle/pointer
		~AnnDynamicLibraryHolder()
		{
			if(holdedLibrary)
#ifdef _WIN32
				if(!FreeLibrary(holdedLibrary))
#else
				if(dlclose(holdedLibrary) != 0)
#endif
				{
					//TODO report about error now
				}
		}

	private:
		///handle to dll/so that has been loaded in memory
		AnnDynLib holdedLibrary = 0;
	};

	///handy typename for an unique_ptr using the above class
	using AnnUniqueDynamicLibraryHolder = std::unique_ptr<AnnDynamicLibraryHolder>;
}
