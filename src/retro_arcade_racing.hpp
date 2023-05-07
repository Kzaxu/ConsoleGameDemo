#ifndef __RETRO_ARCADE_RACING_HPP
#define __RETRO_ARCADE_RACING_HPP

#include "olcConsoleGameEngineSDL.hpp"
#include <cmath>

class RetroArcadeRacing : public olcConsoleGameEngine {

public:
    RetroArcadeRacing() {
        m_sAppName = L"Retro Arcade Racing";
    }

protected:
    bool OnUserCreate() override {
        InitPaths();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);

        UpdateStatus(fElapsedTime);
        DrawBackground();
        DrawCar();

        return true;
    }

    bool OnUserDestroy() override {
        return true;
    }

private:
    float cur_speed = 0.0f;
    float distance = 0.0f;
    float track_curvature = 0.0f;
    float player_curvature = 0.0f;

    std::vector<std::pair<float, float>> tar_track_curvature;
    int track_ind = 0;

    static constexpr float max_speed = 70.0f;
    static constexpr float min_road_width = 0.1f;
    static constexpr float max_road_width = 0.9f;
    static constexpr float rel_clip_width = 0.15f;

    static constexpr float car_rel_y = 0.83f;
    static constexpr float car_perspect = (car_rel_y - 0.5f) / 0.5f;


    void InitPaths() {
        std::vector<std::pair<float, float>> paths {
            {0.f, 200.f},{1.f, 100.f},{0.f, 200.f},
            {-1.f, 100.f}
        };
        float trace_distance = 0.f;
        for (auto p : paths) {
            trace_distance += p.second;
            tar_track_curvature.emplace_back(p.first, trace_distance);
        }
    }

    void UpdateStatus(float fElapsedTime) {
        if (m_keys[VK_DOWN].bHeld) {
            cur_speed -= 25.0f * fElapsedTime;
        } else if (m_keys[VK_UP].bHeld) {
            cur_speed += 5.0f * fElapsedTime;
        } else {
            cur_speed -= 1.0f * fElapsedTime;
        } 
        

        cur_speed = std::min( std::max(0.f, cur_speed), max_speed);

        distance += cur_speed * fElapsedTime;
        while (track_ind < tar_track_curvature.size() - 1 
                && tar_track_curvature[track_ind].second < distance) {
            track_ind++;
        }
        track_curvature += (tar_track_curvature[track_ind].first - track_curvature) * fElapsedTime;

    }

    void DrawCar() {

        // constexpr float perspect = (car_rel_y - 0.5f) / 0.5f;
        float car_rel_x = std::max(0.f, std::min(1.f, 0.5f + player_curvature - track_curvature));
        int car_x = car_rel_x * ScreenWidth();
        int car_y = car_rel_y * ScreenHeight(); 
        
        int direction = 0;
        if (player_curvature > 0)
            direction = 1;
        else if (player_curvature == 0)
            direction = 0;
        else
            direction = -1;

        DrawCar(car_x, car_y, direction);
    }

    void DrawCar(int x, int y, int direction) {
        x -= 7;
        switch (direction)
		{
		case 0:
			DrawStringAlpha(x, y + 0, L"   ||####||   ");
			DrawStringAlpha(x, y + 1, L"      ##      ");
			DrawStringAlpha(x, y + 2, L"     ####     ");
			DrawStringAlpha(x, y + 3, L"     ####     ");
			DrawStringAlpha(x, y + 4, L"|||  ####  |||");
			DrawStringAlpha(x, y + 5, L"|||########|||");
			DrawStringAlpha(x, y + 6, L"|||  ####  |||");
			break;

		case +1:
			DrawStringAlpha(x, y + 0, L"      //####//");
			DrawStringAlpha(x, y + 1, L"         ##   ");
			DrawStringAlpha(x, y + 2, L"       ####   ");
			DrawStringAlpha(x, y + 3, L"      ####    ");
			DrawStringAlpha(x, y + 4, L"///  ####//// ");
			DrawStringAlpha(x, y + 5, L"//#######///O ");
			DrawStringAlpha(x, y + 6, L"/// #### //// ");
			break;

		case -1:
			DrawStringAlpha(x, y + 0, L"\\\\####\\\\      ");
			DrawStringAlpha(x, y + 1, L"   ##         ");
			DrawStringAlpha(x, y + 2, L"   ####       ");
			DrawStringAlpha(x, y + 3, L"    ####      ");
			DrawStringAlpha(x, y + 4, L" \\\\\\\\####  \\\\\\");
			DrawStringAlpha(x, y + 5, L" O\\\\\\#######\\\\");
			DrawStringAlpha(x, y + 6, L" \\\\\\\\ #### \\\\\\");
			break;
		}
    }

    void DrawBackground() {
        
        for (int j = 0; j < ScreenHeight() / 2; ++j) {

            int y = ScreenHeight() / 2 + j;
            float perspect = j * 2.0f/ ScreenHeight();
            int l_mid_len = (0.5f + track_curvature * powf(1 - perspect, 3)) * ScreenWidth();
            int r_mid_len = ScreenWidth() - l_mid_len;
            float road_width = min_road_width + (max_road_width - min_road_width) * perspect;
            float clip_width = rel_clip_width * road_width;

            COLOUR clip_cor = sinf(80.0f *  powf(1.0f - perspect, 2) + distance) > 0.0f ? FG_RED : FG_WHITE;
            COLOUR grass_cor = sinf(20.0f *  powf(1.0f - perspect, 2) + distance) > 0.0f ? FG_GREEN : FG_DARK_GREEN;

            for (int i = 0; i < ScreenWidth(); ++i) {

                if (i <= (1 - road_width - clip_width) * l_mid_len) {
                    Draw(i, y, PIXEL_SOLID, grass_cor);
                } else if (i <= (1 - road_width) * l_mid_len) {
                    Draw(i, y, PIXEL_SOLID, clip_cor);
                } else if ( i <= l_mid_len + road_width * r_mid_len ) {
                    Draw(i, y, PIXEL_SOLID, FG_GREY);
                } else if (i <= l_mid_len + (road_width + clip_width) * r_mid_len) {
                    Draw(i, y, PIXEL_SOLID, clip_cor);
                } else {
                    Draw(i, y, PIXEL_SOLID, grass_cor);
                }
            }
        }
    }
};


void TestRetroArcadeRacing() {
    RetroArcadeRacing game;
    game.ConstructConsole(200, 150, 4, 4);
    game.Start();
}

#endif