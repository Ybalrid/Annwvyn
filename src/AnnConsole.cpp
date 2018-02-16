// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnConsole.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"

#include <Ogre.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <OgreRenderOperation.h>
#include <Hlms/Unlit/OgreHlmsUnlitDatablock.h>
#include <OgreHardwarePixelBuffer.h>

using namespace Annwvyn;

AnnConsole::AnnConsole() :
 AnnSubSystem("OnScreenConsole"),
 modified(false),
 consoleNode(nullptr),
 offset(0, 0.125f, -0.75f),
 visibility(false),
 lastUpdate{ 0 },
 refreshRate{ 1.0 / 15.0 },
 historyStatus{ -1 },
 cursorPos{ 0 }
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
	displaySurface->begin("Console", Ogre::OT_TRIANGLE_STRIP); //Strip of triangle : Define a triangle then add them by points

	//Add the four vertices. This will directly describe two Triangles
	for(char i(0); i < 4; i++)
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
	if(!Ogre::FontManager::getSingletonPtr()) //The FontManager isn't initialized by default
	{
		//Create a FontManager
		std::cerr << "FontManager not usable yet. Initializing a new FontManager" << std::endl;
		// ReSharper disable CppNonReclaimedResourceAcquisition
		// Reason : Ogre will cleanup the FontManager when cleaning root.
		OGRE_NEW Ogre::FontManager();
		// ReSharper restore CppNonReclaimedResourceAcquisition
	}

	//Create a manual font
	font = Ogre::FontManager::getSingleton().create("VeraMono", AnnResourceManager::getDefaultResourceGroupName());

	//Load the VeraMono.ttf file
	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource("VeraMono.ttf");
	font->setTrueTypeResolution(96);
	font->setTrueTypeSize(BASE / 32); //Size of font is relative to the size of the pixel-buffer (texture)

	//Aspect ration of the console is 2:1. The actual size of texture is 2*BASE x BASE
	//Create an map the texture to the displaySurface
	texture = Ogre::TextureManager::getSingleton().createManual("Write Texture", AnnResourceManager::getDefaultResourceGroupName(), Ogre::TEX_TYPE_2D, 2 * BASE, BASE, Ogre::MIP_UNLIMITED, Ogre::PF_X8R8G8B8, Ogre::TU_AUTOMIPMAP | Ogre::TU_RENDERTARGET);

	auto datablock = AnnGetVRRenderer()->getRoot()->getHlmsManager()->getDatablock("Console");
	if(auto unlitDatablock = reinterpret_cast<Ogre::HlmsUnlitDatablock*>(datablock))
	{
		AnnDebug() << "got unlit datablock for "
				   << "Console";
		unlitDatablock->setTexture(Ogre::HlmsTextureManager::TEXTURE_TYPE_DIFFUSE, 0, texture);
	}

	//Load background texture to a buffer
	background = Ogre::TextureManager::getSingleton().load("background.png", AnnResourceManager::getDefaultResourceGroupName());

	//Initialize the text buffer.
	//CONSOLE_BUFFER is the number of lines to keep in memory and to load on the texture.
	//Text is drawn from the 1st line. The number of line define how main lines are visible on the console
	for(size_t i(0); i < CONSOLE_BUFFER; i++)
		buffer[i] = "";

	//Get the opengl ids
	if(Ogre::Root::getSingleton().getRenderSystem()->getName()
	   == "OpenGL 3+ Rendering Subsystem")
	{
		background->getCustomAttribute("GLID", &backgroundID);
		texture->getCustomAttribute("GLID", &textureID);
	}
}

void AnnConsole::append(const std::string& str)
{
	rotate(begin(buffer), begin(buffer) + 1, end(buffer));
	buffer[CONSOLE_BUFFER - 1] = str;

	//The console will be redrawn next frame
	modified = true;
}

void AnnConsole::setVisible(bool state)
{
	visibility = state;
	AnnGetEventManager()->keyboardUsedForText(visibility);
	if(visibility)
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
	modified   = false;
	lastUpdate = AnnGetEngine()->getTimeFromStartupSeconds();

	//Get the content of the buffer into a static string
	std::stringstream content;

	//For each line
	for(auto i{ 0 }; i < CONSOLE_BUFFER; i++)
	{
		//Make the len fit the screen
		auto logLine = buffer[i].substr(0, MAX_CONSOLE_LOG_WIDTH);

		//No newline char
		for(auto j{ 0 }; j < logLine.size(); j++)
			if(logLine[j] == '\n') logLine[j] = '|';

		//Append to display content
		content << logLine << '\n';
	}

	//horizontal separator
	for(auto i{ 0 }; i < MAX_CONSOLE_LOG_WIDTH; ++i) content << "-";

	//Command Invite
	content << "\n%> ";
	auto textInputer = AnnGetEventManager()->getTextInputer();
	auto command	 = textInputer->getInput();
	cursorPos		 = textInputer->getCursorOffset();
	if(command.empty())
	{
		cursorPos = 0;
		textInputer->setCursorOffset(cursorPos);
	}

	std::string strippedCommand;
	//Display with a scrolling window
	if(!command.empty())
	{
		strippedCommand = command.substr(std::max(0, int(command.size()) - (MAX_CONSOLE_LOG_WIDTH - 5)), command.size());
		content << strippedCommand;
		if(command[command.size() - 1] == '\r')
		{
			//Execute command code here
			runInput(command);
			AnnGetEventManager()->getTextInputer()->clearInput();
			cursorPos = 0;
		}
	}

	//Append blinking cursor
	//if (static_cast<int>(4 * AnnGetEngine()->getTimeFromStartupSeconds()) % 2) content << "_";
	const auto textToDisplay = content.str();

	//Erase plane (draw background)
	glCopyImageSubData(backgroundID, GL_TEXTURE_2D, 0, 0, 0, 0, textureID, GL_TEXTURE_2D, 0, 0, 0, 0, texture->getSrcWidth(), texture->getSrcHeight(), 1);

	//Write text to texture
	WriteToTexture(textToDisplay,															   //Text
				   texture,																	   //Texture
				   Ogre::Image::Box(0 + MARGIN, 0 + MARGIN, 2 * BASE - MARGIN, BASE - MARGIN), //Part of the pixel buffer to write to
				   font.get(),
				   Ogre::ColourValue::Black, //Color
				   'l',
				   true); //Alignment

	std::string cursor;
	for(auto i = 0; i < CONSOLE_BUFFER + 1; ++i) cursor += '\n';
	cursor += ".  ";
	for(auto i = 0; i < std::max(0, int(strippedCommand.size()) - cursorPos); ++i) cursor += ' ';
	cursor += '_';

	WriteToTexture(cursor,
				   texture,
				   Ogre::Image::Box(0 + MARGIN, 0 + MARGIN, 2 * BASE - MARGIN, BASE - MARGIN), //Part of the pixel buffer to write to
				   font.get(),
				   Ogre::ColourValue::Blue,
				   'l',
				   true);
}

bool AnnConsole::isForbdiden(const std::string& keyword)
{
	for(const auto& forbiddenKeyword : forbidden)
		if(keyword == forbiddenKeyword)
			return true;
	return false;
}

void AnnConsole::WriteToTexture(const Ogre::String& str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue& color, char justify, bool wordwrap)
{
	using namespace Ogre;

	if(destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if(destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if(!font->isLoaded())
		font->load();

	auto fontTexture = TexturePtr(TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName()));

	auto fontBuffer = fontTexture->getBuffer();
	auto destBuffer = destTexture->getBuffer();

	const auto destPb = destBuffer->lock(destRectangle, v1::HardwareBuffer::HBL_NORMAL);

	// The font texture textureBuffer was created write only...so we cannot read it back :o). One solution is to copy the textureBuffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)

	// create a textureBuffer
	const auto nBuffSize	 = fontBuffer->getSizeInBytes();
	const auto textureBuffer = static_cast<uint8*>(calloc(nBuffSize, sizeof(uint8)));

	// create pixel box using the copy of the textureBuffer
	const PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(), fontBuffer->getDepth(), fontBuffer->getFormat(), textureBuffer);
	fontBuffer->blitToMemory(fontPb);

	const auto fontData = static_cast<uint8*>(fontPb.data);
	const auto destData = static_cast<uint8*>(destPb.data);

	const auto fontPixelSize = PixelUtil::getNumElemBytes(fontPb.format);
	const auto destPixelSize = PixelUtil::getNumElemBytes(destPb.format);

	const auto fontRowPitchBytes = fontPb.rowPitch * fontPixelSize;
	const auto destRowPitchBytes = destPb.rowPitch * destPixelSize;

	std::vector<Box> GlyphTexCoords(str.size());

	size_t charheight = 0;
	size_t charwidth  = 0;

	for(unsigned int i = 0; i < str.size(); i++)
	{
		if((str[i] != '\t') && (str[i] != '\n') && (str[i] != ' '))
		{
			auto glypheTexRect		 = font->getGlyphTexCoords(str[i]);
			GlyphTexCoords[i].left   = uint32_t(glypheTexRect.left * fontTexture->getSrcWidth());
			GlyphTexCoords[i].top	= uint32_t(glypheTexRect.top * fontTexture->getSrcHeight());
			GlyphTexCoords[i].right  = uint32_t(glypheTexRect.right * fontTexture->getSrcWidth());
			GlyphTexCoords[i].bottom = uint32_t(glypheTexRect.bottom * fontTexture->getSrcHeight());

			if(GlyphTexCoords[i].getHeight() > charheight)
				charheight = GlyphTexCoords[i].getHeight();
			if(GlyphTexCoords[i].getWidth() > charwidth)
				charwidth = GlyphTexCoords[i].getWidth();
		}
	}

	//get the size of the glyph '0'
	const auto glypheTexRect = font->getGlyphTexCoords('0');
	Box spaceBox;
	spaceBox.left   = uint32_t(glypheTexRect.left * fontTexture->getSrcWidth());
	spaceBox.right  = uint32_t(glypheTexRect.right * fontTexture->getSrcWidth());
	auto spacewidth = spaceBox.getWidth();

	//if not mono-spaced
	if(spacewidth != charwidth) spacewidth = size_t(float(spacewidth) * 0.5f);

	size_t cursorX	 = 0;
	size_t cursorY	 = 0;
	auto lineend	   = destRectangle.getWidth();
	auto carriagreturn = true;
	for(size_t strindex = 0; strindex < str.size(); strindex++)
	{
		switch(str[strindex])
		{
			case ' ': cursorX += spacewidth; break;
			case '\t': cursorX += charwidth * 3; break;
			case '\n':
				cursorY += charheight;
				carriagreturn = true;
				break;
			default:
			{
				//wrapping
				if((cursorX + GlyphTexCoords[strindex].getWidth() > lineend) && !carriagreturn)
				{
					cursorY += charheight;
					carriagreturn = true;
				}

				//justify
				if(carriagreturn)
				{
					auto l			 = strindex;
					size_t textwidth = 0;
					size_t wordwidth = 0;

					while((l < str.size()) && (str[l] != '\n'))
					{
						wordwidth = 0;

						switch(str[l])
						{
							case ' ':
								wordwidth = charwidth;
								++l;
								break;
							case '\t':
								wordwidth = charwidth * 3;
								++l;
								break;
							case '\n': l = str.size();
							default: break;
						}

						if(wordwrap)
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

						if((textwidth + wordwidth) <= destRectangle.getWidth())
							textwidth += (wordwidth);
						else
							break;
					}

					if((textwidth == 0) && (wordwidth > destRectangle.getWidth()))
						textwidth = destRectangle.getWidth();

					switch(justify)
					{
						case 'c':
							cursorX = (destRectangle.getWidth() - textwidth) / 2;
							lineend = destRectangle.getWidth() - uint32(cursorX);
							break;

						case 'r':
							cursorX = (destRectangle.getWidth() - textwidth);
							lineend = destRectangle.getWidth();
							break;

						default:
							cursorX = 0;
							lineend = uint32(textwidth);
							break;
					}

					carriagreturn = false;
				}

				//abort - not enough space to draw
				if((cursorY + charheight) > destRectangle.getHeight())
					goto stop;

				//draw pixel by pixel
				for(size_t i = 0; i < GlyphTexCoords[strindex].getHeight(); i++)
					for(size_t j = 0; j < GlyphTexCoords[strindex].getWidth(); j++)
					{
						const auto alpha		= float(color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize + 1] / 255.0));
						const auto invalpha   = 1.0f - alpha;
						const auto charOffset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;
						ColourValue pix;
						PixelUtil::unpackColour(&pix, destPb.format, &destData[charOffset]);
						pix = (pix * invalpha) + (color * alpha);
						PixelUtil::packColour(pix, destPb.format, &destData[charOffset]);
					}

				cursorX += GlyphTexCoords[strindex].getWidth();
			} //default
		}	  //switch
	}		  //for

stop:

	destBuffer->unlock();

	// Free the memory allocated for the textureBuffer
	free(textureBuffer);
}

bool AnnConsole::setFromPointedHistory()
{
	const auto& command = commandHistory[historyStatus];
	if(!command.empty())
	{
		AnnGetEventManager()->getTextInputer()->setInput(command);
		return false;
	}
	return true;
}

void AnnConsole::notifyNavigationKey(KeyCode::code code)
{
	if(!visibility) return;

	switch(code)
	{
		default: break;
		case KeyCode::up:
			//AnnDebug() << "history command!";
			historyStatus++;
			if(historyStatus >= CONSOLE_HISTORY)
				historyStatus = CONSOLE_HISTORY - 1;
			if(setFromPointedHistory())
				historyStatus--;
			break;
		case KeyCode::down:
			historyStatus--;
			if(historyStatus <= -1)
			{
				historyStatus = -1;
				break;
			}
			setFromPointedHistory();
			break;
		case KeyCode::right:
			cursorPos--;
			if(cursorPos < 0) cursorPos = 0;
			AnnGetEventManager()->getTextInputer()->setCursorOffset(cursorPos);
			break;
		case KeyCode::left:
			cursorPos++;
			AnnGetEventManager()->getTextInputer()->setCursorOffset(cursorPos);
	}
}

void AnnConsole::syncConsolePosition() const
{
	const auto targetPosition	= AnnGetVRRenderer()->trackedHeadPose.position + AnnGetVRRenderer()->trackedHeadPose.orientation * offset;
	const auto targetOrientaiton = AnnGetVRRenderer()->trackedHeadPose.orientation;

	consoleNode->setPosition(targetPosition);
	consoleNode->setOrientation(static_cast<Ogre::Quaternion>(targetOrientaiton));
}

bool AnnConsole::needUpdate()
{
	syncConsolePosition();

	if(AnnGetEngine()->getTimeFromStartupSeconds() - lastUpdate > refreshRate)
		modified = true;

	return modified && visibility;
}

void AnnConsole::runInput(std::string& input)
{
	//do some cleanup on the inputed string
	//remove the \r termination
	input.pop_back();

	addToHistory(input);
	historyStatus = -1;

	//Echo the command to the console
	AnnDebug() << "% - " << input;

	//Prevent to start with some chaiscript symbols in global space.
	std::string firstWord;
	std::stringstream inputStream(input);
	inputStream >> firstWord;

	if(isForbdiden(firstWord))
	{
		AnnDebug() << "Console input error : " << firstWord << " is a forbidden keyword";
		return;
	}

	if(runSpecialInput(input)) return;

	try
	{
		AnnGetScriptManager()->evalString(input);
	}
	catch(const chaiscript::exception::eval_error& eval_error)
	{
		AnnDebug() << "Console script error : " << input;
		AnnDebug() << eval_error.what();
		AnnDebug() << eval_error.pretty_print();
	}
}

void AnnConsole::addToHistory(const std::string& input)
{
	if(input.empty()) return;
	rotate(rbegin(commandHistory), rbegin(commandHistory) + 1, rend(commandHistory));
	commandHistory[0] = input;
}

bool AnnConsole::runSpecialInput(const std::string& input)
{
	if(input == "help")
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

	else if(input == "status")
	{
		bufferClear();
		auto currentLevel = AnnGetLevelManager()->getCurrentLevel();
		size_t nbControllers;
		
		append("Running VR system: " + AnnGetVRRenderer()->getName());
		append("LevelManager: " + std::to_string(currentLevel->getContent().size()) + " active objects");
		append("LevelManager: " + std::to_string(currentLevel->getLights().size()) + " active light sources");
		append("LevelManager: " + std::to_string(currentLevel->getTriggers().size()) + " physics trigger object");
		append("HandController : " + std::to_string(nbControllers = AnnGetVRRenderer()->getHanControllerArraySize()) + " max tracked controllers");
		
		if(nbControllers > 0)
		{
			if(AnnGetVRRenderer()->getHandControllerArray()[0])
			{
				append("HandControllers connected");
				append("HandController types : " + AnnGetVRRenderer()->getHandControllerArray()[0]->getTypeString());
			}
			else
			{
				append("No HandControllers active");
			}
		}
		return true;
	}

	return false;
}

void AnnConsole::bufferClear()
{
	for(auto& line : buffer)
		line.clear();
}

AnnConsole::~AnnConsole()
{
	font.setNull();
}
