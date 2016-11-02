//This file only exist to document the scripting API with doxygen. This file should NEVER be included ANYWERE

#pragma once

#include "Annwvyn.h"
#include "doxygen_dummy.hpp"
#include "AnnGameObject.hpp"
#include "AnnLevelManager.hpp"

namespace Annwvyn
{
	//As a safety measure, if anybody include this file even with the warning in the 1st line, it will put it's junk in a sub-namespace
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

		///ScriptFunction: Remove a game boject. Will be removed from the current level if a level is running
		/// \param objectName Name of the object
		void AnnRemoveGameObject(const std::string& objectName);

		///ScriptFunction: get a GameObject from it's ID
		/// \param id The string ID of the object you want
		std::shared_ptr<AnnGameObject> AnnGetGameObject(std::string id);

		///ScriptFunction: set the gravity vector
		/// \param gravity The vector to use as `g`
		void AnnChangeGravity(const Ogre::Vector3& gravity);

		///ScriptFunction: restore the gravity vector
		void AnnRestoreGravity(void);

		///ScriptFunction: Jump the level manager to another level
		/// \param id The ID number of the level
		void AnnJumpLevel(Annwvyn::level_id id);
	}
}