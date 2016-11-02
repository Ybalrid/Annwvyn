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
		///Invalid script constructor
		AnnBehaviorScript();

		///Callable script constructor
		AnnBehaviorScript(std::string name, std::function<void(chaiscript::Boxed_Value&)> updateHook, chaiscript::Boxed_Value chaisriptInstance);

		///Copy constructor
		AnnBehaviorScript(const AnnBehaviorScript& script);

		///Script destructor
		~AnnBehaviorScript();

		///Assign operator
		AnnBehaviorScript operator=(const AnnBehaviorScript& script);

		///Call this to call the "update" of the script
		void update();

		///Return true if the script is valid. If the object is in a state where this returns false, calling "update" on it will crash
		bool isValid();

	private:
		///Validity state of this object. Cannot change.
		const bool valid;

		///Name of this script. Name of the "class" defined in the script
		std::string name;

		///The ChaiScript object that is an instance of "name" class. BlackBox to find what "update" to call on the script engine
		chaiscript::Boxed_Value ScriptObjectInstance;

		///The "update" function. ChaiScript "object.update()" is like calling "update(object)"
		std::function<void(chaiscript::Boxed_Value&)> callUpdateOnScriptInstance;

		///Just call the update on the instance
		inline void callUpdateOnScript() { callUpdateOnScriptInstance(ScriptObjectInstance); }
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

		///Evaluate a file. Exceptions internally catches with messages in the log. Return true or false depending on errors
		bool evalFile(const std::string& file);

		///Create a instance to the script. Return a shared pointer.
		std::shared_ptr <AnnBehaviorScript> getBehaviorScript(const std::string& scriptName, AnnGameObject* owner = nullptr);

	private:

		///ChaiScript engine
		chaiscript::ChaiScript chai;

		///Register to the script engine all the things that are possible to do. Called by the constructor
		void registerApi();

		///The extension of script files
		static constexpr const char* const scriptExtension{ ".chai" };

		///Prefix for error regarding loading script files
		static constexpr const char* const fileErrorPrefix{ "Script File ERROR - " };

		///Prefix for debug print called from a script
		static constexpr const char* const logFromScript{ "Script - " };

		///String constant for script loading and class initialization
		static constexpr const char* const scriptTemplate
		{
R"(
def create__SCRIPT_NAME____OBJECT_SCRIPT_ID__(owner)
{
	var ScriptInstance__OBJECT_SCRIPT_ID__ = __SCRIPT_NAME__(owner);
	return ScriptInstance__OBJECT_SCRIPT_ID__;
}
)"
		};
		///Marker for the name of the script
		static constexpr const char* const scriptNameMarker{ "__SCRIPT_NAME__" };
		///Marker for the ID of the script
		static constexpr const char* const scriptObjectID{ "__OBJECT_SCRIPT_ID__" };
		///Marker for the owner of the script
		static constexpr const char* const scriptOwnerMarker{ "__SCRIPT_OWNER__" };
		///Prefix of the name of an instance
		static constexpr const char* const scriptInstanceMarker{ "ScriptInstance" };
		///Prefix for the owner of a script
		static constexpr const char* const scriptOwnerPrefix{ "ScriptOwner" };

		///Static lengths of constant string of the same name
		static constexpr const size_t nameMarkerLen{ 15 };
		///Static lengths of constant string of the same name
		static constexpr const size_t scriptIDMarkerLen{ 20 };
		///Static lengths of constant string of the same name
		static constexpr const size_t scriptOwnerMarkerLen{ 16 };

		///Static counter that will be incremented at each script creation
		static AnnScriptID ID;
	};
}