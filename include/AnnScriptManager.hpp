#pragma once

#include <chaiscript.hpp>
#include <chaiscript_stdlib.hpp>

#include "systemMacro.h"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	class AnnGameObject;
	class DLL AnnBehaviorScript
	{
	public:
		AnnBehaviorScript();
		AnnBehaviorScript(std::string name, std::function<void(chaiscript::Boxed_Value&)> updateHook, chaiscript::Boxed_Value chaisriptInstance);
		AnnBehaviorScript(const AnnBehaviorScript& script);
		//AnnBehaviorScript(bool invalid = false);

		AnnBehaviorScript operator=(const AnnBehaviorScript& script);

		void update();

		bool isValid();

	private:
		const bool valid;
		std::string name;
		chaiscript::Boxed_Value ScriptObjectInstance;
		std::function<void(chaiscript::Boxed_Value&)> callUpdateOnScriptInstance;

		void callUpdateOnScript() { callUpdateOnScriptInstance(ScriptObjectInstance); }
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

		std::shared_ptr <AnnBehaviorScript> getBehaviorScript(const std::string& scriptName, AnnGameObject* owner = nullptr);

		//void AnnScriptManager::addTaggedGameObject(AnnGameObject)

	private:

		///ChaiScript engine
		chaiscript::ChaiScript chai;

		void registerApi();

		static constexpr const char* const scriptExtension{ ".chai" };

		//Debug messages
		static constexpr const char* const fileErrorPrefix{ "Script File ERROR - " };
		static constexpr const char* const logFromScript{ "Script - " };

		//String constant for script loading and class initialization
		static constexpr const char* const scriptTemplate{ R"(
def create__SCRIPT_NAME____OBJECT_SCRIPT_ID__(owner)
{
	var ScriptInstance__OBJECT_SCRIPT_ID__ = __SCRIPT_NAME__(owner);
	return ScriptInstance__OBJECT_SCRIPT_ID__;
}
)" };
		static constexpr const char* const scriptNameMarker{ "__SCRIPT_NAME__" };
		static constexpr const char* const scriptObjectID{ "__OBJECT_SCRIPT_ID__" };
		static constexpr const char* const scriptOwnerMarker{ "__SCRIPT_OWNER__" };
		static constexpr const char* const scriptInstanceMarker{ "ScriptInstance" };
		static constexpr const char* const scriptOwnerPrefix{ "ScriptOwner" };
		static constexpr const size_t nameMarkerLen{ 15 };
		static constexpr const size_t scriptIDMarkerLen{ 20 };
		static constexpr const size_t scriptOwnerMarkerLen{ 16 };

		//Static counter that will be incremented at each script creation
		static AnnScriptID ID;
	};
}