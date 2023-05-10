#ifndef __FROGGER_HPP
#define __FROGGER_HPP

#include "olcConsoleGameEngineSDL.hpp"

class Frogger : public olcConsoleGameEngine {

public:
    Frogger(int fontw, int fonth) {

        m_sAppName = L"Frogger";
        
        lanes_pos = std::vector<float>(10);

        cell_num_y = lanes.size();
        init_frog_cell_y = cell_num_y - 1;

        ResetFrog();

        ConstructConsole(cell_num_x * cell_size, 
                        cell_num_y * cell_size, fontw, fonth);
    }

protected:
    bool OnUserCreate() override {
        spriteBus = new olcSprite(L"resource/frogger/bus.spr");
		spriteLog = new olcSprite(L"resource/frogger/log.spr");
		spriteWater = new olcSprite(L"resource/frogger/water.spr");
		spriteFrog = new olcSprite(L"resource/frogger/frog.spr");
		spritePavement = new olcSprite(L"resource/frogger/pavement.spr");
		spriteWall = new olcSprite(L"resource/frogger/wall.spr");
		spriteHome = new olcSprite(L"resource/frogger/home.spr");
		spriteCar1 = new olcSprite(L"resource/frogger/car1.spr");
		spriteCar2 = new olcSprite(L"resource/frogger/car2.spr");

        return true;
    }

    bool OnUserDestroy() override {
        delete spriteBus ;
        delete spriteLog ;
        delete spriteWater ;
        delete spriteFrog ;
        delete spritePavement;
        delete spriteWall ;
        delete spriteHome ;
        delete spriteCar1 ;
        delete spriteCar2 ;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        UpdateStatus(fElapsedTime);
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        DrawLanes();
        DrawFrog();
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

    constexpr static int cell_size = 8;
    constexpr static int cell_num_x = 16;
    constexpr static int init_frog_cell_x = 7;
    
    std::wstring security_string = L".jklph";

    int cell_num_y;
    int init_frog_cell_y;
    float frogger_cell_x;
    float frogger_cell_y;

    void UpdateStatus(float fElapsedTime) {

        for (int i = 0; i < lanes_pos.size(); ++i) {
            lanes_pos[i] += lanes[i].first * fElapsedTime;
            if (lanes_pos[i] >= 64)
                lanes_pos[i] -= 64;
            else if (lanes_pos[i] < 0)
                lanes_pos[i] += 64;
        }
        UpdateFrogStatus(fElapsedTime);

    }

    void UpdateFrogStatus(float fElapsedTime) {
        if (m_keys[VK_UP].bReleased) {
            frogger_cell_y -= 1;
        }
        if (m_keys[VK_DOWN].bReleased) {
            frogger_cell_y += 1;
        }
        if (m_keys[VK_LEFT].bReleased) {
            frogger_cell_x -= 1;
        }
        if (m_keys[VK_RIGHT].bReleased) {
            frogger_cell_x += 1;
        }

        frogger_cell_x = std::min(cell_num_x - 1.f, std::max(0.f, frogger_cell_x));
        frogger_cell_y = std::min(cell_num_y - 1.f, std::max(0.f, frogger_cell_y));

        if (FrogInDanger()) {
            ResetFrog();
        }

        int cell_y = frogger_cell_y;
        if (cell_y >= 1 && cell_y <= 3) {
            frogger_cell_x -= lanes[cell_y].first * fElapsedTime;
        }
        frogger_cell_x = std::min(cell_num_x - 1.f, std::max(0.f, frogger_cell_x));
    }

    bool FrogInDanger() {
        int l_char_ind = ((int)(frogger_cell_x + lanes_pos[frogger_cell_y])) % 64;
        int r_char_ind = ((int)(frogger_cell_x + 1 + lanes_pos[frogger_cell_y])) % 64;
        wchar_t l_char = lanes[frogger_cell_y].second[l_char_ind];
        wchar_t r_char = lanes[frogger_cell_y].second[r_char_ind];
        return security_string.find(l_char) == std::wstring::npos ||
                security_string.find(r_char) == std::wstring::npos;
    }

    void ResetFrog() {
        frogger_cell_x = init_frog_cell_x;
        frogger_cell_y = init_frog_cell_y;
    }

    void DrawLanes() {
        for (int j = 0; j < cell_num_y; ++j) {

            int char_ind = lanes_pos[j];
            int remain = (lanes_pos[j] - char_ind) * cell_size;
            int first_remain = cell_size - remain;
            int y_upper = j * cell_size;
            int y_lower = y_upper + cell_size;

            for (int i = 0; i <= cell_num_x; ++i) {
                DrawGraphicChar(i * cell_size - remain, y_upper, lanes[j].second[(char_ind + i) % 64]);
            }
        }
    }

    void DrawFrog() {
        int x = frogger_cell_x * cell_size;
        int y = frogger_cell_y * cell_size;
        DrawSprite(x, y, spriteFrog);
    }

    void DrawGraphicChar(int x, int y, wchar_t g_char) {
        switch (g_char) // Graphics always make code verbose and less clear
        {					
            case L'a':	DrawPartialSprite(x, y, spriteBus, 0, 0, 8, 8);		break; // Bus 1
            case L's':	DrawPartialSprite(x, y, spriteBus, 8, 0, 8, 8);		break; // Bus 2
            case L'd':	DrawPartialSprite(x, y, spriteBus, 16, 0, 8, 8);	break; // Bus 3
            case L'f':	DrawPartialSprite(x, y, spriteBus, 24, 0, 8, 8);	break; // Bus 4

            case L'j':	DrawPartialSprite(x, y, spriteLog, 0, 0, 8, 8);		break; // Log Start
            case L'l':	DrawPartialSprite(x, y, spriteLog, 8, 0, 8, 8);		break; // Log Middle
            case L'k':	DrawPartialSprite(x, y, spriteLog, 16, 0, 8, 8);	break; // Log End
                                    
            case L'z': 	DrawPartialSprite(x, y, spriteCar1, 0, 0, 8, 8);	break; // Car1 Back
            case L'x':	DrawPartialSprite(x, y, spriteCar1, 8, 0, 8, 8);	break; // Car1 Front

            case L't': 	DrawPartialSprite(x, y, spriteCar2, 0, 0, 8, 8);	break; // Car2 Back
            case L'y': 	DrawPartialSprite(x, y, spriteCar2, 8, 0, 8, 8);	break; // Car2 Front

            case L'w': 	DrawPartialSprite(x, y, spriteWall, 0, 0, 8, 8);	break; // Wall
            case L'h':	DrawPartialSprite(x, y, spriteHome, 0, 0, 8, 8);	break; // Home
            case L',': 	DrawPartialSprite(x, y, spriteWater, 0, 0, 8, 8);	break; // Water
            case L'p': 	DrawPartialSprite(x, y, spritePavement, 0, 0, 8, 8); break; // Pavement
            case L'.': 	Fill(x ,y, x + cell_size, y + cell_size, L' ');	break; // Road
        }
    }
};


void TestFrogger() {
    Frogger game(8, 8);
    game.Start();
}

#endif