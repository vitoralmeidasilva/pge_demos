#define _USE_MATH_DEFINES // to use M_PI in Microsoft Visual C++ Compiler (https://stackoverflow.com/questions/6563810/m-pi-works-with-math-h-but-not-with-cmath-in-visual-studio)

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Source:
// https://fabiensanglard.net/Tunnel/index.php
// https://lodev.org/cgtutor/tunnel.html

class Tunnel : public olc::PixelGameEngine
{
private:
	std::unique_ptr<olc::Sprite> texture;
	std::vector<int> angles, distances;
	int forward = 0, roll = 0;
	float ratio = 32; // the ratio between the width and height the texture will be having on screen, or how long the texture stretches out in the distance

public:
	Tunnel()
	{
		sAppName = "Old School Tunnel Effect";
	}

	void ResetValues()
	{
		forward = roll = 0;
		ratio = 32;
	}

	// create a xor texture (https://lodev.org/cgtutor/xortexture.html)
	void CreateXORTexture(int width, int height)
	{
		texture = std::make_unique<olc::Sprite>(width, height);
		SetDrawTarget(texture.get());
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int c = x ^ y;
				Draw(x, y, olc::Pixel(c, c, c));
			}
		}
		SetDrawTarget(nullptr);
	}

	void PreCalculateTables()
	{
		int w = ScreenWidth();
		int h = ScreenHeight();
		int s = w * h;
		angles.resize(s);
		distances.resize(s);

		// generate non-linear transformation table
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int id = y * w + x; // 2d to 1d conversion

				// The distance table contains for every pixel of the screen, 
				// the inverse of the distance to the center of the screen this pixel has.
				// This gives pixels of the center of the screen a very high value 
				// while the pixels on the sides of the screen get a low value 
				// (these pixels represent parts of the tunnel close to the camera).
				distances[id] = static_cast<int>(ratio * texture.get()->height / sqrt((x - w / 2.0f) * (x - w / 2.0f) + (y - h / 2.0f) * (y - h / 2.0f))) % texture.get()->height;

				// The angle table contains the angle of every pixel of the screen, 
				// where the center of the screen represents the origin.
				angles[id] = static_cast<int>(0.5f * texture.get()->width * atan2(y - h / 2.0f, x - w / 2.0f) / M_PI);
			}
		}
	}

	bool OnUserCreate() override
	{
		CreateXORTexture(ScreenWidth(), ScreenHeight());
		PreCalculateTables();

		return true;
	}


	bool OnUserUpdate(float fElapsedTime) override
	{
		// ==============
		// Input
		// ==============
		if (GetKey(olc::UP).bHeld) forward++;
		if (GetKey(olc::DOWN).bHeld) forward--;
		if (GetKey(olc::RIGHT).bHeld) roll++;
		if (GetKey(olc::LEFT).bHeld) roll--;
		if (GetKey(olc::PGDN).bHeld) { ratio--; PreCalculateTables(); }
		if (GetKey(olc::PGUP).bHeld) { ratio++; PreCalculateTables(); }
		if (GetKey(olc::R).bPressed) ResetValues();

		// auto-animation
		//forward++;
		//roll--; // TODO: why when roll is <= -130, a part of the screen becomes black?

		// ==============
		// Drawing
		// ==============
		Clear(olc::BLACK);
		//DrawSprite(0, 0, xorTexture.get()); // DEBUG: draw the whole texture to the screen

		// calculate the shift values out of the animation value
		int shiftX = (int)(texture.get()->width + forward);
		int shiftY = (int)(texture.get()->height + roll);

		for (int y = 0; y < ScreenHeight(); y++)
		{
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int id = y * ScreenWidth() + x;
				int c_x = static_cast<int>(distances[id] + shiftX) % texture.get()->width;
				int c_y = static_cast<int>(angles[id] + shiftY) % texture.get()->height;
				Draw({ x, y }, texture->GetPixel({ c_x, c_y }));
			}
		}

		// ==============
		// Text Output
		// ==============
		// - FPS
		uint32_t fps = GetFPS();
		DrawString(10, 10, "FPS: " + std::to_string(fps), (fps < 30) ? olc::RED : olc::GREEN);
		// - misc
		DrawString(10, 20, "AR (PGDN, PGUP): " + std::to_string(ratio), olc::CYAN);  // aspect ratio
		DrawString(10, 30, "FORWARD (UP, DOWN): " + std::to_string(forward), olc::CYAN);
		DrawString(10, 40, "ROLL (LEFT, RIGHT): " + std::to_string(roll), olc::CYAN);
		// - bottom status bar
		FillRect(0, ScreenHeight() - 10, ScreenWidth(), ScreenHeight(), olc::DARK_BLUE);
		DrawString(28, ScreenHeight() - 9, "PRESS \"R\" TO RESET VALUES", olc::WHITE);

		// ESC ends the program
		return !(GetKey(olc::ESCAPE).bPressed);
	}
};


int main()
{
	Tunnel demo;

	if (demo.Construct(256, 256, 3, 3))
		demo.Start();

	return 0;
}

