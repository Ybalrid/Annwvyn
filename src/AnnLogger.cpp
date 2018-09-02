// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnLogger.hpp"
//The debug output is opened by the AnnEngine class
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnDebug::LogLevel AnnDebug::verbosity = LogLevel::Default;

void AnnDebug::setVerbosity(LogLevel level)
{
	verbosity = level;
}

AnnDebug::AnnDebug(Log importance) :
 std::ostream(new AnnDebugBuff(importance))
{
}

AnnDebug::AnnDebug(const std::string& message, Log importance) :
 std::ostream(new AnnDebugBuff(importance))
{
		(void)(*this << message);
}

AnnDebug::~AnnDebug()
{
	delete rdbuf();
}

AnnDebug::AnnDebugBuff::~AnnDebugBuff()
{
	pubsync();
}

int AnnDebug::AnnDebugBuff::sync()
{
	if(currentImportance == Log::Important || verbosity == LogLevel::Verbose) goto logNoMatterWhat;

	if(currentImportance == Log::Trivial && verbosity == LogLevel::Minimal)
		return 0;

logNoMatterWhat:
	AnnEngine::writeToLog(str());
	str("");
	return 0;
}
