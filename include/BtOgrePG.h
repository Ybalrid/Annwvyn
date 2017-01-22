/*
 * =====================================================================================
 *
 *       Filename:  BtOgrePG.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Bullet to
 *                  Ogre (like updating graphics object positions).
 *
 *        Version:  1.0.1
 *        Created:  27/12/2008 03:40:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _BtOgreGP_H_
#define _BtOgreGP_H_

#ifdef _WIN32
#pragma warning (disable : 4244)
#endif

#include "systemMacro.h"

#include "btBulletDynamicsCommon.h"
#include "OgreSceneNode.h"
#include "BtOgreExtras.h"

namespace BtOgre {
//A MotionState is Bullet's way of informing you about updates to an object.
//Pass this MotionState to a btRigidBody to have your SceneNode updated automatically.
	class DLL RigidBodyState : public btMotionState
	{
	protected:
		btTransform mTransform;
		btTransform mCenterOfMassOffset;

		Ogre::SceneNode *mNode;

	public:
		RigidBodyState(Ogre::SceneNode *node, const btTransform &transform, const btTransform &offset = btTransform::getIdentity())
			: mTransform(transform),
			mCenterOfMassOffset(offset),
			mNode(node)
		{
		}

		RigidBodyState(Ogre::SceneNode *node)
			: mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0, 0, 0, 1)),
			((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition()) : btVector3(0, 0, 0))),
			mCenterOfMassOffset(btTransform::getIdentity()),
			mNode(node)
		{
		}

		virtual void getWorldTransform(btTransform &ret) const
		{
			ret = mTransform;
		}

		virtual void setWorldTransform(const btTransform &in)
		{
			if (mNode == NULL)
				return;

			mTransform = in;
			btTransform transform = in * mCenterOfMassOffset;

			btQuaternion rot = transform.getRotation();
			btVector3 pos = transform.getOrigin();

			//Hack by Ybalrid : move the world transform instead of the `relative to parent one
			Ogre::Vector3 ogrePos(pos.x(), pos.y(), pos.z());
			Ogre::Quaternion ogreRot(rot.w(), rot.x(), rot.y(), rot.z());

			mNode->_setDerivedOrientation(ogreRot);
			mNode->_setDerivedPosition(ogrePos);
		}

		void setNode(Ogre::SceneNode *node)
		{
			mNode = node;
		}
	};

	//Softbody-Ogre connection goes here!
}

#endif
