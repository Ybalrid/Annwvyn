// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnSplashLevel.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnSplashLevel::AnnSplashLevel(Ogre::String resourceName, std::shared_ptr<AnnLevel> nextLevel, float timeoutTime) :
 constructLevel(),
 timeout(timeoutTime * 1000),
 currentTime(0),
 startTime(-1),
 next(nextLevel),
 CurvedPlane{ nullptr },
 Splash{ nullptr },
 splashImageName(resourceName),
 hasBGM(false) {}

std::vector<AnnVect3> AnnSplashLevel::createCurvedPlaneVertices(float curvature, float width, float height, float definition)
{
	//This will return a vertex list describing a vertical plane with a parabolic curve.
	//The curvature of the plane follow the equation
	//
	//		z = - (1 / curvature) * x^2
	//
	//Used as a vertex buffer, the pivot point of the object is in the center of the plane
	//The definition parameter set the "resolution" of the curve. It's the number of points
	//in the x axis used to define it.

	//Compute some basic parameters
	const auto resolution = width / definition;
	const auto alpha	  = 1 / curvature;
	const auto xmax		  = width / 2;
	const auto xmin		  = -xmax;
	const auto ymax		  = height / 2;
	const auto ymin		  = -ymax;

	//how the z component is calculated
	auto depth = [=](float x) { return alpha * (x * x); };

	//Where the data is stored. Take advantage of RVO
	std::vector<AnnVect3> curve;
	curve.reserve(size_t(definition));

	for(float x = xmin; x < xmax; x += resolution)
	{
		curve.emplace_back(x, ymax, depth(x));
		curve.emplace_back(x, ymin, depth(x));
	}

	return curve;
}

void AnnSplashLevel::createSplashCurvedPlane()
{
	//Create manual object
	AnnDebug() << "Creating the display \"plane\" for the splash";
	auto smgr(AnnGetEngine()->getSceneManager());

	CurvedPlane = smgr->createManualObject();
	CurvedPlane->begin("Splash", Ogre::OT_TRIANGLE_STRIP);

	const float curvature = 50;
	const float width	 = 40;
	const float height	= 40;
	const float definiton = 10;

	auto vertices = createCurvedPlaneVertices(curvature, width, height, definiton);

	Ogre::uint32 index = 0;
	for(const auto& pos : vertices)
	{
		CurvedPlane->position(pos);
		CurvedPlane->textureCoord((pos.x + width / 2.f) / width, 1.0f - ((pos.y + height / 2.f) / height));
		CurvedPlane->index(index++);
	}

	CurvedPlane->end();

	AnnDebug() << "Add plane to scene";
	Splash = smgr->getRootSceneNode()->createChildSceneNode();
	Splash->attachObject(CurvedPlane);
}

Ogre::HlmsUnlitDatablock* AnnSplashLevel::createSplashDatablock(Ogre::HlmsUnlit* unlit)
{
	auto macroblock				   = Ogre::HlmsMacroblock();
	auto blendblock				   = Ogre::HlmsBlendblock();
	macroblock.mCullMode		   = Ogre::CULL_NONE;
	macroblock.mDepthCheck		   = false;
	macroblock.mDepthWrite		   = false;
	macroblock.mScissorTestEnabled = false;

	auto block = static_cast<Ogre::HlmsUnlitDatablock*>(unlit->createDatablock("Splash", "Splash", macroblock, blendblock, Ogre::HlmsParamVec(), true, Ogre::BLANKSTRING, AnnGetResourceManager()->getDefaultResourceGroupName()));
	block->setUseColour(true);

	return block;
}

void AnnSplashLevel::load()
{
	AnnDebug() << "Ignore physics";
	AnnGetPlayer()->setPosition({ 0, 0, 10 });
	AnnGetPlayer()->resetPlayerPhysics();
	AnnGetPlayer()->setOrientation(Ogre::Euler(0, 0, 0));
	AnnGetPlayer()->ignorePhysics = false;
	AnnGetPhysicsEngine()->changeGravity(AnnVect3::ZERO);

	float ev = 1;
	AnnGetSceneryManager()->setExposure(ev, ev, ev);
	AnnGetSceneryManager()->setSkyColor(AnnColor(0, 0, 0), 0);

	auto sun = addLightObject();
	sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
	sun->setDirection({ 0, 1, 0.5f });

	//Get splashscreen material
	auto unlit			 = static_cast<Ogre::HlmsUnlit*>(AnnGetVRRenderer()->getRoot()->getHlmsManager()->getHlms(Ogre::HLMS_UNLIT));
	auto splashDatablock = static_cast<Ogre::HlmsUnlitDatablock*>(unlit->getDatablock("Splash"));
	if(!splashDatablock) splashDatablock = createSplashDatablock(unlit);

	//Get splashscreen texture
	auto textureManager = Ogre::TextureManager::getSingletonPtr();
	auto texture		= textureManager->getByName(splashImageName);
	if(!texture) texture = textureManager->load(splashImageName, AnnGetResourceManager()->getDefaultResourceGroupName(), Ogre::TEX_TYPE_2D, 0, 1, false, Ogre::PF_UNKNOWN, true);
	if(!texture) throw AnnInitializationError(ANN_ERR_NOTINIT, "Texture not found for splash " + splashImageName);

	//Set datablock parameters
	splashDatablock->setColour(Ogre::ColourValue::White * 1);
	splashDatablock->setTexture(Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE, 0, texture);

	createSplashCurvedPlane();
}

void AnnSplashLevel::setBGM(std::string soundfileName, bool preload)
{
	if(preload) AnnGetAudioEngine()->preLoadBuffer(soundfileName);
	bgmName = soundfileName;
	hasBGM  = true;
}

void AnnSplashLevel::runLogic()
{
	Splash->setPosition(AnnGetPlayer()->getPosition() + AnnVect3{ 0, 0, -10 });

	//If start time not set yet
	if(startTime == -1)
	{
		//The app is "not visible" if the user isn't wearing the HMD, or if a system menu is up
		if(AnnGetEngine()->appVisibleInHMD())
		{
			//This set the "startTime" variable, preventing this piece of code to be ran twice
			AnnDebug() << "Starting time at : " << AnnGetEngine()->getTimeFromStartUp();
			startTime = float(AnnGetEngine()->getTimeFromStartUp());

			//If you put some background music or sound for the splash-screen, we start it
			if(hasBGM)
			{
				AnnGetAudioEngine()->playBGM(bgmName);
			}
		}
		else
		{
			return;
		}
	}

	//Run the following only if you set a "next" level to jump to
	auto nextLevel = next.lock();
	if(nextLevel && AnnGetEngine()->getTimeFromStartUp() - startTime > timeout)
	{
		startTime = -1;
		AnnGetEngine()->getLevelManager()->switchToLevel(nextLevel);
	}
}

void AnnSplashLevel::unload()
{
	//Normal level unload
	AnnLevel::unload();

	AnnDebug() << "Removing object from scene";
	auto smgr(AnnGetEngine()->getSceneManager());
	smgr->destroySceneNode(Splash);
	Ogre::MaterialManager::getSingleton().remove("Splash");

	AnnDebug() << "Restore Player's normal state : ";
	AnnGetPlayer()->ignorePhysics = false;
	AnnGetPlayer()->setPosition(AnnVect3(0, 0, 10));
	AnnGetPlayer()->resetPlayerPhysics();
}

void AnnSplashLevel::setNextLevel(std::shared_ptr<AnnLevel> level)
{
	next = level;
}

void AnnSplashLevel::setTimeout(float time)
{
	if(time > 0) timeout = 1000 * time;
}

void AnnSplashLevel::setTimeoutMillisec(unsigned time)
{
	timeout = float(time);
}
