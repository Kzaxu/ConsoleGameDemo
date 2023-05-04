#ifndef __MATRIX_HPP
#define __MATRIX_HPP

#include "olcConsoleGameEngineSDL.hpp"
#include <iostream>
#include <string>


class OneLoneCoder_Matrix : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Matrix()
	{
		m_sAppName = L"Matrix";
	}

private:
	struct sStreamer
	{
		int nColumn = 0;
		float fPosition = 0;
		float fSpeed = 0;
		std::wstring sText;
	};

	std::list<sStreamer> listStreamers;

	int nMaxStreamers = 300;

	wchar_t RandomCharacter()
	{
		return (wchar_t)(rand() % 0x1EF + 0x00C0);
		return (wchar_t)(rand() % 93 + 33); // Random ASCII
	}

	void PrepareStreamer(sStreamer *s)
	{
		s->nColumn = rand() % ScreenWidth();
		s->fPosition = 0;
		s->fSpeed = rand() % 40 + 5;
		s->sText.clear();

		int nStreamerLength = rand() % 80 + 10;
		for (int i = 0; i < nStreamerLength; i++)
			s->sText.append(1, RandomCharacter());

		//s->sText = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	}


protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		for (int n = 0; n < nMaxStreamers; n++)
		{
			sStreamer s;
			PrepareStreamer(&s);
			listStreamers.push_back(s);
		}
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		for (auto &s : listStreamers)
		{
			s.fPosition += fElapsedTime * s.fSpeed;			
			for (int i = 0; i < s.sText.size(); i++)
			{
				// If you hate ternary operators do this
				// start ->
				short col = s.fSpeed < 15.0f ? FG_DARK_GREEN : FG_GREEN; // ;-)
				if (i == 0)
					col = FG_WHITE;
				else
					if (i <= 3)
						col = FG_GREY;

				int nCharIndex = (i - (int)s.fPosition) % s.sText.size();

				Draw(s.nColumn, (int)s.fPosition - i, s.sText[nCharIndex], col);
				// <- end

				// If you like them, do this!
				// Draw(s.nColumn, (int)s.fPosition - i, s.sText[(abs(i - (int)s.fPosition) % s.sText.size())], i == 0 ? FG_WHITE : i<=3 ? FG_GREY: i>=s.sText.size()-2 || s.fSpeed < 15.0f? FG_DARK_GREEN: FG_GREEN);
				
				// Missed this out on live stream, occasionally glitch a character
				if (rand() % 1000 < 5)
					s.sText[i] = RandomCharacter();
			}

			if ( s.fPosition - s.sText.size() >= ScreenHeight())
				PrepareStreamer(&s);

		}
		return true;
	}
};

#endif
