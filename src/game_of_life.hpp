#ifndef __GAME_OF_LIFE_HPP
#define __GAME_OF_LIFE_HPP

#include "olcConsoleGameEngineSDL.hpp"

class GameOfLife : public olcConsoleGameEngine {

public:
    enum InitMethod {
        RANDOM,
        RPentonimo,
        GosperGliderGun
    };

    GameOfLife(InitMethod m = RANDOM) : init_method(m) {
        m_sAppName = L"Game Of Life";
    }

protected:
    bool OnUserCreate() override {
        m_state = new int[ScreenHeight() * ScreenWidth()]();
        m_output = new int[ScreenHeight() * ScreenWidth()]();

        switch (init_method) {
        case GosperGliderGun:
            InitGosperGliderGun();
            break;
        case RPentonimo:
            InitRPentonimo();
            break;
        case RANDOM:
        default:
            RandomInit();
            break;
        }

        return true;
    }

    bool OnUserDestroy() override {
        delete [] m_state;
        delete [] m_output;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        auto Cell = [&](int x, int y) {
            return m_state[y * ScreenWidth() + x];
        };
        memcpy(m_state, m_output, ScreenWidth() * ScreenHeight() * sizeof(int));

        for (int i = 1; i < ScreenWidth() - 1; ++i) {
            for (int j = 1; j < ScreenHeight() - 1; ++j) {
                int neigbors = Cell(i - 1, j - 1) + Cell(i, j - 1) + Cell(i + 1, j - 1) + 
                               Cell(i - 1, j + 0) + 0              + Cell(i + 1, j + 0) +
                               Cell(i - 1, j + 1) + Cell(i, j + 1) + Cell(i + 1, j + 1);
                int offset = j * ScreenWidth() + i;

                m_output[offset] = m_state[offset] ? neigbors == 2 || neigbors == 3 : neigbors == 3;

                if (m_output[offset]) {
                    Draw(i, j, PIXEL_SOLID, FG_WHITE);
                } 
                else {
                    Draw(i, j, PIXEL_SOLID, FG_BLACK);
                }
            }
        }

        return true;
    }

private:
    int* m_state;
    int* m_output;
    InitMethod init_method;

    void RandomInit() {
        for (int i = 1; i < ScreenWidth(); ++i) {
            for (int j = 1; j < ScreenHeight(); ++j) {
                m_output[j * ScreenWidth() + i] = rand() % 2;
            }
        }
    }

    void SetString(int x, int y, std::wstring str) {
        int len = str.length();
        for (int i = 0; i < len; ++i) {
            if (str[i] == L'#') {
                m_output[y * ScreenWidth() + x + i] = 1;
            }
        }
    }

    void InitRPentonimo() {
        int st_x = ScreenWidth() / 3;
        int st_y = ScreenHeight() / 2;
        SetString(st_x, st_y,     L"  ## ");
        SetString(st_x, st_y + 1, L" ##  ");
        SetString(st_x, st_y + 2, L"  #  ");
    }

    void InitGosperGliderGun() {
        int st_x = ScreenWidth() / 3;
        int st_y = ScreenHeight() / 2 - 4;
        int index = 0;
        auto set = [&](std::wstring str) {
            SetString(st_x, st_y + index, str);
            index++;
        };
        set(L"........................#............");
		set(L"......................#.#............");
		set(L"............##......##............##.");
		set(L"...........#...#....##............##.");
		set(L"##........#.....#...##...............");
		set(L"##........#...#.##....#.#............");
		set(L"..........#.....#.......#............");
		set(L"...........#...#.....................");
		set(L"............##.......................");
    }
};

void TestGameOfLife() {
	GameOfLife game(GameOfLife::InitMethod::GosperGliderGun);
	game.ConstructConsole(200, 150, 4, 4);
	game.Start();
}

#endif