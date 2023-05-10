#ifndef __FROGGER_HPP
#define __FROGGER_HPP

#include "olcConsoleGameEngineSDL.hpp"

class Frogger : public olcConsoleGameEngine {

public:
    Frogger(int fontw, int fonth) {
        m_sAppName = L"Frogger";
        cell_num_y = lanes.size();
        lanes_pos = std::vector<float>(10);
        ConstructConsole(cell_num_x * cell_size, 
                        cell_num_y * cell_size, fontw, fonth);
    }

protected:
    bool OnUserCreate() override {
        return true;
    }

    bool OnUserDestroy() override {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        UpdateStatus(fElapsedTime);
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        DrawLanes();
        return true;
    }

private:
    olcSprite *spriteBus = nullptr;
	olcSprite *spriteLog = nullptr;
	olcSprite *spriteWater = nullptr;
	olcSprite *spriteFrog = nullptr;
	olcSprite *spritePavement = nullptr;
	olcSprite *spriteCar1 = nullptr;
	olcSprite *spriteCar2 = nullptr;
	olcSprite *spriteWall = nullptr;
	olcSprite *spriteHome = nullptr;

    std::vector<std::pair<float, std::wstring>> lanes {
        {0.0f,  L"wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // 64 elements per lane
		{-3.0f, L",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"}, 
		{3.0f,  L",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"},
		{2.0f,  L",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"},
		{0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"},
		{-3.0f, L"....asdf.......asdf....asdf..........asdf........asdf....asdf..."},
		{ 3.0f, L".....ty..ty....ty....ty.....ty........ty..ty.ty......ty.......ty"},
		{-4.0f, L"..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."},		
		{2.0f,  L"..ty.....ty.......ty.....ty......ty..ty.ty.......ty....ty......."},
		{0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"}
    };
    std::vector<float> lanes_pos;

    constexpr static int cell_size = 10;
    constexpr static int cell_num_x = 16;
    int cell_num_y;

    void UpdateStatus(float fElapsedTime) {
        for (int i = 0; i < lanes_pos.size(); ++i) {
            lanes_pos[i] += lanes[i].first * fElapsedTime;
            if (lanes_pos[i] >= 64)
                lanes_pos[i] -= 64;
            else if (lanes_pos[i] < 0)
                lanes_pos[i] += 64;
        }
    }

    void DrawLanes() {
        for (int j = 0; j < cell_num_y; ++j) {

            int char_ind = lanes_pos[j];
            int remain = (lanes_pos[j] - char_ind) * cell_size;
            int first_remain = cell_size - remain;
            int y_upper = j * cell_size;
            int y_lower = y_upper + cell_size;

            for (int i = 0; i <= cell_num_x; ++i) {
                Fill(i * cell_size - remain, y_upper, (i + 1) * cell_size - remain, y_lower, 
                        lanes[j].second[(char_ind + i) % 64]);
            }
        }
    }

    void DrawGraphicChar(int x, int y, wchar_t g_char) {
        switch (g_char) // Graphics always make code verbose and less clear
        {					
            case L'a':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 0, 0, 8, 8);		break; // Bus 1
            case L's':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 8, 0, 8, 8);		break; // Bus 2
            case L'd':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 16, 0, 8, 8);	break; // Bus 3
            case L'f':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 24, 0, 8, 8);	break; // Bus 4

            case L'j':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 0, 0, 8, 8);		break; // Log Start
            case L'l':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 8, 0, 8, 8);		break; // Log Middle
            case L'k':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 16, 0, 8, 8);	break; // Log End
                                    
            case L'z': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 0, 0, 8, 8);	break; // Car1 Back
            case L'x':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 8, 0, 8, 8);	break; // Car1 Front

            case L't': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 0, 0, 8, 8);	break; // Car2 Back
            case L'y': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 8, 0, 8, 8);	break; // Car2 Front

            case L'w': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWall, 0, 0, 8, 8);	break; // Wall
            case L'h':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteHome, 0, 0, 8, 8);	break; // Home
            case L',': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWater, 0, 0, 8, 8);	break; // Water
            case L'p': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spritePavement, 0, 0, 8, 8); break; // Pavement
            case L'.': 	Fill((x + i)*nCellSize - nCellOffset, y*nCellSize, (x + i + 1)*nCellSize - nCellOffset, (y + 1)*nCellSize, L' ');	break; // Road
        }
    }
};


void TestFrogger() {
    Frogger game(8, 8);
    game.Start();
}

#endif