#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


// Doom Fire Effect
// as explained by Fabien Sanglard (https://fabiensanglard.net/doom_fire_psx/)
// video demonstrating the technique (by praetor64): https://www.youtube.com/watch?v=B7iacc3HiVE

class DoomFireEffect : public olc::PixelGameEngine
{
public:
	DoomFireEffect()
	{
		sAppName = "Doom Fire Effect";
	}


private:

	// fire dimensions + color palette
#define FIRE_WIDTH			256
#define FIRE_HEIGHT			240
#define NUM_COLORS			37
	unsigned char palette[NUM_COLORS][3] = { // palette from darkest to lightest color
		{ 0x07,0x07,0x07 }, // near black
		{ 0x1F,0x07,0x07 },
		{ 0x2F,0x0F,0x07 },
		{ 0x47,0x0F,0x07 },
		{ 0x57,0x17,0x07 },
		{ 0x67,0x1F,0x07 },
		{ 0x77,0x1F,0x07 },
		{ 0x8F,0x27,0x07 },
		{ 0x9F,0x2F,0x07 },
		{ 0xAF,0x3F,0x07 },
		{ 0xBF,0x47,0x07 },
		{ 0xC7,0x47,0x07 },
		{ 0xDF,0x4F,0x07 },
		{ 0xDF,0x57,0x07 },
		{ 0xDF,0x57,0x07 },
		{ 0xD7,0x5F,0x07 },
		{ 0xD7,0x5F,0x07 },
		{ 0xD7,0x67,0x0F },
		{ 0xCF,0x6F,0x0F },
		{ 0xCF,0x77,0x0F },
		{ 0xCF,0x7F,0x0F },
		{ 0xCF,0x87,0x17 },
		{ 0xC7,0x87,0x17 },
		{ 0xC7,0x8F,0x17 },
		{ 0xC7,0x97,0x1F },
		{ 0xBF,0x9F,0x1F },
		{ 0xBF,0x9F,0x1F },
		{ 0xBF,0xA7,0x27 },
		{ 0xBF,0xA7,0x27 },
		{ 0xBF,0xAF,0x2F },
		{ 0xB7,0xAF,0x2F },
		{ 0xB7,0xB7,0x2F },
		{ 0xB7,0xB7,0x37 },
		{ 0xCF,0xCF,0x6F },
		{ 0xDF,0xDF,0x9F },
		{ 0xEF,0xEF,0xC7 },
		{ 0xFF,0xFF,0xFF }, // white
	};

	int framebuffer[FIRE_HEIGHT * FIRE_WIDTH];

// update & speed control
#define FIRE_TICK_UPDATE	(1000 / 30)	// the fire will be updated at 30 FPS
#define FIRE_TICK_STOP		(1000 / 10)	// the fire will accept stop command at 10 FPS
	float fire_tick = 0.0f;
	float fire_tick_stop = 0.0f;

public:

	void start_fire() {
		// start fire by lighting last row again
		for (int c = 0; c < FIRE_WIDTH; c++) {
			framebuffer[(FIRE_HEIGHT - 1) * FIRE_WIDTH + c] = NUM_COLORS - 1;
		}
	}

	void stop_fire() {
		for (int c = 0; c < FIRE_WIDTH; c++) {
			if (framebuffer[(FIRE_HEIGHT - 1) * FIRE_WIDTH + c] > 0) {
				framebuffer[(FIRE_HEIGHT - 1) * FIRE_WIDTH + c] -= 2; // darkens a little
			}
		}
	}

	bool OnUserCreate() override
	{
		// starting fire map
		for (int r = 0; r < FIRE_HEIGHT; r++) {
			for (int c = 0; c < FIRE_WIDTH; c++) {
				framebuffer[r * FIRE_WIDTH + c] = 0; // "black" (or our darkest color in the palette)
			}
		}

		start_fire();

		return true;
	}


	void spread_fire(int from) {
		if (framebuffer[from] == 0) // if the fire is already out...
			framebuffer[from - FIRE_WIDTH] = 0; // ...also put out the fire above
		else
		{
			int rand = std::rand() % 3;
			int randx = std::rand() % 3;
			int to = from - randx + 1; // randomize x axis position

			framebuffer[to - FIRE_WIDTH] = framebuffer[from] - (rand & 1); // between 1 and 3 dimmer
		}
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// input
		if (GetKey(olc::E).bHeld) { if (fire_tick_stop++ > FIRE_TICK_STOP) { stop_fire(); fire_tick_stop = 0.0f; } }
		if (GetKey(olc::I).bHeld) start_fire();


		// spread the fire
		if (fire_tick++ > FIRE_TICK_UPDATE) {
			for (int r = 1; r < FIRE_HEIGHT; r++) { // skip first row
				for (int c = 0; c < FIRE_WIDTH; c++) {
					spread_fire(r * FIRE_WIDTH + c); // spread the fire from the current pixel
				}
			}

			fire_tick = 0.0f;
		}


		// draw the fire
		for (int r = 0; r < FIRE_HEIGHT; r++) {
			for (int c = 0; c < FIRE_WIDTH; c++) {
				int index = framebuffer[r * FIRE_WIDTH + c];
				unsigned char* rgb = palette[index];
				olc::Pixel color(rgb[0], rgb[1], rgb[2]);
				
				Draw(c, r, color);
			}
		}

		// - FPS
		uint32_t fps = GetFPS();
		DrawString(10, 10, "FPS: " + std::to_string(fps), (fps < 30) ? olc::RED : olc::GREEN);

		// - instructions
		DrawString(10, 30, "E = Extinguish", olc::BLUE);
		DrawString(10, 40, "I = Ignite", olc::MAGENTA);


		return !GetKey(olc::ESCAPE).bPressed;
	}
};


int main()
{
	DoomFireEffect demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
