#ifndef __MAZES_HPP
#define __MAZES_HPP

#include "olcConsoleGameEngineSDL.hpp"
#include <queue>
#include <utility>
#include <thread>
#include <chrono>
#include <stdlib.h>


class MyGameMazes : public olcConsoleGameEngine {

public:
    enum CellStatus {
        CELL_PATH_N = 0x01,
        CELL_PATH_S = 0x02,
        CELL_PATH_E = 0x04,
        CELL_PATH_W = 0x08,
        CELL_VISIT = 0x10
    };

    MyGameMazes(int w, int h, int path_width) :
        m_maze_width(w), m_maze_height(h), m_path_width(path_width) {

        m_sAppName = L"Mazes";
        ConstructConsole(w * path_width + w - 1, h * path_width + h - 1, 
            8, 8);

    }

protected:

    bool OnUserCreate() override {
        m_mazes = new int[m_maze_width * m_maze_height]();

        int init_x = 0, init_y = 0;
        stack.emplace_back(init_x, init_y);
        m_mazes[Offset(init_x, init_y)] |= CELL_VISIT;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ');
        UpdateMazes();
        DrawAllMazeBlock();
        DrawCurrentPoint();
        return true;
    }

    bool OnUserDestroy() override {
        delete [] m_mazes;
        return true;
    }

private:
    int m_maze_width;
    int m_maze_height;
    int m_path_width;
    int* m_mazes;

    std::deque<std::pair<int, int>> stack;

    int Offset(int x, int y) {
        return y * m_maze_width + x;
    }

    void DrawMazeBlock(int x, int y) {
        int offset = Offset(x, y);
        short block_color = m_mazes[offset] & CELL_VISIT ? FG_WHITE : FG_BLUE;
        Fill(x * (m_path_width + 1), y * (m_path_width  + 1), 
             (x + 1) * (m_path_width + 1) - 1, (y + 1) * (m_path_width + 1) - 1,
             PIXEL_SOLID, block_color);
        if (m_mazes[offset] & CELL_PATH_S) {
            for (int i = 0; i < m_path_width; ++i) {
                Draw(x * (m_path_width + 1) + i, (y + 1) * (m_path_width + 1) - 1);
            }
        }
        if (m_mazes[offset] & CELL_PATH_E) {
            for (int i = 0; i < m_path_width; ++i) {
                Draw((x + 1) * (m_path_width + 1) - 1, y * (m_path_width + 1) + i);
            }            
        }
    }

    void DrawAllMazeBlock() {
        for (int i = 0; i < m_maze_width; ++i) {
            for (int j = 0; j < m_maze_height; ++j) {
                DrawMazeBlock(i, j);
            }
        }
    }

    void DrawCurrentPoint() {
        if (stack.empty()) return;
        auto& [cur_x,cur_y] = stack.back();
        Fill(cur_x * (m_path_width + 1), cur_y * (m_path_width  + 1), 
             (cur_x + 1) * (m_path_width + 1) - 1, (cur_y + 1) * (m_path_width + 1) - 1,
             PIXEL_SOLID, FG_GREEN);
    }

    
    auto GetNeighbors(int x, int y) {
        std::vector<std::pair<int, int>> res;
        constexpr int directs[4][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };
        for (int i = 0; i < 4; ++i) {
            int x_ = x + directs[i][0], y_ = y + directs[i][1];
            if (x_ >= 0 && x_ < m_maze_width 
                && y_ >= 0 && y_ < m_maze_height
                && !(m_mazes[Offset(x_, y_)] & CELL_VISIT)) {
                res.emplace_back(x_, y_);
            }
        }
        return res;
    }

    void UpdateMazesWall(int src_x, int src_y, int dest_x, int dest_y) {
        int diff_x = dest_x - src_x, diff_y = dest_y - src_y;
        int src_offset = Offset(src_x, src_y);
        int dest_offset = Offset(dest_x, dest_y);
        if (diff_x == -1 && diff_y == 0) {
            m_mazes[src_offset] |= CELL_PATH_W;
            m_mazes[dest_offset] |= CELL_PATH_E;
        }
        else if (diff_x == 1 && diff_y == 0) {
            m_mazes[src_offset] |= CELL_PATH_E;
            m_mazes[dest_offset] |= CELL_PATH_W;        
        }    
        else if (diff_x == 0 && diff_y == -1) {
            m_mazes[src_offset] |= CELL_PATH_N;
            m_mazes[dest_offset] |= CELL_PATH_S;
        }
        else if (diff_x == 0 && diff_y == 1) {
            m_mazes[src_offset] |= CELL_PATH_S;
            m_mazes[dest_offset] |= CELL_PATH_N;
        }
        else
            throw "diff error";
    }

    void UpdateMazes() {
        if (stack.empty()) return;
        auto [cur_x, cur_y] = stack.back();
        std::vector<std::pair<int, int>> neighbors = GetNeighbors(cur_x, cur_y);

        if (neighbors.empty()) {
            stack.pop_back();
            return;
        }

        int ind = rand() % (neighbors.size());
        auto [dest_x, dest_y] = neighbors[ind];
        stack.emplace_back(neighbors[ind]);

        m_mazes[Offset(dest_x, dest_y)] |= CELL_VISIT;
        UpdateMazesWall(cur_x, cur_y, dest_x, dest_y);

    }
};


#endif