#include "stdafx.h"

#include "BtOgrePG.h"

BtOgre::RigidBodyState::RigidBodyState(Ogre::SceneNode* node, const btTransform& transform, const btTransform& offset) : mTransform(transform),
mCenterOfMassOffset(offset),
mNode(node)
{
}

BtOgre::RigidBodyState::RigidBodyState(Ogre::SceneNode* node) : mTransform(((node != NULL) ? BtOgre::Convert::toBullet(node->getOrientation()) : btQuaternion(0, 0, 0, 1)),
((node != NULL) ? BtOgre::Convert::toBullet(node->getPosition()) : btVector3(0, 0, 0))),
mCenterOfMassOffset(btTransform::getIdentity()),
mNode(node)
{
}

void BtOgre::RigidBodyState::getWorldTransform(btTransform& ret) const
{
	ret = mTransform;
}

void BtOgre::RigidBodyState::setWorldTransform(const btTransform& in)
{
	if (mNode == NULL)
		return;

	mTransform = in;
	btTransform transform = in * mCenterOfMassOffset;

	btQuaternion rot = transform.getRotation();
	btVector3 pos = transform.getOrigin();

	//Hack by Ybalrid : move the world positions instead of the absolute position
	Ogre::Vector3 ogrePos(pos.x(), pos.y(), pos.z());
	Ogre::Quaternion ogreRot(rot.w(), rot.x(), rot.y(), rot.z());

	mNode->_setDerivedOrientation(ogreRot);
	mNode->_setDerivedPosition(ogrePos);
}

void BtOgre::RigidBodyState::setNode(Ogre::SceneNode* node)
{
	mNode = node;
}