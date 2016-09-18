#include "stdafx.h"
#include "Ann3DTextPlane.hpp"
#include "AnnLogger.hpp"
using namespace Ogre;
void WriteToTexture(const String &str, TexturePtr destTexture, Image::Box destRectangle, Font* font, const ColourValue &color, char justify = 'l', bool wordwrap = true)
{
	using namespace Ogre;

	if (destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if (destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if (!font->isLoaded())
		font->load();

	TexturePtr fontTexture = (TexturePtr)TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName());

	HardwarePixelBufferSharedPtr fontBuffer = fontTexture->getBuffer();
	HardwarePixelBufferSharedPtr destBuffer = destTexture->getBuffer();

	PixelBox destPb = destBuffer->lock(destRectangle, HardwareBuffer::HBL_NORMAL);

	// The font texture buffer was created write only...so we cannot read it back :o). One solution is to copy the buffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a buffer
	size_t nBuffSize = fontBuffer->getSizeInBytes();
	uint8* buffer = (uint8*)calloc(nBuffSize, sizeof(uint8));

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

	size_t cursorX = 0;
	size_t cursorY = 0;
	size_t lineend = destRectangle.getWidth();
	bool carriagreturn = true;
	for (unsigned int strindex = 0; strindex < str.size(); strindex++)
	{
		switch (str[strindex])
		{
			case ' ': cursorX += charwidth;  break;
			case '\t':cursorX += charwidth * 3; break;
			case '\n':cursorY += charheight; carriagreturn = true; break;
			default:
			{
				//wrapping
				if ((cursorX + GlyphTexCoords[strindex].getWidth()> lineend) && !carriagreturn)
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

					while ((l < str.size()) && (str[l] != '\n)'))
					{
						wordwidth = 0;

						switch (str[l])
						{
							case ' ': wordwidth = charwidth; ++l; break;
							case '\t': wordwidth = charwidth * 3; ++l; break;
							case '\n': l = str.size();
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
						float alpha = color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize + 1] / 255.0);
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
	free(buffer); buffer = 0;
}
using namespace Annwvyn;

Ann3DTextPlane::Ann3DTextPlane(float w,
							   float h,
							   float resolution, 
							   std::string str,
							   std::string fName) :
	width(w),
	height(h),
	resolutionFactor(resolution),
	caption(str),
	fontName(fName)
{
	AnnDebug() << "3D Text plane created";
	AnnDebug() << "Size is : " << width << "x" << height;
	if (caption.empty())
		AnnDebug() << "No caption yet";
	else
		AnnDebug() << "caption : " << caption;
	if (fontName.empty())
		AnnDebug() << "No font family yet";
	else
		AnnDebug() << "font : " << fontName;

	calculateVerticesForPlaneSize();
	
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

	if (!fontName.empty())
	{
		if (!Ogre::FontManager::getSingletonPtr()) new Ogre::FontManager();
		font = Ogre::FontManager::getSingleton().getByName(fontName);
		if (font.isNull())
		{
			font = Ogre::FontManager::getSingleton().create(fontName, "ANNWVYN_CORE");
			font->setType(Ogre::FontType::FT_TRUETYPE);
			font->setSource("VeraMono.ttf");
			font->setTrueTypeResolution(300);
			font->setTrueTypeSize(64);
		}
	}
	if (!caption.empty())
	{
		renderText();
	}
}

void Ann3DTextPlane::calculateVerticesForPlaneSize()
{
	xOffset = { width / 2.0f };
	yOffset = { height / 2.0f };

	/*
	* The plane is a perfect rectangle drawn by 2 polygons (triangles). 
	* The position in object-space are defined as folowing
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

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::Pass* materialPass = material.getPointer()->getTechnique(0)->getPass(0);
	materialPass->setCullingMode(Ogre::CullingMode::CULL_NONE);
	materialPass->setAlphaRejectSettings(Ogre::CompareFunction::CMPF_EQUAL, 255, true);
	Ogre::TextureUnitState* renderPlaneTextureUnitState = materialPass->createTextureUnitState();

	texture = Ogre::TextureManager::getSingleton().createManual(generateRandomString(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, width * resolutionFactor, height * resolutionFactor, 0, Ogre::PF_R8G8B8A8,  Ogre::TU_RENDERTARGET);
	renderPlaneTextureUnitState->setTexture(texture);
}

void Ann3DTextPlane::generateMaterialName()
{
	materialName = generateRandomString(materialNameLen);
}

std::string Ann3DTextPlane::generateRandomString(size_t len)
{
	std::string s;
	std::string textSpace("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	for (size_t i(0); i < len; i++) s += textSpace[rand() % textSpace.length()];
	return s; 
}

void Ann3DTextPlane::renderText()
{
	//clearTexture(); 
	WriteToTexture(caption, texture, Ogre::Image::Box(0, 0, width*resolutionFactor, height*resolutionFactor), font.getPointer(), Ogre::ColourValue::Red, 'c', true);
	needUpdating = false; 
//	texture->getBuffer()->getRenderTarget()->writeContentsToTimestampedFile("", "textDebug.png");
}

void Ann3DTextPlane::clearTexture()
{
	HardwarePixelBufferSharedPtr textureBuffer = texture->getBuffer();
	const size_t w = textureBuffer->getWidth();
	const size_t h = textureBuffer->getHeight();
	const size_t bufferSize = w*h;
	
	//Create an array of black, transparant R8G8B8A8 pixels to fill the texture
	unsigned char* clearBuffer = (unsigned char*)malloc(bufferSize * 4);
	for (size_t i(0); i < bufferSize*4; i++) clearBuffer[i] = 0;
	textureBuffer->writeData(0, bufferSize * 4, (void*)clearBuffer);
	free(clearBuffer);
}

using namespace Ogre;
