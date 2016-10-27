#include "stdafx.h"
#include "AnnScriptManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnScriptManager::AnnScriptManager() : AnnSubSystem("ScriptManager"),
chai(chaiscript::Std_Lib::library())
{
	AnnDebug() << "Initialized ChaiScript Std_Lib";
	chai.add(chaiscript::fun([](const std::string& s) {AnnDebug() << s; }), "AnnDebugLog");

	chai.eval(R"(
	AnnDebugLog("Try to write to the engine log from ChaiScript");
	)");
}