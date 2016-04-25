#include "stdafx.h"
#include "AnnConsole.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnConsole::AnnConsole() : 
	modified(false),
	visibility(false),
	consoleNode(NULL),
	offset(0,0.125f, -0.75f)
{
	std::cerr << "Creating on screen console " << (void*)this << std::endl;
	//Define the custom material
	Ogre::MaterialPtr Console = Ogre::MaterialManager::getSingleton().create("Console", "General", true);
	Ogre::Technique* technique = Console.getPointer()->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	//pass->setDepthFunction(Ogre::CompareFunction::CMPF_ALWAYS_PASS);

	/*
	* The displaySurface is a perfect rectangle drawn by 2 polygons (tiangles). The position in object-space are defined as folowing 
	* on the "points" array : 
	*  0 +---------------+ 2
	*    |           /   |
	*    |        /      |
	*    |     /         |
	*    |  /            |
	*  1 +----------------+ 3
	* Texture coordinates are also mapped. To display properly, the texture should respect the same aspect ratio (2:1)
	*/

	//Define vertex data
	points[0] = AnnVect3(-0.5,  .25, 0);
	points[1] = AnnVect3(-0.5, -.25, 0);
	points[2] = AnnVect3( 0.5,  .25, 0);
	points[3] = AnnVect3( 0.5, -.25, 0);

	//Define texture coordinates
	textCoord[0] = AnnVect2(0, 0);
	textCoord[1] = AnnVect2(0, 1);
	textCoord[2] = AnnVect2(1, 0);
	textCoord[3] = AnnVect2(1, 1);

	//creatre the quad itself
	displaySurface = AnnGetEngine()->getSceneManager()->createManualObject("DISPLAY_SURFACE");
	displaySurface->begin("Console", Ogre::RenderOperation::OT_TRIANGLE_STRIP);//Srip of triangle : Define a triangle then add them by points

	//Add the four vertices. This will directly describe two Triangles
	for(char i(0); i < 4; i++)
	{
		displaySurface->position(points[i]);
		displaySurface->textureCoord(textCoord[i]);
	}

	//Object compleate
	displaySurface->end();

	//create a node child to the camera here : 
	consoleNode = AnnEngine::Instance()->getCamera()->createChildSceneNode();

	//attach The object
	consoleNode->attachObject(displaySurface);

	//set the camera relative position
	consoleNode->setPosition(offset);

	//Make sure the object is the last thing rendered (to be on top of everyting
	//displaySurface->setRenderQueueGroup(Ogre::uint8(-1));
	//Set the visibility state
	consoleNode->setVisible(visibility);

	//Create a The font
	if(!Ogre::FontManager::getSingletonPtr()) //The FontManager isn't initialized by default
	{
		//Create a FontManager
		std::cerr << "FontManager not usable yet. Initializing a new FontManager" << std::endl;
		new Ogre::FontManager();
	}

	//Create a manual font
	font = Ogre::FontManager::getSingleton().create("VeraMono", "ANNWVYN_DEFAULT");

	//Load the VeraMono.ttf file
	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource("VeraMono.ttf");
	font->setTrueTypeResolution(96);
	font->setTrueTypeSize(BASE/32); //Size of font is relative to the size of the pixelbuffer (texture)

	//Aspect ration of the console is 2:1. The actuall size of texture is 2*BASE x BASE
	//Create an map the texture to the displaySurface
	texture = Ogre::TextureManager::getSingleton().createManual("Write Texture", "ANNWVYN_DEFAULT", Ogre::TEX_TYPE_2D, 2 * BASE, BASE, Ogre::MIP_UNLIMITED, Ogre::PF_X8R8G8B8, Ogre::TU_AUTOMIPMAP | Ogre::TU_RENDERTARGET);
	Ogre::TextureUnitState* displaySurfaceTextureUniteState = pass->createTextureUnitState();
	displaySurfaceTextureUniteState->setTexture(texture);

	//Load background texture to a buffer
	background = Ogre::TextureManager::getSingleton().load("background.png", "ANNWVYN_DEFAULT");

	//Initialize the text buffer.
	//CONSOLE_BUFFER is the number of lines to keep in memory and to load on the texture.
	//Text is drawn from the 1st line. The number of line define how mani lines are visible on the console
	for(size_t i(0); i < CONSOLE_BUFFER; i++)
		buffer[i] = "";
}

void AnnConsole::append(std::string str)
{
	//Copy with an offset of 1 the buffer content
	for(size_t i(1); i < CONSOLE_BUFFER; i++)
		buffer[i-1] = buffer[i];

	//Write the texture to the buffer
	buffer[CONSOLE_BUFFER-1] = str;

	//The console will be redrawn next frame
	modified = true;
}

void AnnConsole::setVisible(bool state)
{
	visibility = state;
	consoleNode->setVisible(visibility);
}

void AnnConsole::toogle()
{
	setVisible(!visibility);
}

void AnnConsole::update()
{
	//Updated
	modified = false;
	//Get the content of the buffer into a static string
	std::stringstream content;
	for(size_t i(0); i < CONSOLE_BUFFER; i++)
		content << buffer[i] << std::endl;
	Ogre::String textToDisplay = content.str();

	//Erase texture by filling it with the background buffer. 
	//Not that apparently this is the origin of a bug that cause the next viewport update to be poluated with color from background buffer
	background->copyToTexture(texture);

	//Write text to texture
	WriteToTexture
		(textToDisplay,																//Text
		texture,																	//Texture
		Ogre::Image::Box(0 + MARGIN, 0 + MARGIN, 2 * BASE - MARGIN, BASE - MARGIN),		//Part of the pixel buffer to write to
		font.getPointer(),															//Font
		Ogre::ColourValue::Black,															//Color
		'l',																		//Alignement
		false);																		//LineWrap
}

void AnnConsole::WriteToTexture(const Ogre::String &str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify,  bool wordwrap)
{
	using namespace Ogre;

	if (destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if (destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if (!font->isLoaded())
		font->load();

	TexturePtr fontTexture = (TexturePtr) TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName());

	HardwarePixelBufferSharedPtr fontBuffer = fontTexture->getBuffer();
	HardwarePixelBufferSharedPtr destBuffer = destTexture->getBuffer();

	PixelBox destPb = destBuffer->lock(destRectangle,HardwareBuffer::HBL_NORMAL);

	// The font texture buffer was created write only...so we cannot read it back :o). One solution is to copy the buffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a buffer
	size_t nBuffSize = fontBuffer->getSizeInBytes();
	uint8* buffer = (uint8*)calloc(nBuffSize, sizeof(uint8)); 

	// create pixel box using the copy of the buffer
	PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(),fontBuffer->getDepth(), fontBuffer->getFormat(), buffer);          
	fontBuffer->blitToMemory(fontPb);

	uint8* fontData = static_cast<uint8*>( fontPb.data );
	uint8* destData = static_cast<uint8*>( destPb.data );

	const size_t fontPixelSize = PixelUtil::getNumElemBytes(fontPb.format);
	const size_t destPixelSize = PixelUtil::getNumElemBytes(destPb.format);

	const size_t fontRowPitchBytes = fontPb.rowPitch * fontPixelSize;
	const size_t destRowPitchBytes = destPb.rowPitch * destPixelSize;

	Box *GlyphTexCoords;
	GlyphTexCoords = new Box[str.size()];

	Font::UVRect glypheTexRect;
	size_t charheight = 0;
	size_t charwidth = 0;

	for(unsigned int i = 0; i < str.size(); i++)
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
		switch(str[strindex])
		{
		case ' ': cursorX += charwidth;  break;
		case '\t':cursorX += charwidth * 3; break;
		case '\n':cursorY += charheight; carriagreturn = true; break;
		default:
			{
				//wrapping
				if ((cursorX + GlyphTexCoords[strindex].getWidth()> lineend) && !carriagreturn )
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

					while( (l < str.size() ) && (str[l] != '\n)'))
					{        
						wordwidth = 0;

						switch (str[l])
						{
						case ' ': wordwidth = charwidth; ++l; break;
						case '\t': wordwidth = charwidth *3; ++l; break;
						case '\n': l = str.size();
						}

						if (wordwrap)
							while((l < str.size()) && (str[l] != ' ') && (str[l] != '\t') && (str[l] != '\n'))
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
					case 'c':    cursorX = (destRectangle.getWidth() - textwidth)/2;
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

				//abort - not enough space to draw
				if ((cursorY + charheight) > destRectangle.getHeight())
					goto stop;

				//draw pixel by pixel
				for (size_t i = 0; i < GlyphTexCoords[strindex].getHeight(); i++ )
					for (size_t j = 0; j < GlyphTexCoords[strindex].getWidth(); j++)
					{
						float alpha =  color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize +1 ] / 255.0);
						float invalpha = 1.0 - alpha;
						size_t offset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;
						ColourValue pix;
						PixelUtil::unpackColour(&pix,destPb.format,&destData[offset]);
						pix = (pix * invalpha) + (color * alpha);
						PixelUtil::packColour(pix,destPb.format,&destData[offset]);
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

bool AnnConsole::needUpdate()
{
	return modified && visibility;
}