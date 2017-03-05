/*
* =====================================================================================
*
*       Filename:  BtOgreExtras.h
*
*    Description:  Contains the Ogre Mesh to Bullet Shape converters.
*
*        Version:  1.0
*        Created:  27/12/2008 01:45:56 PM
*
*         Author:  Nikhilesh (nikki)
*
* =====================================================================================
*/

#ifndef _BtOgreShapes_H_
#define _BtOgreShapes_H_

#include "btBulletDynamicsCommon.h"
#include "OgreSceneNode.h"
#include "OgreSimpleRenderable.h"
#include "OgreCamera.h"
#include "OgreHardwareBufferManager.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"

#include "OgreLogManager.h"

namespace BtOgre
{
	typedef std::vector<Ogre::Vector3> Vector3Array;

	//Converts from and to Bullet and Ogre stuff. Pretty self-explanatory.
	class Convert
	{
	public:
		Convert() {};
		~Convert() {};

		static btQuaternion toBullet(const Ogre::Quaternion &q)
		{
			return btQuaternion(q.x, q.y, q.z, q.w);
		}
		static btVector3 toBullet(const Ogre::Vector3 &v)
		{
			return btVector3(v.x, v.y, v.z);
		}

		static Ogre::Quaternion toOgre(const btQuaternion &q)
		{
			return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z());
		}
		static Ogre::Vector3 toOgre(const btVector3 &v)
		{
			return Ogre::Vector3(v.x(), v.y(), v.z());
		}
	};

	//From here on its debug-drawing stuff. ------------------------------------------------------------------

	//Draw the lines Bullet want's you to draw
	class LineDrawer
	{
		Ogre::String sceneManagerName;
		///How a line is stored
		struct line
		{
			Ogre::Vector3 start;
			Ogre::Vector3 end;
			Ogre::ColourValue vertexColor;
		};

		///Where the created objects will be attached
		Ogre::SceneNode* attachNode;

		///The name of the HLMS datablock to use
		Ogre::String datablockToUse;

		///Array of lines and objects to use
		//std::vector<Ogre::ManualObject*> objects;
		Ogre::ManualObject* manualObject;
		std::vector<line> lines;

		Ogre::SceneManager* smgr;

		int index;

	public:
		LineDrawer(Ogre::SceneNode* node, Ogre::String datablockId, Ogre::String smgrName) :
			attachNode(node), manualObject(nullptr), index(0),
			sceneManagerName(smgrName)
		{
			smgr = Ogre::Root::getSingleton().getSceneManager(sceneManagerName);
		}

		~LineDrawer()
		{
			clear();
		}

		void clear()
		{
			if (manualObject) manualObject->clear();
			lines.clear();
		}

		void addLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& value)
		{
			lines.push_back({ start, end, value });
		}

		void checkForMaterial()
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

		void update()
		{
			if (!manualObject)
			{
				Ogre::LogManager::getSingleton().logMessage("Create manual object");
				manualObject = smgr->createManualObject(Ogre::SCENE_STATIC);
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
			manualObject->setCastShadows(false);
		}
	};

	class DebugDrawer : public btIDebugDraw
	{
	protected:
		Ogre::SceneNode *mNode;
		btDynamicsWorld *mWorld;
		int mDebugOn;
		static constexpr char* unlitDatablockName{ "DebugLinesGenerated" };
		const Ogre::IdString unlitDatablockId;

		//To accommodate the diffuse color -> light value "change" of meaning of the color of a fragment in HDR pipelines, multiply all colors by this value
		float unlitDiffuseMultiplier;

		bool stepped;
		Ogre::String scene;
		LineDrawer drawer;

	public:
		DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world, Ogre::String smgrName = "MAIN_SMGR") :
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

		~DebugDrawer()
		{
		}

		void setUnlitDiffuseMultiplier(float value)
		{
			if (value >= 1) unlitDiffuseMultiplier = value;
		}

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
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

		void draw3dText(const btVector3 &location, const char *textString) override
		{
		}

		void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override
		{
			drawLine(PointOnB, PointOnB + normalOnB * distance * 20, color);
		}

		void reportErrorWarning(const char *warningString) override
		{
			Ogre::LogManager::getSingleton().logMessage(warningString);
		}

		//0 for off, anything else for on.
		void setDebugMode(int isOn) override
		{
			mDebugOn = isOn;

			if (!mDebugOn)
				drawer.clear();
		}

		//0 for off, anything else for on.
		int	getDebugMode() const override
		{
			return mDebugOn;
		}

		void step()
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
	};
}

#endif
