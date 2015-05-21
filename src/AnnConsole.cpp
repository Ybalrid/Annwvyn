#include "stdafx.h"
#include "AnnConsole.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnConsole::AnnConsole() : 
	visibility(false),
	consoleNode(NULL)
{

	Ogre::MaterialPtr Console = Ogre::MaterialManager::getSingleton().create("Console", "General", true);
	Ogre::Technique* technique = Console.getPointer()->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	pass->setDepthFunction(Ogre::CompareFunction::CMPF_ALWAYS_PASS);

	/*
	 * The displaySurface is a perfect rectangle drawn by 2 polygons. The position in object-space are defined as folowing 
	 * on the "points" array : 
	 *  0 +----+ 2
	 *    |   /|
	 *	  |  / |
	 *    | /  |
     *	  |/   |
	 *	1 +----+ 3
	 * Note that the rectangle is actually wider that it's height but I'm not verry good with ASCII art so... 
	 */
	points[0] = AnnVect3(-1, .5, 0);
	points[1] = AnnVect3(-1,-.5, 0);
	points[2] = AnnVect3( 1, .5, 0);
	points[3] = AnnVect3( 1,-.5, 0);
	
	textCoord[0] = AnnVect2(0,0);
	textCoord[1] = AnnVect2(0,1);
	textCoord[2] = AnnVect2(1,0);
	textCoord[3] = AnnVect2(1,1);

	std::cerr << "Creating on screen console " << (void*)this << std::endl;
	for(size_t i(0); i < CONSOLE_BUFFER; i++)
		buffer[i] = "";
	//creatre a quad
	displaySurface = AnnEngine::Instance()->getSceneManager()->createManualObject("DISPLAY_SURFACE");
	displaySurface->begin("Console", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	for(char i(0); i < 4; i++)
	{
		displaySurface->position(points[i]);
		displaySurface->textureCoord(textCoord[i]);
	}

	displaySurface->end();
	
	//create a node
	consoleNode = AnnEngine::Instance()->getCamera()->createChildSceneNode();
	//attach the quad
	consoleNode->attachObject(displaySurface);
	consoleNode->setPosition(0,0,-1);
	//displaySurface->setRenderQueueGroup(Ogre::uint8(-1));//draw that object in last position
	consoleNode->setVisible(visibility);

	//create a manual texture
	//put the texture to the quad with a material that don't care about lighting and shadows
	//init the texture with black

	if(!Ogre::FontManager::getSingletonPtr())
	{
		std::cerr << "fontManager not usable" << std::endl;
		new Ogre::FontManager();
	}

	font = Ogre::FontManager::getSingleton().create("VeraMono","ANNWVYN_DEFAULT");
	texture = TextureManager::getSingleton().createManual("Write Texture","ANNWVYN_DEFAULT",TEX_TYPE_2D, 1024, 512, MIP_UNLIMITED , PF_X8R8G8B8, Ogre::TU_AUTOMIPMAP|Ogre::TU_RENDERTARGET);

	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource("VeraMono.ttf");
	font->setTrueTypeResolution(96);
	font->setTrueTypeSize(16);
	//background = TextureManager::getSingleton().load("Background.png","ANNWVYN_DEFAULT");
	//Ogre::TextureUnitState* tus = pass->createTextureUnitState();
	//tus->setTexture(background);
	background = TextureManager::getSingleton().load("background.png","ANNWVYN_DEFAULT");
	Ogre::TextureUnitState* displaySurfaceTextureUniteState = pass->createTextureUnitState();
	displaySurfaceTextureUniteState->setTexture(texture);
	
}

void AnnConsole::append(std::string str)
{
	for(size_t i(1); i < CONSOLE_BUFFER; i++)
		buffer[i-1] = buffer[i];

	buffer[CONSOLE_BUFFER-1] = str;
	update();
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
	std::stringstream content;
	for(size_t i(0); i < CONSOLE_BUFFER; i++)
		content << buffer[i] << std::endl;

	Ogre::String textToDisplay = content.str();
	//redraw the texture
	//reaply texture to floating quad
	 
	//texture->getBuffer()->unlock();
	//background->getBuffer()->unlock();
	texture->getBuffer()->blit(background->getBuffer(),Ogre::Image::Box(0,0,1024,512),Ogre::Image::Box(0,0,1024,512));
	//texture->getBuffer()->blit(background->getBuffer());

	WriteToTexture(textToDisplay,texture,Image::Box(0,0,1024,512),font.getPointer(),ColourValue(0, 1.0, 0, 1.0),'l',false);

}

void AnnConsole::WriteToTexture(const Ogre::String &str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const ColourValue &color, char justify,  bool wordwrap)
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
 
                //abort - net enough space to draw
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