/**
* \file AnnLogger.hpp
* \brief Create a ostream to the Ogre logger
* \author A. Brainville (Ybalrid)
*/
#ifndef ANN_LOGGER
#define ANN_LOGGER

#include "systemMacro.h"
//We need the standard string format to be accessible
#include <string>
#include <iostream>
namespace Annwvyn
{
	///Open an output stream to the engine log
	class AnnDllExport AnnDebug : public std::ostream
	{
		///Nested buffer class. Write the stings to the engine log.
		class AnnDebugBuff : public std::stringbuf
		{
		public:
			///Construct an AnnDebug buffer
			AnnDebugBuff() {};

			///Will sync the buffer
			~AnnDebugBuff();;

			///Sync the buffer by performing an AnnEngine::log, clear it and return success.
			int sync() override;;
		};

	public:
		///Create an AnnDebug object that offer you a output stream to the AnnEngine logger
		///This permit you to write messages to the log using C++ style ostream
		/// example : AnnDebug() << "Player life is now " << playerLife;
		/// where playerLife is a variable. Everything that works with an std::ostream works here.
		AnnDebug();

		///Permit to log a static string via the debug stream
		/// \copydoc Annwvyn::AnnDebug()
		AnnDebug(const std::string& message);

		///Destroy the debug outputer object
		~AnnDebug();
	};
}

#endif