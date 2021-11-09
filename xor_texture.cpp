
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
    
#include <string>

    
class XorTexture : public olc::PixelGameEngine
{

public:
	XorTexture()
	{
		sAppName = "XorTexture";
	}

    
    bool OnUserCreate() override
	{
		return true;
	}


    bool OnUserUpdate(float fElapsedTime) override
	{
        // https://lodev.org/cgtutor/xortexture.html
        for (int y = 0; y < ScreenHeight(); y++)
            for (int x = 0; x < ScreenWidth(); x++)
            {
                int c = x ^ y;
                //Draw(x, y, olc::Pixel(c, c, c)); // without colors
                Draw(x, y, olc::Pixel(255 - c, c, c % 128)); // with colors
            }
        
        // FPS
        DrawString(10, 10, "FPS: " + std::to_string(GetFPS()));

        return true;
	}
};


int main()
{
	XorTexture demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}

