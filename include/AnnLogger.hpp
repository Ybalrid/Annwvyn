/**
* \file AnnLogger.hpp
* \brief Create a ostream to the Ogre logger
* \author A. Brainville (Ybalrid)
*/
#pragma once

#include "systemMacro.h"
//We need the standard string format to be accessible
#include <string>
#include <sstream>
#include <iostream>

namespace Annwvyn
{
	enum class AnnDllExport Log {
		Trivial,
		Status,
		Important
	};

	///Open an output stream to the engine log
	class AnnDllExport AnnDebug : public std::ostream
	{
	public:
		enum class LogLevel {
			Minimal,
			Default,
			Verbose
		};

	private:
		static LogLevel verbosity;
		///Nested buffer class. Write the stings to the engine log.
		class AnnDebugBuff : public std::stringbuf
		{
		public:
			///Construct an AnnDebug buffer
			AnnDebugBuff(Log importance) :
			 currentImportance(importance) {}

			///Will sync the buffer
			~AnnDebugBuff();

			///Sync the buffer by performing an AnnEngine::log, clear it and return success.
			int sync() override;

			Log currentImportance;
		};

	public:
		static void setVerbosity(LogLevel level);

		///Create an AnnDebug object that offer you a output stream to the AnnEngine logger
		///This permit you to write messages to the log using C++ style ostream
		/// example : AnnDebug() << "Player life is now " << playerLife;
		/// where playerLife is a variable. Everything that works with an std::ostream works here.
		AnnDebug(Log importance = Log::Trivial);

		///Permit to log a static string via the debug stream
		/// \copydoc Annwvyn::AnnDebug()
		AnnDebug(const std::string& message, Log importance = Log::Trivial);

		///Destroy the debug outputer object
		~AnnDebug();
	};
}
