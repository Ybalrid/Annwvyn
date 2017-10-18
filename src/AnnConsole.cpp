#include "stdafx.h"
#include "AnnConsole.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"
#include <OgreHardwarePixelBuffer.h>

using namespace Annwvyn;

AnnConsole::AnnConsole() : AnnSubSystem("OnScreenConsole"),
modified(false),
consoleNode(nullptr),
offset(0, 0.125f, -0.75f),
openGL43plus(false),
visibility(false),
lastUpdate{ 0 },
refreshRate{ 1.0 / 15.0 }
{
	/*
	* The displaySurface is a perfect rectangle drawn by 2 polygons (triangles). The position in object-space are defined as following
	* on the "points" array :
	*  0 +---------------+ 2
	*    |           /   |
	*    |        /      |
	*    |     /         |
	*    |  /            |
	*  1 +---------------+ 3
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
	displaySurface = AnnGetEngine()->getSceneManager()->createManualObject();
	displaySurface->begin("Console", Ogre::OT_TRIANGLE_STRIP);//Strip of triangle : Define a triangle then add them by points

	//Add the four vertices. This will directly describe two Triangles
	for (char i(0); i < 4; i++)
	{
		displaySurface->position(points[i]);
		displaySurface->normal(0, 0, 1);
		displaySurface->tangent(1, 0, 0);
		displaySurface->textureCoord(textCoord[i]);
		displaySurface->index(i);
	}

	//Object complete
	displaySurface->end();

	displaySurface->setCastShadows(false);

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
		OGRE_NEW Ogre::FontManager();
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
	texture = Ogre::TextureManager::getSingleton().createManual("Write Texture", "ANNWVYN_CORE",
		Ogre::TEX_TYPE_2D, 2 * BASE, BASE,
		Ogre::MIP_UNLIMITED, Ogre::PF_X8R8G8B8,
		Ogre::TU_AUTOMIPMAP | Ogre::TU_RENDERTARGET);

	auto datablock = AnnGetVRRenderer()->getRoot()->getHlmsManager()->getDatablock("Console");
	if (auto unlitDatablock = reinterpret_cast<Ogre::HlmsUnlitDatablock*>(datablock))
	{
		AnnDebug() << "got unlit datablock for " << "Console";
		unlitDatablock->setTexture(Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE, 0, texture);
	}

	//Load background texture to a buffer
	background = Ogre::TextureManager::getSingleton().load("background.png", "ANNWVYN_CORE");

	//Initialize the text buffer.
	//CONSOLE_BUFFER is the number of lines to keep in memory and to load on the texture.
	//Text is drawn from the 1st line. The number of line define how main lines are visible on the console
	for (size_t i(0); i < CONSOLE_BUFFER; i++)
		buffer[i] = "";

	//To optimize texture copy, make sure to use the most efficient OpenGL method

	if (Ogre::Root::getSingleton().getRenderSystem()->getName()
		== "OpenGL 3+ Rendering Subsystem")
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
	AnnGetEventManager()->keyboardUsedForText(visibility);
	if (visibility)
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
	std::stringstream content;

	//For each line
	for (auto i{ 0 }; i < CONSOLE_BUFFER; i++)
	{
		//Make the len fit the screen
		auto logLine = buffer[i].substr(0, MAX_CONSOLE_LOG_WIDTH);

		//No newline char
		for (auto j{ 0 }; j < logLine.size(); j++)
			if (logLine[j] == '\n') logLine[j] = '|';

		//Append to display content
		content << logLine << '\n';
	}

	//horizontal separator
	for (auto i{ 0 }; i < MAX_CONSOLE_LOG_WIDTH; ++i) content << "-";

	//Command Invite
	content << "\n%> ";
	auto command = AnnGetEventManager()->getTextInputer()->getInput();

	//Display with a scrolling window
	if (!command.empty())
	{
		content << command.substr(std::max(0, int(command.size()) - (MAX_CONSOLE_LOG_WIDTH - 5)), command.size());
		if (command[command.size() - 1] == '\r')
		{
			//Execute command code here
			runInput(command);
			AnnGetEventManager()->getTextInputer()->clearInput();
		}
	}

	//Append blinking cursor
	if (static_cast<int>(4 * AnnGetEngine()->getTimeFromStartupSeconds()) % 2) content << "_";
	auto textToDisplay = content.str();

	//Erase plane (draw background)
	if (openGL43plus)
	{
		glCopyImageSubData(backgroundID, GL_TEXTURE_2D, 0, 0, 0, 0,
			textureID, GL_TEXTURE_2D, 0, 0, 0, 0,
			texture->getSrcWidth(), texture->getSrcHeight(), 1);
	}
	else
	{
		auto w(texture->getBuffer()->getWidth());
		auto h(texture->getBuffer()->getHeight());

		auto texture_out = texture->getBuffer()->lock(Ogre::Image::Box(0, 0, w, h), Ogre::v1::HardwareBuffer::LockOptions::HBL_WRITE_ONLY);

		w = background->getBuffer()->getWidth();
		h = background->getBuffer()->getHeight();

		auto background_in = background->getBuffer()->lock(Ogre::Image::Box(0, 0, w, h), Ogre::v1::HardwareBuffer::LockOptions::HBL_READ_ONLY);

		for (auto y(0u); y < h; ++y) for (auto x(0u); x < w; ++x)
			texture_out.setColourAt(background_in.getColourAt(x, y, 0), x, y, 0);

		background->getBuffer()->unlock();
		texture->getBuffer()->unlock();
	}

	//Write text to texture
	WriteToTexture(textToDisplay,																	//Text
		texture,																					//Texture
		Ogre::Image::Box(0 + MARGIN, 0 + MARGIN, 2 * BASE - MARGIN, BASE - MARGIN),					//Part of the pixel buffer to write to
		font.get(),
		Ogre::ColourValue::Black,																	//Color
		'l', true);																					//Alignment
}

bool AnnConsole::isForbdiden(const std::string& keyword)
{
	for (const auto& forbiddenKeyword : forbidden)
		if (keyword == forbiddenKeyword)
			return true;
	return false;
}

void AnnConsole::WriteToTexture(const Ogre::String &str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify, bool wordwrap)
{
	using namespace Ogre;

	if (destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if (destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if (!font->isLoaded())
		font->load();

	auto fontTexture = TexturePtr(TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName()));

	auto fontBuffer = fontTexture->getBuffer();
	auto destBuffer = destTexture->getBuffer();

	auto destPb = destBuffer->lock(destRectangle, v1::HardwareBuffer::HBL_NORMAL);

	// The font texture textureBuffer was created write only...so we cannot read it back :o). One solution is to copy the textureBuffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a textureBuffer
	auto nBuffSize = fontBuffer->getSizeInBytes();
	auto textureBuffer = static_cast<uint8*>(calloc(nBuffSize, sizeof(uint8)));

	// create pixel box using the copy of the textureBuffer
	PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(), fontBuffer->getDepth(), fontBuffer->getFormat(), textureBuffer);
	fontBuffer->blitToMemory(fontPb);

	auto fontData = static_cast<uint8*>(fontPb.data);
	auto destData = static_cast<uint8*>(destPb.data);

	const auto fontPixelSize = PixelUtil::getNumElemBytes(fontPb.format);
	const auto destPixelSize = PixelUtil::getNumElemBytes(destPb.format);

	const auto fontRowPitchBytes = fontPb.rowPitch * fontPixelSize;
	const auto destRowPitchBytes = destPb.rowPitch * destPixelSize;

	std::vector<Box> GlyphTexCoords(str.size());

	size_t charheight = 0;
	size_t charwidth = 0;

	for (unsigned int i = 0; i < str.size(); i++)
	{
		if ((str[i] != '\t') && (str[i] != '\n') && (str[i] != ' '))
		{
			auto glypheTexRect = font->getGlyphTexCoords(str[i]);
			GlyphTexCoords[i].left = uint32_t(glypheTexRect.left * fontTexture->getSrcWidth());
			GlyphTexCoords[i].top = uint32_t(glypheTexRect.top * fontTexture->getSrcHeight());
			GlyphTexCoords[i].right = uint32_t(glypheTexRect.right * fontTexture->getSrcWidth());
			GlyphTexCoords[i].bottom = uint32_t(glypheTexRect.bottom * fontTexture->getSrcHeight());

			if (GlyphTexCoords[i].getHeight() > charheight)
				charheight = GlyphTexCoords[i].getHeight();
			if (GlyphTexCoords[i].getWidth() > charwidth)
				charwidth = GlyphTexCoords[i].getWidth();
		}
	}

	//get the size of the glyph '0'
	auto glypheTexRect = font->getGlyphTexCoords('0');
	Box spaceBox;
	spaceBox.left = uint32_t(glypheTexRect.left * fontTexture->getSrcWidth());
	spaceBox.right = uint32_t(glypheTexRect.right * fontTexture->getSrcWidth());
	auto spacewidth = spaceBox.getWidth();

	//if not mono-spaced
	if (spacewidth != charwidth) spacewidth = size_t(float(spacewidth) * 0.5f);

	size_t cursorX = 0;
	size_t cursorY = 0;
	auto lineend = destRectangle.getWidth();
	auto carriagreturn = true;
	for (size_t strindex = 0; strindex < str.size(); strindex++)
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
				auto l = strindex;
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
					lineend = destRectangle.getWidth() - uint32(cursorX);
					break;

				case 'r':    cursorX = (destRectangle.getWidth() - textwidth);
					lineend = destRectangle.getWidth();
					break;

				default:    cursorX = 0;
					lineend = uint32(textwidth);
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
					auto alpha = float(color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize + 1] / 255.0));
					auto invalpha = 1.0f - alpha;
					auto charOffset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;
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

	destBuffer->unlock();

	// Free the memory allocated for the textureBuffer
	free(textureBuffer);
}

void AnnConsole::notifyNavigationKey(KeyCode::code code)
{
}

void AnnConsole::syncConsolePosition() const
{
	auto targetPosition = AnnGetVRRenderer()->trackedHeadPose.position + AnnGetVRRenderer()->trackedHeadPose.orientation * offset;
	auto targetOrientaiton = AnnGetVRRenderer()->trackedHeadPose.orientation;

	consoleNode->setPosition(targetPosition);
	consoleNode->setOrientation(static_cast<Ogre::Quaternion>(targetOrientaiton));
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
	//remove the \r termination
	input.pop_back();

	//Prevent to start with some chaiscript symbols in global space.
	std::string firstWord;
	std::stringstream inputStream(input);
	inputStream >> firstWord;

	if (isForbdiden(firstWord))
	{
		AnnDebug() << "Console input error : " << firstWord << " is a forbidden keyword";
		return;
	}

	if (runSpecialInput(input)) return;

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

bool AnnConsole::runSpecialInput(const std::string& input)
{
	if (input == "help")
	{
		bufferClear();
		append("You asked for help :");
		append("This debug console understand the same thing as the integrated");
		append("scripting language.");
		append("However, it runs on global space. To prevent breaking stuff");
		append("you can't create global variables from that console. You have to");
		append("reference GameObject by their name for example");
		append("You can display this help by typing \"help\"");

		return true;
	}

	if (input == "status")
	{
		bufferClear();
		append("Running VR system: " + AnnGetVRRenderer()->getName());
		append("LevelManager : " + std::to_string(AnnGetLevelManager()->getCurrentLevel()->getContent().size()) + " active objects");
		append("LevelManager : " + std::to_string(AnnGetLevelManager()->getCurrentLevel()->getLights().size()) + " active light sources");
		size_t nbControllers;
		append("HandController : " + std::to_string(nbControllers = AnnGetVRRenderer()->getHanControllerArraySize()) + " current controllers");
		if (nbControllers > 0) if (AnnGetVRRenderer()->getHandControllerArray()[0])
		{
			append("HandControllers connected");
			append("HandController types : " + AnnGetVRRenderer()->getHandControllerArray()[0]->getTypeString());
		}
		else
		{
			append("HandControllers are not connected");
		}

		return true;
	}

	return false;
}

void AnnConsole::bufferClear()
{
	for (auto& line : buffer)
		line.clear();
}

AnnConsole::~AnnConsole()
{
	font.setNull();
}