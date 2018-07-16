// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnLogger.hpp"
//The debug output is opened by the AnnEngine class
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnDebug::AnnDebug() :
 std::ostream(new AnnDebugBuff())
{
}

AnnDebug::AnnDebug(const std::string& message) :
 std::ostream(new AnnDebugBuff())
{
	*this << message;
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
	AnnEngine::writeToLog(str());
	str("");
	return 0;
}
