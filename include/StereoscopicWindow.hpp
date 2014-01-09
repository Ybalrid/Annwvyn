#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
namespace CEGUI
{
    class StereoscopicWindow
    {
        public:
            StereoscopicWindow();
            void createWindow(const char* type , const char* name);
            void setText(const char* text);
            void setSize(float x, float y);
            void setPosition(float x, float y);
            void setCenterOffset(float offset);
            void addChildToSheet(CEGUI::Window* sheet);
        private:
            float calculatePosX(float posX, float sizeX, float offset, bool right = false);

            float centerOffset;
            bool knownOffset;
            float sizeX, sizeY;
            float posX, posY;
            CEGUI::Window *left, *right;
    };
}
