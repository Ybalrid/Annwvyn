/**
 * \file AnnConsole.hpp
 * \brief Represent the output console that can be shown by calling AnnEngine::toggleOnScreenConsole()
 *		  This class create handle the texture buffer and text rendering for text display
 * \author A. Brainville (Ybalrid)
 */

#ifndef	ANNCONSOLE
#define ANNCONSOLE

#include "systemMacro.h"
#include "AnnTypes.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <Ogre.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>

#include "AnnSubsystem.hpp"

#define CONSOLE_BUFFER 17
#define MAX_CONSOLE_LOG_WIDTH 72
#define BASE 256
#define MARGIN 4

namespace Annwvyn
{
	///In engine - On screen floating console
	class DLL AnnConsole : public AnnSubSystem
	{
	public:
		///Construct the console. This should only be called by AnnEngine itself when the camera and ogre are operational
		AnnConsole();

		///Add text to the console buffer. The console buffer will keep CONSOLE_BUFFER lines of messages in memory only
		/// \param string text to append to the console
		void append(std::string string);

		///Set arbitrary the visibility state of the console
		void setVisible(bool visibility);

		///Toggle the console.
		void toggle();

		///True if text has been updated on the console and the console is visible.
		bool needUpdate() override;

		///Update the console by filling it with background texture then blitting text on it.
		///Can take some computing time depending on the size/resolution of the textures and buffer
		void update() override;

		///Move the console where it should
		void syncConsolePosition();

		void bufferClear();

	private:

		///Cleanup and run the user input.
		void runInput(std::string& input);

		bool runSpecialInput(const std::string& input);

		///This piece of code if from the Ogre Wiki. Write text to a texture using Ogre::FontManager to create glyphs
		void WriteToTexture(const Ogre::String& str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, const Ogre::ColourValue &color, char justify = 'l', bool wordwrap = false);
		///True if content of the buffer has been modified
		bool modified;

		///Array of 3D points to construct the render plane
		AnnVect3 points[4];

		///Array of UV coordinates to constructed the render plane
		AnnVect2 textCoord[4];

		///Buffer of string objects
		std::string buffer[CONSOLE_BUFFER];

		///The surface used to display (aka the render plane)
		Ogre::ManualObject* displaySurface;

		///Node where the console is attached
		Ogre::SceneNode* consoleNode;

		///The actual texture of the display
		Ogre::TexturePtr texture;

		///Position of the plane using the camera as reference
		AnnVect3 offset;

		///Background texture, should be a random PNG file from the CORE resources
		Ogre::TexturePtr background;

		///OpenGL Texture IDs, to use glCopyImageSubData to clone texture quickly.
		GLuint backgroundID, textureID;

		///There's a small optimization we can do on the way we copy a texture to another, but the opengl function is available on GL4.3+ hardware only.
		bool openGL43plus;

		///The font object used, should be Vera Mono in true type format from the Gnome project, included in CORE resources
		Ogre::FontPtr font;

		///If false, the console is not visible
		bool visibility;

		///Timestamp in seconds since the start of the game the last console refresh was performed
		double lastUpdate;

		///Delay in seconds to re-refresh the console.
		const double refreshRate;
	};
}

#endif