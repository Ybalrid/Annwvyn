/**
 * \file AnnConsole.hpp
 * \brief Represent the output console that can be shown by calling AnnEngine::toggleOnScreenConsole()
 *		  This class create handle the texture buffer and text rendering for text display
 * \author A. Brainville (Ybalrid)
 */

#pragma once

#include "systemMacro.h"

#include <iostream>
#include <string>
#include <array>
#include <algorithm>

#include <Ogre.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <OgreRenderOperation.h>
#include <Hlms/Unlit/OgreHlmsUnlitDatablock.h>
#include <OgreHardwarePixelBuffer.h>
#include "AnnSubsystem.hpp"
#include "AnnTypes.h"
#include "AnnKeyCode.h"

#include <GL/glew.h>

namespace Annwvyn
{
	///In engine - On screen floating console
	class AnnDllExport AnnConsole : public AnnSubSystem
	{
	public:

		static constexpr auto CONSOLE_BUFFER = 17;
		static constexpr auto MAX_CONSOLE_LOG_WIDTH = 72;
		static constexpr auto BASE = 256;
		static constexpr auto MARGIN = 4;
		static constexpr auto CONSOLE_HISTORY = 64;

		///Construct the console. This should only be called by AnnEngine itself when the camera and ogre are operational
		AnnConsole();

		///Destructor
		~AnnConsole();

		///Add text to the console buffer. The console buffer will keep CONSOLE_BUFFER lines of messages in memory only
		/// \param string text to append to the console
		void append(const std::string& string);

		///Set arbitrary the visibility state of the console. Visible if no arg given, hide it if visibility = false
		void setVisible(bool visibility = true);

		///Toggle the console.
		void toggle();

		///True if text has been updated on the console and the console is visible.
		bool needUpdate() override;

		///Update the console by filling it with background texture then blitting text on it.
		///Can take some computing time depending on the size/resolution of the textures and buffer
		void update() override;

		///Move the console where it should
		void syncConsolePosition() const;

		///Clear the text draw buffer of the console
		void bufferClear();

		///Array of forbidden keyword to check
		std::array<const char*, 2> forbidden = { {"var", "auto"} };

		///This piece of code if from the Ogre Wiki. Write text to a texture using Ogre::FontManager to create glyphs
		static void WriteToTexture(const Ogre::String& str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify = 'l', bool wordwrap = false);

		///return true if pointed to an empty slot
		bool setFromPointedHistory();

		///Method to be called for navigation keys
		void notifyNavigationKey(KeyCode::code code);
	private:

		///Cleanup and run the user input.
		void runInput(std::string& input);

		///Push the inputed text into the command history
		void addToHistory(const std::string& input);

		///Run input that are not regular script commands
		bool runSpecialInput(const std::string& input);

		///Return true if the given string match with any of the forbidden keyword int the array
		bool isForbdiden(const std::string& keyword);

		///True if content of the buffer has been modified
		bool modified;

		///Array of 3D points to construct the render plane
		std::array<AnnVect3, 4> points;

		///Array of UV coordinates to constructed the render plane
		std::array<AnnVect2, 4> textCoord;

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

		///The font object used, should be Vera Mono in true type format from the Gnome project, included in CORE resources
		Ogre::FontPtr font;

		///If false, the console is not visible
		bool visibility;

		///Timestamp in seconds since the start of the game the last console refresh was performed
		double lastUpdate;

		///Delay in seconds to re-refresh the console.
		const double refreshRate;

		///Buffer of strings containing past run commands
		std::array<std::string, CONSOLE_HISTORY> commandHistory;

		///Status of the history
		int historyStatus;

		///Position of the text cursor, indexed from the end of the string.
		int cursorPos;
	};

	using AnnConsolePtr = std::shared_ptr<AnnConsole>;
}
