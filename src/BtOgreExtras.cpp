#include "stdafx.h"

#include "BtOgreExtras.h"

btQuaternion BtOgre::Convert::toBullet(const Ogre::Quaternion& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}

btVector3 BtOgre::Convert::toBullet(const Ogre::Vector3& v)
{
	return btVector3(v.x, v.y, v.z);
}

Ogre::Quaternion BtOgre::Convert::toOgre(const btQuaternion& q)
{
	return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z());
}

Ogre::Vector3 BtOgre::Convert::toOgre(const btVector3& v)
{
	return Ogre::Vector3(v.x(), v.y(), v.z());
}

BtOgre::LineDrawer::LineDrawer(Ogre::SceneNode* node, Ogre::String datablockId, Ogre::String smgrName) :
	sceneManagerName(smgrName),
	attachNode(node),
	datablockToUse(datablockId),
	manualObject(nullptr),
	index(0)
{
	smgr = Ogre::Root::getSingleton().getSceneManager(sceneManagerName);
}

BtOgre::LineDrawer::~LineDrawer()
{
	clear();
	if (manualObject)
		smgr->destroyManualObject(manualObject);
}

void BtOgre::LineDrawer::clear()
{
	if (manualObject) manualObject->clear();
	lines.clear();
}

void BtOgre::LineDrawer::addLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& value)
{
	lines.push_back({ start, end, value });
}

void BtOgre::LineDrawer::checkForMaterial()
{
	Ogre::HlmsUnlit* hlmsUnlit = (Ogre::HlmsUnlit*) Ogre::Root::getSingleton().getHlmsManager()->getHlms(Ogre::HLMS_UNLIT);
	auto datablock = hlmsUnlit->getDatablock(datablockToUse);

	if (datablock) return;
	Ogre::LogManager::getSingleton().logMessage("BtOgre's datablock not found, creating...");
	auto createdDatablock = hlmsUnlit->createDatablock(datablockToUse, datablockToUse, Ogre::HlmsMacroblock(), Ogre::HlmsBlendblock(), Ogre::HlmsParamVec(), true, Ogre::BLANKSTRING, "BtOgre");

	if (!createdDatablock)
	{
		Ogre::LogManager::getSingleton().logMessage("Mh. Datablock hasn't been created. Weird.");
	}
}

void BtOgre::LineDrawer::update()
{
	if (!manualObject)
	{
		Ogre::LogManager::getSingleton().logMessage("Create manual object");
		manualObject = smgr->createManualObject(Ogre::SCENE_STATIC);
		manualObject->setCastShadows(false);
		attachNode->attachObject(manualObject);
	}

	checkForMaterial();
	manualObject->begin(datablockToUse, Ogre::OT_LINE_LIST);
	index = 0;

	for (const auto& l : lines)
	{
		manualObject->position(l.start);
		manualObject->colour(l.vertexColor);
		manualObject->index(index++);

		manualObject->position(l.end);
		manualObject->colour(l.vertexColor);
		manualObject->index(index++);
	}

	manualObject->end();
}

BtOgre::DebugDrawer::DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world, Ogre::String smgrName) :
	mNode(node->createChildSceneNode(Ogre::SCENE_STATIC)),
	mWorld(world),
	mDebugOn(true),
	unlitDatablockId(unlitDatablockName),
	unlitDiffuseMultiplier(1),
	stepped(false),
	scene(smgrName),
	drawer(mNode, unlitDatablockName, scene)
{
	if (!Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("BtOgre"))
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup("BtOgre");
}

BtOgre::DebugDrawer::~DebugDrawer()
{
}

void BtOgre::DebugDrawer::setUnlitDiffuseMultiplier(float value)
{
	if (value >= 1) unlitDiffuseMultiplier = value;
}

void BtOgre::DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	if (stepped)
	{
		drawer.clear();
		stepped = false;
	}

	auto ogreFrom = Convert::toOgre(from);
	auto ogreTo = Convert::toOgre(to);
	Ogre::ColourValue ogreColor(color.x(), color.y(), color.z(), 1);
	ogreColor *= unlitDiffuseMultiplier;

	drawer.addLine(ogreFrom, ogreTo, ogreColor);
}

void BtOgre::DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void BtOgre::DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance * 20, color);
}

void BtOgre::DebugDrawer::reportErrorWarning(const char* warningString)
{
	Ogre::LogManager::getSingleton().logMessage(warningString);
}

void BtOgre::DebugDrawer::setDebugMode(int isOn)
{
	mDebugOn = isOn;

	if (!mDebugOn)
		drawer.clear();
}

int BtOgre::DebugDrawer::getDebugMode() const
{
	return mDebugOn;
}

void BtOgre::DebugDrawer::step()
{
	if (mDebugOn)
	{
		mWorld->debugDrawWorld();
		drawer.update();
	}
	else
	{
		drawer.clear();
	}
	stepped = true;
}