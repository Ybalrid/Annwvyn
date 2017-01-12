#include "stdafx.h"
#include "AnnConsole.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnConsole::AnnConsole() : AnnSubSystem("OnScreenConsole"),
modified(false),
consoleNode(nullptr),
offset(0, 0.125f, -0.75f),
openGL43plus(false),
visibility(false),
lastUpdate{ 0 },
refreshRate{ 1 / 15 }
{
	//Define the custom material
	Ogre::MaterialPtr Console = Ogre::MaterialManager::getSingleton().create("Console", "General", true);
	Ogre::Technique* technique = Console.getPointer()->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	//pass->setDepthFunction(Ogre::CompareFunction::CMPF_ALWAYS_PASS);

	/*
	* The displaySurface is a perfect rectangle drawn by 2 polygons (triangles). The position in object-space are defined as following
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
	points[0] = AnnVect3(-0.5, .25, 0);
	points[1] = AnnVect3(-0.5, -.25, 0);
	points[2] = AnnVect3(0.5, .25, 0);
	points[3] = AnnVect3(0.5, -.25, 0);

	//Define texture coordinates
	textCoord[0] = AnnVect2(0, 0);
	textCoord[1] = AnnVect2(0, 1);
	textCoord[2] = AnnVect2(1, 0);
	textCoord[3] = AnnVect2(1, 1);

	//create the quad itself
	displaySurface = AnnGetEngine()->getSceneManager()->createManualObject("DISPLAY_SURFACE");
	displaySurface->begin("Console", Ogre::RenderOperation::OT_TRIANGLE_STRIP);//Strip of triangle : Define a triangle then add them by points

	//Add the four vertices. This will directly describe two Triangles
	for (char i(0); i < 4; i++)
	{
		displaySurface->position(points[i]);
		displaySurface->textureCoord(textCoord[i]);
	}

	//Object complete
	displaySurface->end();

	//create a node child to the camera here :
	consoleNode = AnnGetEngine()->getSceneManager()->getRootSceneNode()->createChildSceneNode();

	//attach The object
	consoleNode->attachObject(displaySurface);

	//set the player relative position
	consoleNode->setPosition(offset + AnnGetPlayer()->getEyesHeight() * AnnVect3::UNIT_Y);

	//Make sure the object is the last thing rendered (to be on top of everything
	//displaySurface->setRenderQueueGroup(Ogre::uint8(-1));
	//Set the visibility state
	consoleNode->setVisible(visibility);

	//Create a The font
	if (!Ogre::FontManager::getSingletonPtr()) //The FontManager isn't initialized by default
	{
		//Create a FontManager
		std::cerr << "FontManager not usable yet. Initializing a new FontManager" << std::endl;
		// ReSharper disable CppNonReclaimedResourceAcquisition
		// Reason : Ogre will cleanup the FontManager when cleaning root.
		new Ogre::FontManager();
		// ReSharper restore CppNonReclaimedResourceAcquisition
	}

	//Create a manual font
	font = Ogre::FontManager::getSingleton().create("VeraMono", "ANNWVYN_CORE");

	//Load the VeraMono.ttf file
	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource("VeraMono.ttf");
	font->setTrueTypeResolution(96);
	font->setTrueTypeSize(BASE / 32); //Size of font is relative to the size of the pixel-buffer (texture)

	//Aspect ration of the console is 2:1. The actual size of texture is 2*BASE x BASE
	//Create an map the texture to the displaySurface
	texture = Ogre::TextureManager::getSingleton().createManual("Write Texture", "ANNWVYN_CORE", Ogre::TEX_TYPE_2D, 2 * BASE, BASE, Ogre::MIP_UNLIMITED, Ogre::PF_X8R8G8B8, Ogre::TU_AUTOMIPMAP | Ogre::TU_RENDERTARGET);
	Ogre::TextureUnitState* displaySurfaceTextureUniteState = pass->createTextureUnitState();
	displaySurfaceTextureUniteState->setTexture(texture);

	//Load background texture to a buffer
	background = Ogre::TextureManager::getSingleton().load("background.png", "ANNWVYN_CORE");

	//Initialize the text buffer.
	//CONSOLE_BUFFER is the number of lines to keep in memory and to load on the texture.
	//Text is drawn from the 1st line. The number of line define how main lines are visible on the console
	for (size_t i(0); i < CONSOLE_BUFFER; i++)
		buffer[i] = "";

	//To optimize texture copy, make sure to use the most efficient OpenGL method

	if (Ogre::Root::getSingleton().getRenderSystem()->getName()
		== "OpenGL Rendering Subsystem")
	{
		background->getCustomAttribute("GLID", &backgroundID);
		texture->getCustomAttribute("GLID", &textureID);

		// check if OpenGL version is > at 4.3
		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		std::stringstream log;
		log << "AnnConsolen constructor detected OpenGL version " << major << "." << minor;
		AnnEngine::log(log.str());

		if ((openGL43plus = major >= 4 && minor >= 3)) { AnnEngine::log("This version is 4.3 or greater. Texture copy optimization enabled"); }
	}
}

void AnnConsole::append(std::string str)
{
	//Copy with an offset of 1 the buffer content
	for (size_t i(1); i < CONSOLE_BUFFER; i++)
		buffer[i - 1] = buffer[i];

	//Write the texture to the buffer
	buffer[CONSOLE_BUFFER - 1] = str;

	//The console will be redrawn next frame
	modified = true;
}

void AnnConsole::setVisible(bool state)
{
	visibility = state;
	AnnGetEventManager()->keyboardUsedForText(state);
	if (state)
		AnnGetEventManager()->getTextInputer()->startListening();
	else
		AnnGetEventManager()->getTextInputer()->stopListening();

	consoleNode->setVisible(visibility);
}

void AnnConsole::toggle()
{
	setVisible(!visibility);
}

void AnnConsole::update()
{
	//Updated
	modified = false;
	lastUpdate = AnnGetEngine()->getTimeFromStartupSeconds();

	//Get the content of the buffer into a static string
	std::stringstream content; std::string logLine;
	for (auto i{ 0 }; i < CONSOLE_BUFFER; i++)
	{
		logLine = buffer[i].substr(0, MAX_CONSOLE_LOG_WIDTH);

		for (auto j{ 0 }; j < logLine.size(); j++)
			if (logLine[j] == '\n') logLine[j] = '|';

		content << logLine << '\n';
	}

	for (auto i{ 0 }; i < MAX_CONSOLE_LOG_WIDTH; ++i) content << "-";
	content << "\n%> ";

	auto command = AnnGetEventManager()->getTextInputer()->getInput();
	content << command.substr(std::max(0, int(command.size()) - (MAX_CONSOLE_LOG_WIDTH - 5)), command.size());

	//If carriage return character
	if (command[command.size() - 1] == '\r')
	{
		runInput(command);
		//Execute command code here
		AnnGetEventManager()->getTextInputer()->clearInput();
	}

	if (static_cast<int>(4 * AnnGetEngine()->getTimeFromStartupSeconds()) % 2) content << "_";

	Ogre::String textToDisplay = content.str();

	if (openGL43plus)
		glCopyImageSubData(backgroundID, GL_TEXTURE_2D, 0, 0, 0, 0,
						   textureID, GL_TEXTURE_2D, 0, 0, 0, 0,
						   texture->getSrcWidth(), texture->getSrcHeight(), 1);
	else
	{
		//background->copyToTexture(texture);
		float w(texture->getBuffer()->getWidth());
		float h(texture->getBuffer()->getHeight());

		auto texture_out = texture->getBuffer()->lock(Ogre::Image::Box(0, 0, w, h), Ogre::HardwareBuffer::LockOptions::HBL_WRITE_ONLY);

		w = background->getBuffer()->getWidth();
		h = background->getBuffer()->getHeight();

		auto background_in = background->getBuffer()->lock(Ogre::Image::Box(0, 0, w, h), Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY);

		for (auto y(0); y < h; ++y) for (auto x(0); x < w; ++x)
			texture_out.setColourAt(background_in.getColourAt(x, y, 0), x, y, 0);

		background->getBuffer()->unlock();
		texture->getBuffer()->unlock();
	}

	//Write text to texture
	WriteToTexture(textToDisplay,																	//Text
				   texture,																			//Texture
				   Ogre::Image::Box(0 + MARGIN, 0 + MARGIN, 2 * BASE - MARGIN, BASE - MARGIN),		//Part of the pixel buffer to write to
				   Ogre::ColourValue::Black,														//Color
				   'l', true);																		//Alignment
}

void AnnConsole::WriteToTexture(const Ogre::String &str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, const Ogre::ColourValue &color, char justify, bool wordwrap)
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

	// The font texture textureBuffer was created write only...so we cannot read it back :o). One solution is to copy the textureBuffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a textureBuffer
	size_t nBuffSize = fontBuffer->getSizeInBytes();
	uint8* textureBuffer = static_cast<uint8*>(calloc(nBuffSize, sizeof(uint8)));

	// create pixel box using the copy of the textureBuffer
	PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(), fontBuffer->getDepth(), fontBuffer->getFormat(), textureBuffer);
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
							case ' ': wordwidth = charwidth; ++l; break;
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

				//abort - not enough space to draw
				if ((cursorY + charheight) > destRectangle.getHeight())
					goto stop;

				//draw pixel by pixel
				for (size_t i = 0; i < GlyphTexCoords[strindex].getHeight(); i++)
					for (size_t j = 0; j < GlyphTexCoords[strindex].getWidth(); j++)
					{
						float alpha = color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize + 1] / 255.0);
						float invalpha = 1.0 - alpha;
						size_t charOffset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;
						ColourValue pix;
						PixelUtil::unpackColour(&pix, destPb.format, &destData[charOffset]);
						pix = (pix * invalpha) + (color * alpha);
						PixelUtil::packColour(pix, destPb.format, &destData[charOffset]);
					}

				cursorX += GlyphTexCoords[strindex].getWidth();
			}//default
		}//switch
	}//for

stop:
	delete[] GlyphTexCoords;

	destBuffer->unlock();

	// Free the memory allocated for the textureBuffer
	free(textureBuffer);
}

void AnnConsole::syncConsolePosition()
{
	auto targetPosition = AnnGetVRRenderer()->trackedHeadPose.position + AnnGetVRRenderer()->trackedHeadPose.orientation*offset;
	auto targetOrientaiton = AnnGetVRRenderer()->trackedHeadPose.orientation;

	consoleNode->setPosition(targetPosition);
	consoleNode->setOrientation(targetOrientaiton);
}

bool AnnConsole::needUpdate()
{
	syncConsolePosition();

	if (AnnGetEngine()->getTimeFromStartupSeconds() - lastUpdate > refreshRate)
		modified = true;

	return modified && visibility;
}

void AnnConsole::runInput(std::string& input)
{
	//do some cleanup on the inputed string
	//remove the \r terminaison
	input.pop_back();

	try
	{
		AnnGetScriptManager()->evalString(input);
	}
	catch (const chaiscript::exception::eval_error& eval_error)
	{
		AnnDebug() << "Console script error : " << input;
		AnnDebug() << eval_error.what();
		AnnDebug() << eval_error.pretty_print();
	}
}