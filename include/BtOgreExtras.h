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

#include "systemMacro.h"

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
	class DLL Convert
	{
	public:
		Convert() {};
		~Convert() {};

		static btQuaternion toBullet(const Ogre::Quaternion& q);

		static btVector3 toBullet(const Ogre::Vector3& v);

		static Ogre::Quaternion toOgre(const btQuaternion& q);

		static Ogre::Vector3 toOgre(const btVector3& v);
	};

	//From here on its debug-drawing stuff. ------------------------------------------------------------------

	//Draw the lines Bullet want's you to draw
	class DLL LineDrawer
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
		std::vector<line> lines;

		///Manual object used to display the lines
		Ogre::ManualObject* manualObject;

		///Pointer to the scene manager containing the physics objects
		Ogre::SceneManager* smgr;

		///Vertex index
		int index;

	public:

		///Construct the line drawer, need the name of the scene manager and the datablock (material)
		LineDrawer(Ogre::SceneNode* node, Ogre::String datablockId, Ogre::String smgrName);

		~LineDrawer();

		///Clear the manual object AND the line buffer
		void clear();

		///Add a line to the "line buffer", the list of lines that will be shown at next update
		void addLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& value);

		///Check if the material actually exist, if it doesn't create it
		void checkForMaterial();

		///Update the content of the manual object with the line buffer
		void update();
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
		///Debug drawer of Bullet.
		/// \param node SceneNode (usually the Root node) where the lines will be attached. A static child node will be created
		/// \param world Pointer to the btDynamicsWolrd you're using
		/// \param smgrName Name of the scene manager you are using
		DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world, Ogre::String smgrName = "MAIN_SMGR");

		virtual ~DebugDrawer();

		///Set the value to multiply the texure. >= 1. Usefull only for HDR rendering
		void setUnlitDiffuseMultiplier(float value);

		///For bullet : add a line to the drawer
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

		///Dummy. Rendering text is hard :D
		virtual void draw3dText(const btVector3& location, const char* textString) override;

		///Just render the contact point wit a line
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

		///Redirect erros to the Ogre default log
		virtual void reportErrorWarning(const char* warningString) override;

		///Set the debug mode. Acceptable values are combinations of the flags defined by btIDebugDraw::DebugDrawModes
		virtual void setDebugMode(int isOn) override;

		///get the current debug mode
		virtual int getDebugMode() const override;

		///Step the debug drawer
		void step();
	};
}

#endif
