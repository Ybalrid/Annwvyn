//This file only exist to document the scripting API with doxygen. This file should NEVER be included ANYWERE

#pragma once

#include "Annwvyn.h"
#include "doxygen_dummy.hpp"
#include "AnnGameObject.hpp"
#include "AnnLevelManager.hpp"

namespace Annwvyn
{
	//As a safety measure, if anybody include this file even with the warning in the 1st line, it will put it's junk in a sub-namespace
	//As a side effect of the above statement, this permit to access the whole thing via a single link on the generated website. GREAT.
	///List prototypes of C++ non-existing functions that match the interface of the Script functions you can use!
	namespace ChaiScriptAPIDoc
	{
		///ScriptFunction: Log a string
		/// \param s String to log
		void AnnDebugLog(const std::string& s);

		///ScriptFunction: Log a 2D vector
		/// \param v Vector to log
		void AnnDebugLog(const Ogre::Vector2& v);

		///ScriptFunction: Log a 3D vector
		/// \param v Vector to log
		void AnnDebugLog(const Ogre::Vector3& v);

		///ScriptFunction: Log a Quaternion
		/// \param q Quaternion to log
		void AnnDebugLog(const Ogre::Quaternion& q);

		///ScriptFunction: Log an angle in Radian
		/// \param a Angle to log
		void AnnDebugLog(const Ogre::Radian& a);

		///ScriptFunction: Log an angle in Degree
		/// \param a Angle to log
		void AnnDebugLog(const Ogre::Degree& a);

		///ScriptFunction: Log a color
		/// \param c Color to log
		void AnnDebugLog(const AnnColor& c);

		///ScriptFunction: Log a keycode
		/// \param c KeyCode number
		void AnnDebugLog(KeyCode::code c);

		///ScriptFunction: Log a mouse axis
		/// \param a Axis
		void AnnDebugLog(MouseAxisId a);

		///ScriptFunction: log a boolean
		/// \param b bool to log
		void AnnDebugLog(bool b);

		///ScriptFunction: log an integer
		/// \param i int
		void AnnDebugLog(int i);

		///ScriptFunction: log a simple precision floating point number
		/// \param f float
		void AnnDebugLog(float f);

		//-----------------------------------------------------------------------

		///ScritpFunciton: Set the world's background color
		/// \param c Color
		void AnnSetWorldBackgroundColor(const AnnColor& c);

		///ScriptFunction: Set the value of the ambient lighting
		/// \param c Color
		void AnnSetAmbientLight(const AnnColor& c);

		///ScriptFunction: Create a game object. Will be added to the current level if a level is running
		/// \param mesh Mesh to use
		/// \param objectName Name that the object will bear
		void AnnCreateGameObject(const std::string& mesh, const std::string& objectName);

		///ScriptFunction: Remove a game object. Will be removed from the current level if a level is running
		/// \param objectName Name of the object
		void AnnRemoveGameObject(const std::string& objectName);

		///ScriptFunction: get a GameObject from it's ID
		/// \param id The string ID of the object you want
		AnnGameObject* AnnGetGameObject(std::string id);

		///ScriptFunction: set the gravity vector
		/// \param gravity The vector to use as `g`
		void AnnChangeGravity(const Ogre::Vector3& gravity);

		///ScriptFunction: restore the gravity vector
		void AnnRestoreGravity(void);

		///ScriptFunction: Jump the level manager to another level
		/// \param id The ID number of the level
		void AnnJumpLevel(Annwvyn::level_id id);

		//-classes-----------------------------------------------------

		///Keyboard event from scripts
		class AnnKeyEvent
		{
		public:
			///Return if event is press event
			bool isPressed();

			///Return if event is released event
			bool isReleased();

			///Key concerned by this event
			KeyCode::code getKey();
		};

		///Mouse event from script
		class AnnMouseEvent
		{
		public:
			///Axis of the mouse 0 is horizontal, 1 is vertical and 2 is scrolling
			AnnStickAxis getAxis(const int id);

			///State of a button, left, right, middle, and others, maybe...
			bool getButtonState(const int id);
		};

		///Axis of a mouse
		class AnnMouseAxis
		{
		public:
			///get relative value
			int getRelValue();

			///get absolute value
			int getAbsValue();
		};

		///Joystick event
		class AnnStickEvent
		{
		public:
			///return how many buttons
			size_t getNbButtons();

			///return how many axes
			size_t getNbAxis();

			///return how many PoV
			size_t getNbPov();

			///return the vendor string of this controller. In practice it's the name of the controller
			std::string getVendor();

			///Get the ID number of this controller
			unsigned int getSitckID();

			///Return true if this controller is an xbox controller. Usefull for oculus rift games...
			bool isXboxController();

			///Is button `i` pressed
			bool isPressed(const int i);

			///Is button `i` released
			bool isReleased(const int i);

			///Is button `i` <strong>currently down</strong>
			bool isDown(const int i);

			///Get the wanted axis
			AnnStickAxis getAxis(const int i);

			///Get the wanted pov
			AnnStickPov getPov(const int i);
		};

		///state of a PoV from a controller
		class AnnStickPov
		{
		public:
			///N
			bool getNorth();
			///S
			bool getSouth();
			///E
			bool getEast();
			///W
			bool getWest();
			/// N && E
			bool getNorthEast();
			/// N && W
			bool getNorthWest();
			/// S && E
			bool getSouthEast();
			/// S && W
			bool getSouthWest();
		};

		///Wait for a timer to go to 0
		class AnnTimeEvent
		{
		public:
			///Get the ID of the timer that timeouted
			timerID getID();
		};

		///A color
		class AnnColor
		{
			///Construct with RGBA
			AnnColor(float r, float g, float b, float a);
			///Construct with a color from Ogre
			AnnColor(const Ogre::ColourValue& c);
			///Assign another color to this one
			AnnColor operator=(AnnColor& c);

			///Get Red as [0;1] float
			float getRed();

			///Get Green as [0;1] float
			float getGreen();

			///Get Blue as [0;1] float
			float getBlue();

			///Get Alpha as [0;1] float
			float getAlpha();

			///Set Red as [0;1] float
			void setRed(float f);
			///Set Green as [0;1] float
			void setGreen(float f);
			///Set Blue as [0;1] float
			void setBlue(float f);
			///Set Alpha as [0;1] float
			void setAlpha(float f);
		};

		///A game object
		class AnnGameObject
		{
		public:
			///Set the position of this object
			void setPosition(AnnVect3 v);

			///Set the orientation of this object
			void setOrientation(AnnQuaternion q);

			///Set the scaling of this object
			void setScale(AnnVect3 v);

			///Get the position of this object
			AnnVect3 getPosition();

			///Get the orientation of this object
			AnnQuaternion getOrientation();

			///Get the scale of this object
			AnnVect3 getScale();

			///Play a sound
			void playSound(std::string name, bool loop = true);
		};

		///Value of Pi
		float PI;

		///Value of Pi/2
		float HALF_PI;
	}
}