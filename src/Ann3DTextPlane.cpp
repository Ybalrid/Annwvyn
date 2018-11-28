// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <OgreVector2.h>

#include "Ann3DTextPlane.hpp"
#include "AnnTypes.h"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"
#include <OgreHlms.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsPbsDatablock.h>

using namespace Annwvyn;
using namespace std;

void Ann3DTextPlane::createFont(const int& size)
{
	//Create the font
	font = Ogre::FontManager::getSingleton().create(fontName, AnnResourceManager::getDefaultResourceGroupName());

	//Load true-type file
	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource(fontTTF);
	font->setTrueTypeResolution(static_cast<unsigned int>(dpi));
	font->setTrueTypeSize(float(size));
}

void Ann3DTextPlane::createPlane()
{
	auto smgr(AnnGetEngine()->getSceneManager());
	node		= smgr->getRootSceneNode()->createChildSceneNode();
	renderPlane = smgr->createManualObject();

	createMaterial();

	renderPlane->begin(materialName, Ogre::OT_TRIANGLE_STRIP);

	for(char i(0); i < 4; i++)
	{
		renderPlane->position(vertices[i]);
		renderPlane->normal(0, 0, 1);
		renderPlane->tangent(1, 0, 0);
		renderPlane->index(i);
		renderPlane->textureCoord(textureCoords[i]);
	}

	renderPlane->end();

	node->attachObject(renderPlane);
}

Ann3DTextPlane::Ann3DTextPlane(const float& w, const float& h, const string& str, const int& size, const float& resolution, const string& fName, const string& TTF) :
 fontName(fName),
 fontTTF(TTF),
 caption(str),
 width(w),
 height(h),
 resolutionFactor(resolution),
 textColor(AnnColor(1, 0, 0)),
 bgColor(AnnColor(0, 0, 0, 0)),
 autoUpdate(false),
 fontSize(size),
 dpi(resolution),
 pixelMargin(0),
 margin(0),
 useImageAsBackground(false)
{
	AnnDebug() << width << "x" << height << " " << dpi << "dpi 3D Text plane created";
	if(caption.empty())
		AnnDebug() << "No caption yet";

	if(fontName.empty())
	{
		AnnDebug() << "You need set a font to initialize a text render plane";
		throw AnnInitializationError(ANN_ERR_NOTINIT, "3D Text plane initialized without a valid font");
	}

	needUpdating = true;
	resolutionFactor /= dpi2dpm;

	calculateVerticesForPlaneSize();
	createPlane();

	//Create or retrieve the font from the font manager. Will also create the font manager if not available yet (unlikely since the font manager is initialized by the on screen console)
	if(!fontName.empty())
	{
		//Be sure that the font manager exist, if not, instantiate one (singleton)
		// ReSharper disable once CppNonReclaimedResourceAcquisition - Resource cleared by Ogre itself.
		if(!Ogre::FontManager::getSingletonPtr()) OGRE_NEW Ogre::FontManager();

		//Attempt to retrieve the font
		font = Ogre::FontManager::getSingleton().getByName(fontName);

		//Need to create the font
		if(font.isNull())
			createFont(size);
	}

	//If there's text to draw, draw it
	if(!caption.empty()) update();
}

Ann3DTextPlane::~Ann3DTextPlane()
{
	AnnDebug() << "Destructing a 3D Text plane!";
	const auto smgr = AnnGetEngine()->getSceneManager();

	node->detachObject(renderPlane);
	smgr->destroyManualObject(renderPlane);
	Ogre::MaterialManager::getSingleton().remove(materialName);
	Ogre::TextureManager::getSingleton().remove(texture->getName());
	texture.setNull();

	if(!bgTexture.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(bgTexture->getName());
		bgTexture.setNull();
	}

	smgr->destroySceneNode(node);
	node = nullptr;
}

void Ann3DTextPlane::setCaption(const std::string& newCaption)
{
	caption		 = newCaption;
	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::setAutoUpdate(bool state)
{
	autoUpdate = state;
}

void Ann3DTextPlane::calculateVerticesForPlaneSize()
{
	xOffset = { width / 2.0f };
	yOffset = { height / 2.0f };

	/*
	* The plane is a perfect rectangle drawn by 2 polygons (triangles).
	* The position in object-space are defined as following
	* on the "points" array :
	*  0 +---------------+ 2
	*    |           /   |
	*    |       _ /     |
	*    |     /         |
	*    |  /            |
	*  1 +----------------+ 3
	* Texture coordinates are also mapped. To display properly. The aspect ratios needs to match
	*/

	vertices[0] = AnnVect3(-xOffset, +yOffset, 0);
	vertices[1] = AnnVect3(-xOffset, -yOffset, 0);
	vertices[2] = AnnVect3(+xOffset, +yOffset, 0);
	vertices[3] = AnnVect3(+xOffset, -yOffset, 0);
}

void Ann3DTextPlane::createMaterial()
{
	generateMaterialName();

	//We want theses objects to react to the light and be present in the scene. We will create some kind of dielectric material (aka paint)
	auto hlms = dynamic_cast<Ogre::HlmsPbs*>(AnnGetVRRenderer()->getRoot()->getHlmsManager()->getHlms(Ogre::HLMS_PBS));

	if(!hlms)
	{
		throw AnnInitializationError(ANN_ERR_NOTINIT, "3D text plane cannot be created: cannot get HLMS_PBS");
	}

	//We do know that hlms is an hlmsPbs
	auto datablock = static_cast<Ogre::HlmsPbsDatablock*>(hlms->createDatablock(materialName, materialName, {}, {}, {}));

	//Note : if we want to make this run in "unlit" we will need to :
	// 1) create it as unlit (no kidding)
	// 2) a normal texture is good, no need of a single slice of an array
	// 3) need to set a high "diffuse color" underneath the actual texture
	// 4) no "2 sized lighting", set culling to none.
	// 5) alpha blending should work the same

	texture = Ogre::TextureManager::getSingleton() //Texture is a single slice array
				  .createManual(AnnGetStringUtility()->getRandomString(),
								Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
								Ogre::TEX_TYPE_2D_ARRAY,
								Ogre::uint(width * resolutionFactor),
								Ogre::uint(height * resolutionFactor),
								1,
								12,
								Ogre::PF_R8G8B8A8,
								Ogre::TU_RENDERTARGET | Ogre::TU_AUTOMIPMAP);

	datablock->setTexture(Ogre::PBSM_DIFFUSE, 0, texture);
	datablock->setRoughness(0.001);
	datablock->setSpecular({ 0.25, 0.25, 0.25 });
	//Calling this will by default use the "transparent" (not fade) transparency mode, will read alpha from texture and will update the blendblock
	datablock->setTransparency(1);
	datablock->setTwoSidedLighting(true);
}

void Ann3DTextPlane::autoUpdateCheck()
{
	if(autoUpdate) update();
}

void Ann3DTextPlane::generateMaterialName()
{
	materialName = AnnGetStringUtility()->getRandomString(materialNameLen);
}

void Ann3DTextPlane::setTextColor(const AnnColor& color)
{
	textColor	= color;
	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::setBackgroundColor(const AnnColor& color)
{
	bgColor		 = color;
	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::update()
{
	if(!needUpdating) return;
	renderText();
	needUpdating = false;
}

void Ann3DTextPlane::setPosition(AnnVect3 p)
{
	node->setPosition(p);
}

void Ann3DTextPlane::setOrientation(AnnQuaternion q)
{
	node->setOrientation(static_cast<Ogre::Quaternion>(q));
}

void Ann3DTextPlane::setTextAlign(TextAlign talign)
{
	align = talign;
}

AnnVect3 Ann3DTextPlane::getPosition()
{
	if(node) return node->getPosition();
	return AnnVect3();
}

AnnQuaternion Ann3DTextPlane::getOrientation()
{
	if(node) return node->getOrientation();
	return AnnQuaternion();
}

void Ann3DTextPlane::setMargin(float m)
{
	margin		= m;
	pixelMargin = static_cast<unsigned int>(resolutionFactor * margin);

	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::setBackgroundImage(const string& imgName)
{
	bgTexture			 = Ogre::TextureManager::getSingleton().load(imgName, AnnResourceManager::getDefaultResourceGroupName());
	useImageAsBackground = true;
}

void Ann3DTextPlane::renderText()
{
	clearTexture();
	AnnConsole::WriteToTexture(caption,
							   texture,

							   Ogre::Image::Box { pixelMargin, pixelMargin, uint32_t(width * resolutionFactor - pixelMargin), uint32_t(height * resolutionFactor - pixelMargin) },

							   font.getPointer(),
							   textColor.getOgreColor(),
							   char(align),
							   true);

	needUpdating = false;
}

void Ann3DTextPlane::clearTexture()
{
	if(useImageAsBackground)
	{
		//Clear the texture with the content of the background
		bgTexture->copyToTexture(texture);
	}
	else
	{
		/*TODO ISSUE try to use Ogre v2 here, not v1 interfaces*/

		auto textureBuffer = texture->getBuffer();
		const auto w	   = textureBuffer->getWidth();
		const auto h	   = textureBuffer->getHeight();

		Ogre::Image::Box imageBox(0, 0, w, h);
		auto pixelBox = textureBuffer->lock(imageBox, Ogre::v1::HardwareBuffer::HBL_NORMAL);

		for(size_t j(0); j < h; j++)
			for(size_t i(0); i < w; i++)
				pixelBox.setColourAt(bgColor.getOgreColor(), i, j, 0);

		textureBuffer->unlock();
	}
}
