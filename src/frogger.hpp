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

            Fill(0, y_upper, first_remain, y_lower, lanes[j].second[char_ind]);
            for (int i = 1; i < cell_num_x; ++i) {
                Fill(i * cell_size - remain, y_upper, (i + 1) * cell_size - remain, y_lower, 
                        lanes[j].second[(char_ind + i) % 64]);
            }
            Fill(cell_num_x * cell_size - remain, y_upper, cell_num_x * cell_size, y_lower, 
                        lanes[j].second[(char_ind + cell_num_x) % 64]);
        }
    }
};


void TestFrogger() {
    Frogger game(8, 8);
    game.Start();
}

#endif