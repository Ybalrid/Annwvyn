/*
 * =====================================================================================
 *
 *       Filename:  BtOgrePG.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Bullet to
 *                  Ogre (like updating graphics object positions).
 *
 *        Version:  1.0
 *        Created:  27/12/2008 03:40:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _BtOgreGP_H_
#define _BtOgreGP_H_

#include "systemMacro.h"

#include "btBulletDynamicsCommon.h"
#include "OgreSceneNode.h"
#include "BtOgreExtras.h"

namespace BtOgre {
	//A MotionState is Bullet's way of informing you about updates to an object.
	//Pass this MotionState to a btRigidBody to have your SceneNode updated automaticaly.
	class DLL RigidBodyState : public btMotionState
	{
	protected:
		btTransform mTransform;
		btTransform mCenterOfMassOffset;

		Ogre::SceneNode *mNode;

	public:
		RigidBodyState(Ogre::SceneNode* node, const btTransform& transform, const btTransform& offset = btTransform::getIdentity());

		RigidBodyState(Ogre::SceneNode* node);

		virtual void getWorldTransform(btTransform& ret) const override;

		virtual void setWorldTransform(const btTransform& in) override;

		void setNode(Ogre::SceneNode* node);
	};

	//Softbody-Ogre connection goes here!
}

#endif
