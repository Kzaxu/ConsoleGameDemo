#ifndef __A_STAR_HPP
#define __A_STAR_HPP

#include "olcConsoleGameEngineSDL.hpp"
#include <queue>

class AStarGame : public olcConsoleGameEngine {

public:

    enum BlockStatus {
        DEFAULT = 0x00,
        VISIT = 0x01,
        BLOCK = 0x02,
        START = 0x04,
        END = 0x08,
    };

    struct Block {
        Block* parent {nullptr};
        int status = 0;
        int local_score = INT32_MAX / 2;
        int global_score = INT32_MAX / 2;
        int x;
        int y;
    };

    AStarGame(int cell_num_x, int cell_num_y) : cell_num_x(cell_num_x), cell_num_y(cell_num_y) {
        
        m_sAppName = L"A-Star Game";

        ConstructConsole(cell_num_x * cell_size, cell_num_y * cell_size, 
                        font_size, font_size);
    }

protected:
    bool OnUserCreate() override {
        blocks = new Block[cell_num_x * cell_num_y]();
        
        for (int i = 0; i < cell_num_x; ++i) {
            for (int j = 0; j < cell_num_y; ++j) {
                blocks[BlockOffset(i, j)].x = i;
                blocks[BlockOffset(i, j)].y = j;
            }
        }

        st_block_x = 1;
        st_block_y = cell_num_y / 2;
        ed_block_x = cell_num_x - 2;
        ed_block_y = cell_num_y / 2;

        blocks[BlockOffset(st_block_x, st_block_y)].status = START;
        blocks[BlockOffset(ed_block_x, ed_block_y)].status = END;

        ResetBlocks();
        SolveAStar();
        return true;
    }

    bool OnUserDestroy() override {
        delete [] blocks;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        UpdateStatus(fElapsedTime);
        DrawBackground();
        DrawBlocks();
        DrawPath();
        return true;
    }

private:

    constexpr static int cell_size = 9;
    constexpr static int half_cell_size = cell_size / 2;
    constexpr static int block_size = cell_size - half_cell_size;
    constexpr static int font_size = 4;
    constexpr static bool early_stop = true;

    int cell_num_x;
    int cell_num_y;
    int st_block_x;
    int st_block_y;
    int ed_block_x;
    int ed_block_y;

    Block* blocks;

    int BlockOffset(int x, int y) {
        return y * cell_num_x + x;
    }

    void UpdateStatus(float fElapsedTime) {
        HandleMouseInput();
    }

    void HandleMouseInput() {
        int m_x = GetMouseX(), m_y = GetMouseY();
        bool need_reset = false;
        if (m_x < 0 || m_x >= ScreenWidth() || m_y < 0 || m_y >= ScreenHeight()) {
            return;
        }
        m_x /= cell_size;
        m_y /= cell_size;
        if (m_keys[VK_LSHIFT].bHeld && m_mouse[0].bReleased 
            && !(blocks[BlockOffset(m_x, m_y)].status & (END | BLOCK))) {
            blocks[BlockOffset(st_block_x, st_block_y)].status ^= START;
            st_block_x = m_x, st_block_y = m_y;
            blocks[BlockOffset(st_block_x, st_block_y)].status |= START;
            need_reset = true;
        }

        if (m_keys[VK_LCONTROL].bHeld && m_mouse[0].bReleased 
            && !(blocks[BlockOffset(m_x, m_y)].status & (START | BLOCK))) {
            blocks[BlockOffset(ed_block_x, ed_block_y)].status ^= END;
            ed_block_x = m_x, ed_block_y = m_y;
            blocks[BlockOffset(ed_block_x, ed_block_y)].status |= END;
            need_reset = true;
        }

        if (m_mouse[0].bReleased 
            && !(blocks[BlockOffset(m_x, m_y)].status & START)
            && !(blocks[BlockOffset(m_x, m_y)].status & END)) {
            blocks[BlockOffset(m_x, m_y)].status ^= BLOCK;
            need_reset = true;
        }

        if (need_reset) {
            ResetBlocks();
            SolveAStar();
        };
    }

    void ResetBlocks() {
        for (int i = 0; i < cell_num_x * cell_num_y; ++i) {
            Block* block = &blocks[i];
            block->status &= ~VISIT;
            block->parent = nullptr;
            block->local_score = INT32_MAX / 2;
            block->global_score = INT32_MAX / 2;
        }
        Block* st_block = &blocks[BlockOffset(st_block_x, st_block_y)];
        Block* ed_block = &blocks[BlockOffset(ed_block_x, ed_block_y)];

        st_block->local_score = 0;
        st_block->global_score = std::abs(st_block_x - ed_block_x) + abs(st_block_y - ed_block_y);
    }

    void SolveAStar() {

        auto cmp = [&](Block* b1, Block* b2) {
            return b1->global_score < b2->global_score;
        };

        auto est_dest = [&](int x, int y) {
            return std::abs(x - ed_block_x) + std::abs(y - ed_block_y);
        };

        auto update_block = [&](Block* child, Block* parent) {
            if (parent->local_score + 1 < child->local_score) {
                child->local_score = parent->local_score + 1;
                child->global_score = child->local_score + est_dest(child->x, child->y);
                child->parent = parent;
            }
        };

        constexpr int directs[8][2] {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };
        std::list<Block*> not_test_blocks;

        not_test_blocks.emplace_back(&blocks[BlockOffset(st_block_x, st_block_y)]);
        not_test_blocks.front()->status ^= VISIT;

        while (!not_test_blocks.empty()) {
            not_test_blocks.sort(cmp);
            Block* cur_block = not_test_blocks.front();
            if (early_stop && cur_block->status & END) {
                break;
            }
            not_test_blocks.pop_front();
            for (int i = 0; i < 8; ++i) {
                int x_ = cur_block->x + directs[i][0];
                int y_ = cur_block->y + directs[i][1];
                if(x_ < 0 || y_ < 0 || x_ >= cell_num_x || y_ >= cell_num_y) {
                    continue;
                }
                Block* neigbor = &blocks[BlockOffset(x_, y_)];
                if (neigbor->status & BLOCK) {
                    continue;
                }
                if (!(neigbor->status & VISIT)) {
                    not_test_blocks.push_back(neigbor);
                    neigbor->status |= VISIT;
                }
                update_block(neigbor, cur_block);
            }
        }
    }

    void DrawBackground() {
        for (int i = 0; i < cell_num_y; ++i) {
            Fill(half_cell_size + i * cell_size, 0, half_cell_size + i * cell_size + 1, ScreenHeight(), PIXEL_SOLID, FG_DARK_BLUE);
            Fill(0, half_cell_size + i * cell_size, ScreenWidth(), half_cell_size + i * cell_size + 1, PIXEL_SOLID, FG_DARK_BLUE);
        }
    }

    void DrawBlock(Block* block) {
        int x_ = block->x * cell_size + half_cell_size / 2;
        int y_ = block->y * cell_size + half_cell_size / 2;
        COLOUR col;
        int status = block->status;
        if (status & END) {
            col = FG_RED;
        } else if (status & START) {
            col = FG_GREEN;
        } else if (status & BLOCK) {
            col = FG_GREY;
        } else if (status & VISIT) {
            col = FG_BLUE;
        } else {
            col = FG_DARK_BLUE;
        }
        Fill(x_, y_, x_ + block_size, y_ + block_size, PIXEL_SOLID, col);
    }

    void DrawBlocks() {

        for (int i = 0; i < cell_num_x * cell_num_y; ++i) {
            DrawBlock(&blocks[i]);
        }
    }

    void DrawPath() {
        Block* block = &blocks[BlockOffset(ed_block_x, ed_block_y)];
        while (block->parent) {
            int s_x = block->x * cell_size + half_cell_size;
            int s_y = block->y * cell_size + half_cell_size;
            int e_x = block->parent->x * cell_size + half_cell_size;
            int e_y = block->parent->y * cell_size + half_cell_size ; 
            DrawLine(s_x, s_y, e_x, e_y, PIXEL_SOLID, FG_YELLOW);
            block = block->parent;
        } 
    }

};


void TestAStarGame() {
    AStarGame game(20, 20);
    game.Start();
}

#endif