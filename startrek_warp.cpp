
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <string>

class WarpEffect : public olc::PixelGameEngine
{
public:
	WarpEffect()
	{
		sAppName = "Star Trek Next Generation Warp Effect";
	}

	const int nStars = 250;

	struct sStar
	{
		float fAngle = 0.0f;
		float fDistance = 0.0f; // distance away from the center of the screen
		float fSpeed = 0.0f;
		olc::Pixel col = olc::WHITE;
	};

	std::vector<sStar> vStars;
	olc::vf2d vOrigin;

	//int lineLength = 0;
	//std::string algorithm = "";

public:

	float Random(float a, float b)
	{
		return (b - a) * (float(rand()) / float(RAND_MAX)) + a;
	}

	bool OnUserCreate() override
	{
		vStars.resize(nStars);

		// Initial stars
		for (auto& star : vStars)
		{
			star.fAngle = Random(0.0f, 2.0f * 3.1459f);
			star.fSpeed = Random(10.0f, 100.0f);
			star.fDistance = Random(20.0f, 200.0f);
			float lum = Random(0.3f, 1.0f);
			star.col = olc::PixelF(lum, lum, lum, 1.0f);
		}

		vOrigin = { float(ScreenWidth() / 2), float(ScreenHeight() / 2) };

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		// Update Stars & Draw
		for (auto& star : vStars)
		{
			star.fDistance += star.fSpeed * fElapsedTime * (star.fDistance / 100.0f); //  * (star.fDistance / 100.0f) == slower for increased distance
			if (star.fDistance > 200.0f) // randomly generate a new position for the star (when crossing a trashold)
			{
				star.fAngle = Random(0.0f, 2.0f * 3.1459f);
				star.fSpeed = Random(100.0f, 150.0f); // Random(10.0f, 100.0f);
				star.fDistance = Random(1.0f, 100.0f);
				float lum = Random(0.3f, 1.0f);
				star.col = olc::PixelF(lum, lum, lum, 1.0f);
			}

			// " * (star.fDistance / 100.0f)" == non linearity (the star becomes darker the further away) [in principle]
			//Draw(olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * star.fDistance + vOrigin, star.col * (star.fDistance / 100.0f));
			Draw(olc::vf2d(cos(star.fAngle) + 0.2f, sin(star.fAngle) + 0.2f) * star.fDistance + vOrigin, star.col * (star.fDistance / 100.0f));

			DrawLineGradient(
				olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * star.fDistance + vOrigin,
				olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * (star.fDistance + 5.0f) + vOrigin,

				olc::BLUE * (star.fDistance / 50.0f),
				olc::GREEN * (star.fDistance / 50.0f)
			);
			DrawLineGradient(
				olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * (star.fDistance + 5.0f) + vOrigin,
				olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * (star.fDistance + 10.0f) + vOrigin,

				olc::YELLOW * (star.fDistance / 50.0f),
				olc::RED * (star.fDistance / 50.0f)
			);
		}
		DrawString(10, 10, "To boldly go...\nWhere no man has gone before.", olc::Pixel(34, 112, 192));

		// FPS
		//DrawString(10, 10, "FPS: " + std::to_string(GetFPS()));

		/*
		olc::vi2d mousePos = GetMousePos();
		olc::vi2d middleScreen = { ScreenWidth() / 2, ScreenHeight() / 2 };
		lineLength = (middleScreen - mousePos).mag();

		DrawLineGradient(
			middleScreen,
			{ mousePos.x, mousePos.y },
			olc::RED,
			olc::WHITE
		);

		DrawString(middleScreen, middleScreen.str());
		DrawString(mousePos, mousePos.str());
		DrawString(10, 20, "Length: " + std::to_string(lineLength));
		if (algorithm != "") DrawString(10, 30, "Algorithm: " + algorithm);
		*/

		return true;
	}

	void DrawLineGradient(const olc::vi2d& pos1, const olc::vi2d& pos2, olc::Pixel p1 = olc::WHITE, olc::Pixel p2 = olc::BLACK, uint32_t pattern = 0xFFFFFFFF)
	{
		DrawLineGradient(pos1.x, pos1.y, pos2.x, pos2.y, p1, p2, pattern);
	}

	void DrawLineGradient(int32_t x1, int32_t y1, int32_t x2, int32_t y2, olc::Pixel p1 = olc::WHITE, olc::Pixel p2 = olc::BLACK, uint32_t pattern = 0xFFFFFFFF)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		bool inverted = false; // changes from which color we are going to start interpolating

		//algorithm = ""; // TODO: DEBUG DEBUG DEBUG

		auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

		// straight lines idea by gurkanctn
		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) { std::swap(y1, y2); inverted = true; }
			for (y = y1; y <= y2; y++)
				if (rol())
				{
					float t = (float)(y - y1) / (float)(y2 - y1);
					
					if (!inverted)
						Draw(x1, y, olc::PixelLerp(p1, p2, t));
					else
						Draw(x1, y, olc::PixelLerp(p2, p1, t));
				}
			return;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) { std::swap(x1, x2); inverted = true; }
			for (x = x1; x <= x2; x++)
				if (rol())
				{
					float t = (float)(x - x1) / (float)(x2 - x1);

					if (!inverted)
						Draw(x, y1, olc::PixelLerp(p1, p2, t));
					else
						Draw(x, y1, olc::PixelLerp(p2, p1, t));
				}
			return;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}

			if (rol()) Draw(x, y, x > 0 ? p1 : p2);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				if (rol())
				{
					olc::vi2d src = { x1, y1 }, dst = { x2, y2 };
					int d = (src - dst).mag();
					float t = (float)abs(x - x1) / (float)d;
					Draw(x, y, olc::PixelLerp(p1, p2, t));
					//algorithm = "Funk-eye 2 (> dx hor)";
				}
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}

			if (rol()) Draw(x, y, p1);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				if (rol())
				{
					olc::vi2d src = { x1, y1 }, dst = { x2, y2 };
					int d = (src - dst).mag();
					float t = (float)abs(y - y1) / (float)d;
					Draw(x, y, olc::PixelLerp(p1, p2, t));
					//algorithm = "Funk-eye 4 (> dy hor)";
				}
			}
		}
	}
};

int main()
{
	WarpEffect demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}

