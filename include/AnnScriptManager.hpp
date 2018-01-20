#pragma once

#include <systemMacro.h>
#include <AnnScriptFile.hpp>
#include <AnnSubsystem.hpp>
#include <AnnEventManager.hpp>
#include <AnnLightObject.hpp>

#include <chaiscript.hpp>
#include <chaiscript_stdlib.hpp>
#include <AnnTypes.h>

namespace Annwvyn
{
	class AnnGameObject;

	///Object that reprenset a script defining an object "behavior"
	class AnnDllExport AnnBehaviorScript : LISTENER
	{
	public:
		///Invalid script constructor
		AnnBehaviorScript();

		///Callable script constructor
		AnnBehaviorScript(const std::string& name,
						  std::function<void(chaiscript::Boxed_Value&)> updateHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)> KeyEventHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)> MouseEventHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnStickEvent)> StickEventHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)> TimeEventHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)> TriggerHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)> HandControllertHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnCollisionEvent)> CollisionEventHook,
						  std::function<void(chaiscript::Boxed_Value&, AnnPlayerCollisionEvent)> PlayerCollisionEventHook,

						  chaiscript::Boxed_Value scriptObjectInstance);

		///Script destructor
		virtual ~AnnBehaviorScript();

		///Call this to call the "update" of the script
		void update();

		///Return true if the script is valid. If the object is in a state where this returns false, calling "update" on it will crash
		bool isValid() const;

		///register this object as an event listener
		void registerAsListener();

		///unregister this object as an event listener
		void unregisterAsListener();

		///Event from the keyboard
		void KeyEvent(AnnKeyEvent e) override;
		///Event from the mouse
		void MouseEvent(AnnMouseEvent e) override;
		///Event for a Joystick
		void StickEvent(AnnStickEvent e) override;
		///Event from a timer
		void TimeEvent(AnnTimeEvent e) override;
		///Event from a trigger
		void TriggerEvent(AnnTriggerEvent e) override;
		///Event from an HandController
		void HandControllerEvent(AnnHandControllerEvent e) override;
		///Event from the collision between 2 game objects
		void CollisionEvent(AnnCollisionEvent e) override;
		///Event from the collision between the player and a game object
		void PlayerCollisionEvent(AnnPlayerCollisionEvent e) override;

	private:
		///Validity state of this object. Cannot change.
		const bool valid;

		///Name of this script. Name of the "class" defined in the script
		std::string name;

		///The ChaiScript object that is an instance of "name" class. BlackBox to find what "update" to call on the script engine
		chaiscript::Boxed_Value ScriptObjectInstance;

		///The "update" function. ChaiScript "object.update()" is like calling "update(object)"
		std::function<void(chaiscript::Boxed_Value&)> callUpdateOnScriptInstance;
		///Hook for key event
		std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)> callKeyEventOnScriptInstance;
		///Hook for Mouse event
		std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)> callMouseEventOnScriptInstance;
		///Hook for Stick event
		std::function<void(chaiscript::Boxed_Value&, AnnStickEvent)> callStickEventOnScriptInstance;
		///Hook for Time event
		std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)> callTimeEventOnScriptInstance;
		///Hook for Trigger event
		std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)> callTriggerEventOnScriptInstance;
		///Hook for Hand Controller event
		std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)> callHandControllertOnScriptInstance;
		///Hook for Collision event
		std::function<void(chaiscript::Boxed_Value&, AnnCollisionEvent)> callCollisionEventOnScriptInstance;
		///Hook for PlayerCollision event
		std::function<void(chaiscript::Boxed_Value&, AnnPlayerCollisionEvent)> callPlayerCollisionEventOnScriptInstance;

		///Call guards for the hooks above.
		bool cannotKey, cannotMouse, cannotStick, cannotTime, cannotTrigger, cannotHand, cannotCollision, cannotPlayerCollision;

		///Just call the update on the instance
		void callUpdateOnScript() { callUpdateOnScriptInstance(ScriptObjectInstance); }
	};

	///Script Manager, serve as an interface between ChaiScript and the rest of the engine
	class AnnDllExport AnnScriptManager : public AnnSubSystem
	{
	public:
		using AnnScriptID = uID;

		///Construct the script manager, initialize ChaiScript and add global functions. Will initialize the AnnScriptFileManager
		AnnScriptManager();

		///Destruct the Script Manager. will destroy the AnnScriptFileManager
		~AnnScriptManager();

		///This subsystem doesn't need to be updated
		bool needUpdate() override { return false; }

		///This method does nothing
		void update() override {}

		///Evaluate a file. Exceptions internally catches with messages in the log. Return true or false depending on errors
		bool evalFile(const std::string& file);

		///Create a instance to the script. Return a shared pointer.
		std::shared_ptr<AnnBehaviorScript> getBehaviorScript(const std::string& scriptName, AnnGameObject* owner = nullptr);

		///Evaluate one line of chaiCode
		void evalString(const std::string& chaiCode);

		///GetAccess to the chaiscript engine. Only use for special cases.
		chaiscript::ChaiScript* _getEngine();

	private:
		///ChaiScript engine
		chaiscript::ChaiScript chai;

		///Pointer to the script manager
		AnnScriptFileResourceManager* scriptFileManager;

		///Register to the script engine all the things that are possible to do. Called by the constructor
		void registerApi();

		///Register the scriptFileManager
		void registerResourceManager();

		///Unregister the scriptFileManager
		void unregisterResourceManager();

		///The extension of script files
		static constexpr const char* const scriptExtension{ ".chai" };

		///Prefix for error regarding loading script files
		static constexpr const char* const fileErrorPrefix{ "Script File ERROR - " };

		///Prefix for debug print called from a script
		static constexpr const char* const logFromScript{ "Script - " };

		///String constant for script loading and class initialization. This is a bit of ChaiScript code to bootstrap a behavior script
		static constexpr const char* const scriptTemplate{
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

		///To create the event hooks for the scripts :
		std::function<void(chaiscript::Boxed_Value&, AnnKeyEvent)> callKeyEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnMouseEvent)> callMouseEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnStickEvent)> callStickEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnTimeEvent)> callTimeEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnTriggerEvent)> callTriggerEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnHandControllerEvent)> callHandControllertOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnCollisionEvent)> callCollisionEventOnScriptInstance;
		std::function<void(chaiscript::Boxed_Value&, AnnPlayerCollisionEvent)> callPlayerCollisionEventOnScriptInstance;

		///Hook the event listener's "methdod" to the script ones, if possible...
		void tryAndGetEventHooks();
	};

	using AnnScriptManagerPtr = std::shared_ptr<AnnScriptManager>;
}
