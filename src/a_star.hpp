#ifndef __A_STAR_HPP
#define __A_STAR_HPP

#include "olcConsoleGameEngineSDL.hpp"

class AStarGame : public olcConsoleGameEngine {

public:

    enum BlockStatus {
        VISIT = 0x01,
        BLOCK = 0x02,
        START = 0x04,
        END = 0x08,
    };

    AStarGame(int cell_num_x, int cell_num_y) : cell_num_x(cell_num_x), cell_num_y(cell_num_y) {
        
        m_sAppName = L"A-Star Game";

        ConstructConsole(cell_num_x * cell_size, cell_num_y * cell_size, 
                        font_size, font_size);
    }

protected:
    bool OnUserCreate() override {
        blocks = new BlockStatus[cell_num_x * cell_num_y]();
        return true;
    }

    bool OnUserDestroy() override {
        delete [] blocks;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        DrawBackground();
        DrawBlocks();
        return true;
    }

private:

    constexpr static int cell_size = 9;
    constexpr static int half_cell_size = cell_size / 2;
    constexpr static int block_size = cell_size - half_cell_size;
    constexpr static int font_size = 4;

    int cell_num_x;
    int cell_num_y;
    BlockStatus* blocks;

    void UpdateStatus(float fElapsedTime) {

    }

    void DrawBackground() {
        for (int i = 0; i < cell_num_y; ++i) {
            Fill(half_cell_size + i * cell_size, 0, half_cell_size + i * cell_size + 1, ScreenHeight(), PIXEL_SOLID, FG_DARK_BLUE);
            Fill(0, half_cell_size + i * cell_size, ScreenWidth(), half_cell_size + i * cell_size + 1, PIXEL_SOLID, FG_DARK_BLUE);
        }
    }

    void DrawBlock(int x, int y, COLOUR col) {
        int x_ = x * cell_size + half_cell_size / 2;
        int y_ = y * cell_size + half_cell_size / 2;
        Fill(x_, y_, x_ + block_size, y_ + block_size, PIXEL_SOLID, col);
    }

    void DrawBlocks() {

        auto offset = [&](int x, int y) {
            return blocks[y * cell_num_x + x];
        };
        for (int i = 0; i < cell_num_x; ++i) {
            for (int j = 0; j < cell_num_y; ++j) {
                COLOUR col;
                BlockStatus status = offset(i, j);
                if (status & 0x08) {
                    col = FG_RED;
                } else if (status & 0x04) {
                    col = FG_GREEN;
                } else if (status & 0x02) {
                    col = FG_GREY;
                } else if (status & 0x01) {
                    col = FG_BLUE;
                } else {
                    col = FG_DARK_BLUE;
                }
                DrawBlock(i, j, col);
            }
        }
    }

};


void TestAStarGame() {
    AStarGame game(15, 15);
    game.Start();
}

#endif