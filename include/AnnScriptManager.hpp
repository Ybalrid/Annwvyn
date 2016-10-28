#pragma once

#include <chaiscript.hpp>
#include <chaiscript_stdlib.hpp>

#include "systemMacro.h"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	class AnnGammeObject;
	class DLL AnnBehaviourScript
	{
	public:
		AnnBehaviourScript();
		AnnBehaviourScript(std::string name, std::function<void(chaiscript::Boxed_Value&)> updateHook, chaiscript::Boxed_Value chaisriptInstance);
		//AnnBehaviourScript(bool invalid = false);

		void update();

		bool isValid();

	private:
		const bool valid;
		std::string name;
		chaiscript::Boxed_Value ScriptObjectInstance;
		std::function<void(chaiscript::Boxed_Value&)> callUpdateOnScriptInstance;

		//void callUpdateOnScript() { callUpdateOnScriptInstance(ScriptObjectInstance); }
	};

	///Script Manager, serve as an interface between ChaiScript and the rest of the engine
	class DLL AnnScriptManager : public AnnSubSystem
	{
	public:

		using AnnScriptID = unsigned long long int;

		///Construct the script manager, initialize ChaiScript and add global functions
		AnnScriptManager();

		///This subsystem doesn't need to be updated
		bool needUpdate() { return false; };

		///This method does nothing
		void update() { return; }

		bool evalFile(const std::string& file);

		AnnBehaviourScript getBehaviourScript(const std::string& scriptName);

	private:

		///ChaiScript engine
		chaiscript::ChaiScript chai;

		void registerApi();

		static constexpr const char* const scriptExtension{ ".chai" };

		//Debug messages
		static constexpr const char* const fileErrorPrefix{ "Script File ERROR - " };
		static constexpr const char* const logFromScript{ "Script - " };

		//String constant for script loading and class initialization
		static constexpr const char* const scriptTemplate{ "var ScriptInstance__OBJECT_SCRIPT_ID__ = __SCRIPT_NAME__();" };
		static constexpr const char* const scriptNameMarker{ "__SCRIPT_NAME__" };
		static constexpr const char* const scriptObjectID{ "__OBJECT_SCRIPT_ID__" };
		static constexpr const char* const scriptInstanceMarker{ "ScriptInstance" };
		static constexpr const size_t nameMarkerLen{ 15 };
		static constexpr const size_t scriptIDMarkerLen{ 20 };

		//Static counter that will be incremented at each script creation
		static AnnScriptID ID;
	};
}