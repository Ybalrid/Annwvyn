#pragma once

#include "systemMacro.h"

#include <Ogre.h>
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreFont.h>
#include <OgreRenderOperation.h>
#include <Hlms/Pbs/OgreHlmsPbsDatablock.h>
#include <string>

#include "AnnEngine.hpp"

#include "AnnAbstractMovable.hpp"

namespace Annwvyn
{
	///Abstract movable implementation : Manual plane object with some text rendered on it
	class DLL Ann3DTextPlane : public AnnAbstractMovable
	{
	public:
		///Text alignment flag
		enum TextAlign { ALIGN_LEFT = 'l', ALIGN_CENTER = 'c', ALIGN_RIGHT = 'r' };

		///Construct a 3D text plane. Need to provide a caption to auto render text
		/// \param w Width in meter
		/// \param h Height in meter
		/// \param caption Text to print on the plane
		/// \param size Character size in typographic point
		/// \param resolution Character "print" resolution in DPI. This will influence the texture resolution
		/// \param font Your name of the font. To reuse a font configuration
		/// \param fontTTF Name of the TTF file known by the resource manager
		Ann3DTextPlane(const float& w, const float& h, const std::string& caption = "", const int& size = 128, const float& resolution = 96.0f, const std::string& font = "defaultFont", const std::string& fontTTF = "VeraMono.ttf");

		///Class destructor
		~Ann3DTextPlane();

		///Set or change the caption
		/// \param newCaption the new text to display
		void setCaption(std::string newCaption);

		///The plane will auto-re-render at changes if set to true
		/// \param state If true, the text will be updated each time you change something
		void setAutoUpdate(bool state);

		///Change the color
		/// \param color The color of the text
		void setTextColor(AnnColor color);

		///Change the background color. Alpha blending works
		void setBackgroundColor(AnnColor color);

		///Call text re-render (if needed)
		void update();

		///Set the position of the plane
		void setPosition(AnnVect3 p) override;

		///Set the orient of the plane
		void setOrientation(AnnQuaternion q) override;

		///Set the text alignment mode
		void setTextAlign(TextAlign talign);

		///Get the position of the plane
		AnnVect3 getPosition() override;

		///Get the orientation
		AnnQuaternion getOrientation() override;

		///Set margin
		/// \param margin Margin in meters
		void setMargin(float margin);

		///Set background image
		/// \param imgName name of an image loaded in the resource manager
		void setBackgroundImage(std::string imgName);

	private:
		///Render the text
		void renderText();

		///Fill the texture with transparent black
		void clearTexture();

		///Calculate the actual vertex coordinates for the plane geometry
		void calculateVerticesForPlaneSize();

		///Check if auto-update is on. If so, call update
		void autoUpdateCheck();

		///Generate a random material name
		void generateMaterialName();

		///Create the material
		void createMaterial();

		///The actual 3D text plane
		Ogre::ManualObject* renderPlane;

		///Node where the object is attached
		Ogre::SceneNode* node;

		///Textures
		Ogre::TexturePtr texture, bgTexture;

		///Font configuration
		std::string fontName, fontTTF;

		///Text to display
		std::string caption;

		///If true, next update call will update
		bool needUpdating;

		///Length of the random material name
		const size_t materialNameLen = 30;

		///Name of the material
		std::string materialName;

		///Vertex buffer
		AnnVect3 vertices[4];

		///Static texture coordinates
		const AnnVect2 textureCoords[4] = { {0,0},{0,1},{1,0},{1,1} };

		///Various floats for size computation
		float width, height, resolutionFactor, xOffset, yOffset;

		///The font from the Ogre::FontManager
		Ogre::FontPtr font;

		///Colors
		AnnColor textColor, bgColor;

		///Text alignment
		TextAlign align;

		///if true, will update each time something is updated
		bool autoUpdate;

		///Size of the font, in points
		int fontSize;

		///Static conversions factor between Dot Per Inch and Dot Per Meters
		const float dpi2dpm = 0.0254f;

		///DPI resolution of the rendered fonts
		float dpi;

		///Margin in pixels
		unsigned int pixelMargin;

		///Margin in meters
		float margin;

		///Will use an image as background
		bool useImageAsBackground;
	};
}