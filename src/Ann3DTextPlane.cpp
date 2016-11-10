#include "stdafx.h"
#include "Ann3DTextPlane.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

void WriteToTexture(const string &str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify = 'l', bool wordwrap = true)
{
	using namespace Ogre;

	if (destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if (destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if (!font->isLoaded())
		font->load();

	TexturePtr fontTexture = TexturePtr(TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName()));

	HardwarePixelBufferSharedPtr fontBuffer = fontTexture->getBuffer();
	HardwarePixelBufferSharedPtr destBuffer = destTexture->getBuffer();

	PixelBox destPb = destBuffer->lock(destRectangle, HardwareBuffer::HBL_NORMAL);

	// The font texture buffer was created write only...so we cannot read it back :o). One solution is to copy the buffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a buffer
	size_t nBuffSize = fontBuffer->getSizeInBytes();
	uint8* buffer = static_cast<uint8*>(calloc(nBuffSize, sizeof(uint8)));

	// create pixel box using the copy of the buffer
	PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(), fontBuffer->getDepth(), fontBuffer->getFormat(), buffer);
	fontBuffer->blitToMemory(fontPb);

	uint8* fontData = static_cast<uint8*>(fontPb.data);
	uint8* destData = static_cast<uint8*>(destPb.data);

	const size_t fontPixelSize = PixelUtil::getNumElemBytes(fontPb.format);
	const size_t destPixelSize = PixelUtil::getNumElemBytes(destPb.format);

	const size_t fontRowPitchBytes = fontPb.rowPitch * fontPixelSize;
	const size_t destRowPitchBytes = destPb.rowPitch * destPixelSize;

	Box *GlyphTexCoords;
	GlyphTexCoords = new Box[str.size()];

	Font::UVRect glypheTexRect;
	size_t charheight = 0;
	size_t charwidth = 0;

	for (unsigned int i = 0; i < str.size(); i++)
	{
		if ((str[i] != '\t') && (str[i] != '\n') && (str[i] != ' '))
		{
			glypheTexRect = font->getGlyphTexCoords(str[i]);
			GlyphTexCoords[i].left = glypheTexRect.left * fontTexture->getSrcWidth();
			GlyphTexCoords[i].top = glypheTexRect.top * fontTexture->getSrcHeight();
			GlyphTexCoords[i].right = glypheTexRect.right * fontTexture->getSrcWidth();
			GlyphTexCoords[i].bottom = glypheTexRect.bottom * fontTexture->getSrcHeight();

			if (GlyphTexCoords[i].getHeight() > charheight)
				charheight = GlyphTexCoords[i].getHeight();
			if (GlyphTexCoords[i].getWidth() > charwidth)
				charwidth = GlyphTexCoords[i].getWidth();
		}
	}

	size_t spacewidth;
	//get the size of the glyph '0'
	glypheTexRect = font->getGlyphTexCoords('0');
	Box spaceBox;
	spaceBox.left = glypheTexRect.left * fontTexture->getSrcWidth();
	spaceBox.right = glypheTexRect.right * fontTexture->getSrcWidth();
	spacewidth = spaceBox.getWidth();

	//if not mono-spaced
	if (spacewidth != charwidth) spacewidth = size_t(float(spacewidth) * 0.5f);

//	Annwvyn::AnnDebug() << "Width of a space : " << space-width;

	size_t cursorX = 0;
	size_t cursorY = 0;
	size_t lineend = destRectangle.getWidth();
	bool carriagreturn = true;
	for (unsigned int strindex = 0; strindex < str.size(); strindex++)
	{
		switch (str[strindex])
		{
			case ' ': cursorX += spacewidth;  break;
			case '\t':cursorX += charwidth * 3; break;
			case '\n':cursorY += charheight; carriagreturn = true; break;
			default:
			{
				//wrapping
				if ((cursorX + GlyphTexCoords[strindex].getWidth() > lineend) && !carriagreturn)
				{
					cursorY += charheight;
					carriagreturn = true;
				}

				//justify
				if (carriagreturn)
				{
					size_t l = strindex;
					size_t textwidth = 0;
					size_t wordwidth = 0;

					while ((l < str.size()) && (str[l] != '\n'))
					{
						wordwidth = 0;

						switch (str[l])
						{
							case ' ': wordwidth = spacewidth; ++l; break;
							case '\t': wordwidth = charwidth * 3; ++l; break;
							case '\n': l = str.size();
							default: break;
						}

						if (wordwrap)
							while ((l < str.size()) && (str[l] != ' ') && (str[l] != '\t') && (str[l] != '\n'))
							{
								wordwidth += GlyphTexCoords[l].getWidth();
								++l;
							}
						else
						{
							wordwidth += GlyphTexCoords[l].getWidth();
							l++;
						}

						if ((textwidth + wordwidth) <= destRectangle.getWidth())
							textwidth += (wordwidth);
						else
							break;
					}

					if ((textwidth == 0) && (wordwidth > destRectangle.getWidth()))
						textwidth = destRectangle.getWidth();

					switch (justify)
					{
						case 'c':    cursorX = (destRectangle.getWidth() - textwidth) / 2;
							lineend = destRectangle.getWidth() - cursorX;
							break;

						case 'r':    cursorX = (destRectangle.getWidth() - textwidth);
							lineend = destRectangle.getWidth();
							break;

						default:    cursorX = 0;
							lineend = textwidth;
							break;
					}

					carriagreturn = false;
				}

				//abort - net enough space to draw
				if ((cursorY + charheight) > destRectangle.getHeight())
					goto stop;

				//draw pixel by pixel
				for (size_t i = 0; i < GlyphTexCoords[strindex].getHeight(); i++)
					for (size_t j = 0; j < GlyphTexCoords[strindex].getWidth(); j++)
					{
						float alpha = color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize + 1] / 255.0f);
						float invalpha = 1.0 - alpha;
						size_t offset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;

						ColourValue pix;
						PixelUtil::unpackColour(&pix, destPb.format, &destData[offset]);
						pix = (pix * invalpha) + (color * alpha);
						PixelUtil::packColour(pix, destPb.format, &destData[offset]);
					}

				cursorX += GlyphTexCoords[strindex].getWidth();
			}//default
		}//switch
	}//for

stop:
	delete[] GlyphTexCoords;

	destBuffer->unlock();

	// Free the memory allocated for the buffer
	free(buffer);
}

Ann3DTextPlane::Ann3DTextPlane(float w, float h, string str, int size, float resolution, string fName, string TTF) :
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
	AnnDebug() << "3D Text plane created";
	AnnDebug() << "Size is : " << width << "x" << height;
	if (caption.empty())
		AnnDebug() << "No caption yet";
	else
		AnnDebug() << "caption : " << caption.c_str();

	if (fontName.empty())
	{
		AnnDebug() << "You need set a font to initialize a text render plane";
		exit(ANN_ERR_NOTINIT);
	}

	AnnDebug() << "font : " << fontName;
	needUpdating = true;

	resolutionFactor /= dpi2dpm;
	AnnDebug() << "Resolution factor in dot per meters " << resolutionFactor;
	AnnDebug() << "Texture resolution is : " << size_t(w * resolutionFactor) << "x" << size_t(h * resolutionFactor);
	AnnDebug() << "Font resolution in DPI is : " << dpi;

	calculateVerticesForPlaneSize();

	//TOTO put this in a separate method
	auto smgr(AnnGetEngine()->getSceneManager());
	node = smgr->getRootSceneNode()->createChildSceneNode();
	renderPlane = smgr->createManualObject();

	createMaterial();

	renderPlane->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	for (char i(0); i < 4; i++)
	{
		renderPlane->position(vertices[i]);
		renderPlane->textureCoord(textureCoords[i]);
	}

	renderPlane->end();

	node->attachObject(renderPlane);
	//end of the thing that should be in it's own method

	//Create or retrieve the font from the font manager. Will also create the font manager if not available yet (unlikely since the font manager is initialized by the on screen console)
	if (!fontName.empty())
	{
		//Be sure that the font manager exist, if not, instantiate one (singleton)
		// ReSharper disable once CppNonReclaimedResourceAcquisition - Resource cleared by Ogre itself.
		if (!Ogre::FontManager::getSingletonPtr()) new Ogre::FontManager();

		//Attempt to retrieve the font
		font = Ogre::FontManager::getSingleton().getByName(fontName);

		//Need to create the font
		if (font.isNull())
		{
			//Create the font
			font = Ogre::FontManager::getSingleton().create(fontName, "ANNWVYN_CORE");

			//Load true-type file
			font->setType(Ogre::FontType::FT_TRUETYPE);
			font->setSource(fontTTF);

			//Set important parameters
			font->setTrueTypeResolution(dpi);
			font->setTrueTypeSize(size);
		}
	}

	//If there's text to draw, draw it
	if (!caption.empty()) update();
}

Ann3DTextPlane::~Ann3DTextPlane()
{
	AnnDebug() << "Destructing a 3D Text plane!";
	auto smgr = AnnGetEngine()->getSceneManager();

	node->detachObject(renderPlane);
	smgr->destroyManualObject(renderPlane);

	Ogre::MaterialManager::getSingleton().remove(materialName);

	auto textureName = texture->getName();
	Ogre::TextureManager::getSingleton().remove(textureName);

	if (!bgTexture.isNull())
	{
		textureName = texture->getName();
		Ogre::TextureManager::getSingleton().remove(textureName);
	}

	smgr->destroySceneNode(node);
	node = nullptr;
}

void Ann3DTextPlane::setCaption(string newCaption)
{
	caption = newCaption;
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
	AnnDebug() << "materialName : " << materialName;

	auto material = Ogre::MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	auto materialPass = material.getPointer()->getTechnique(0)->getPass(0);
	materialPass->setCullingMode(Ogre::CullingMode::CULL_NONE);
	materialPass->setAlphaRejectSettings(Ogre::CompareFunction::CMPF_EQUAL, 255, true);
	auto renderPlaneTextureUnitState = materialPass->createTextureUnitState();

	texture = Ogre::TextureManager::getSingleton().createManual(generateRandomString(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, width * resolutionFactor, height * resolutionFactor, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
	renderPlaneTextureUnitState->setTexture(texture);
}

void Ann3DTextPlane::autoUpdateCheck()
{
	if (autoUpdate) update();
}

void Ann3DTextPlane::generateMaterialName()
{
	materialName = generateRandomString(materialNameLen);
}

string Ann3DTextPlane::generateRandomString(size_t len)
{
	string s;
	string textSpace("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	for (size_t i(0); i < len; i++) s += textSpace[rand() % textSpace.length()];
	return s;
}

void Ann3DTextPlane::setTextColor(AnnColor color)
{
	textColor.setRed(color.getRed());
	textColor.setGreen(color.getGreen());
	textColor.setBlue(color.getBlue());
	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::setBackgroundColor(AnnColor color)
{
	bgColor.setRed(color.getRed());
	bgColor.setGreen(color.getGreen());
	bgColor.setBlue(color.getBlue());
	bgColor.setAlpha(color.getAlpha());
	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::update()
{
	if (!needUpdating) return;
	renderText();
	needUpdating = false;
}

void Ann3DTextPlane::setPosition(AnnVect3 p)
{
	node->setPosition(p);
}

void Ann3DTextPlane::setOrientation(AnnQuaternion q)
{
	node->setOrientation(q);
}

void Ann3DTextPlane::setTextAlign(TextAlign talign)
{
	align = talign;
}

AnnVect3 Ann3DTextPlane::getPosition()
{
	if (node) return node->getPosition();
	return AnnVect3();
}

AnnQuaternion Ann3DTextPlane::getOrientation()
{
	if (node) return node->getOrientation();
	return AnnQuaternion();
}

void Ann3DTextPlane::setMargin(float m)
{
	margin = m;
	pixelMargin = resolutionFactor*margin;

	needUpdating = true;
	autoUpdateCheck();
}

void Ann3DTextPlane::setBackgroundImage(string imgName)
{
	bgTexture = Ogre::TextureManager::getSingleton().load(imgName, "ANNWVYN_CORE");
	useImageAsBackground = true;
}

void Ann3DTextPlane::renderText()
{
	clearTexture();
	WriteToTexture(caption,
				   texture,

				   Ogre::Image::Box(pixelMargin, pixelMargin,
				   width * resolutionFactor - pixelMargin,
				   height * resolutionFactor - pixelMargin),

				   font.getPointer(),
				   textColor.getOgreColor(),
				   align,
				   true);

	needUpdating = false;
}

void Ann3DTextPlane::clearTexture()
{
	if (useImageAsBackground)
	{
		//Clear the texture with the content of the background
		bgTexture->copyToTexture(texture);
	}
	else
	{
		auto textureBuffer = texture->getBuffer();
		const auto w = textureBuffer->getWidth();
		const auto h = textureBuffer->getHeight();

		Ogre::Image::Box imageBox(0, 0, w, h);
		auto pixelBox = textureBuffer->lock(imageBox, Ogre::HardwareBuffer::HBL_NORMAL);

		for (size_t j(0); j < h; j++) for (size_t i(0); i < w; i++)
			pixelBox.setColourAt(bgColor.getOgreColor(), i, j, 0);

		textureBuffer->unlock();
	}
}