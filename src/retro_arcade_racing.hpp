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
        DrawStatus();
        
        return true;
    }

    bool OnUserDestroy() override {
        return true;
    }

private:
    float cur_speed = 0.0f;
    float distance = 0.0f;
    float curvature = 0.f;
    float track_curvature = 0.0f;
    float player_curvature = 0.0f;
    float car_x = 0.f;
    int car_direction = 0;

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

        UpdateCarStatus(fElapsedTime);
        if (m_keys[VK_DOWN].bHeld) {
            cur_speed -= 5.0f * fElapsedTime;
        } else if (m_keys[VK_UP].bHeld) {
            cur_speed += 2.0f * fElapsedTime;
        } else {
            cur_speed -= 1.0f * fElapsedTime;
        }

        cur_speed = std::min( std::max(0.f, cur_speed), 1.0f);

        distance += cur_speed * fElapsedTime * max_speed;
        while (track_ind < tar_track_curvature.size() - 1 
                && tar_track_curvature[track_ind].second < distance) {
            track_ind++;
        }
        if (track_ind == tar_track_curvature.size() - 1
            && distance >= tar_track_curvature[track_ind].second) {
            track_ind = 0;
            distance = 0;
        }
        curvature += (tar_track_curvature[track_ind].first - curvature) * fElapsedTime * cur_speed;
        track_curvature += curvature * cur_speed * fElapsedTime;

    }

    void UpdateCarStatus(float fElapsedTime) {
        car_direction = 0;

        if (m_keys[VK_RIGHT].bHeld) {
            player_curvature += 0.7f * fElapsedTime * (1 - cur_speed / 2);
            car_direction = 1;
        } else if (m_keys[VK_LEFT].bHeld) {
            player_curvature -= 0.7f * fElapsedTime * (1 - cur_speed / 2);
            car_direction = -1;
        }
        
        constexpr float road_width = min_road_width + (max_road_width - min_road_width) * car_perspect;
        constexpr float clip_width = rel_clip_width * road_width;
        car_x = std::max(0.f, std::min(1.f, 0.5f + player_curvature - track_curvature))
                * ScreenWidth();

        int l_mid_len = (0.5f + curvature * powf(1 - car_perspect, 3)) * ScreenWidth();
        int r_mid_len = ScreenWidth() - l_mid_len;

        if (car_x < (1 - road_width - clip_width) * l_mid_len
            || car_x > l_mid_len + (road_width + clip_width) * r_mid_len) {
            cur_speed -= 5.f * fElapsedTime;
        }
    }

    void DrawCar() {

        // constexpr float perspect = (car_rel_y - 0.5f) / 0.5f;
        int car_y = car_rel_y * ScreenHeight(); 

        DrawCar(car_x, car_y, car_direction);
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

    void DrawStatus() {
        DrawStringAlpha(0, 0, L"Speed: " + std::to_wstring(cur_speed));
        DrawStringAlpha(0, 1, L"Curvature: " + std::to_wstring(curvature));
        DrawStringAlpha(0, 2, L"Player Curvature: " + std::to_wstring(player_curvature));
        DrawStringAlpha(0, 3, L"Track Curvature: " + std::to_wstring(track_curvature));
    }

    void DrawBackground() {
        
        for (int j = 0; j < ScreenHeight() / 2; ++j) {

            int y = ScreenHeight() / 2 + j;
            float perspect = j * 2.0f/ ScreenHeight();
            int l_mid_len = (0.5f + curvature * powf(1 - perspect, 3)) * ScreenWidth();
            int r_mid_len = ScreenWidth() - l_mid_len;
            float road_width = min_road_width + (max_road_width - min_road_width) * perspect;
            float clip_width = rel_clip_width * road_width;

            COLOUR clip_col = sinf(80.0f *  powf(1.0f - perspect, 2) + distance) > 0.0f ? FG_RED : FG_WHITE;
            COLOUR grass_col = sinf(20.0f *  powf(1.0f - perspect, 2) + distance) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
            COLOUR road_col = track_ind == 0 ? FG_WHITE : FG_GREY;

            for (int i = 0; i < ScreenWidth(); ++i) {

                if (i <= (1 - road_width - clip_width) * l_mid_len) {
                    Draw(i, y, PIXEL_SOLID, grass_col);
                } else if (i <= (1 - road_width) * l_mid_len) {
                    Draw(i, y, PIXEL_SOLID, clip_col);
                } else if ( i <= l_mid_len + road_width * r_mid_len ) {
                    Draw(i, y, PIXEL_SOLID, road_col);
                } else if (i <= l_mid_len + (road_width + clip_width) * r_mid_len) {
                    Draw(i, y, PIXEL_SOLID, clip_col);
                } else {
                    Draw(i, y, PIXEL_SOLID, grass_col);
                }
            }
        }

        for (int i = 0; i < ScreenWidth(); ++i) {
            int hill_height = fabs(sinf(0.01f * i + track_curvature)) * ScreenHeight() / 4;
            for (int j = 0; j < ScreenHeight() / 2; ++j) {
                
                if (j < ScreenHeight() / 4) {
                    Draw(i, j, PIXEL_SOLID, FG_DARK_BLUE);
                } else if (j < ScreenHeight() / 2 - hill_height ) {
                    Draw(i, j, PIXEL_SOLID, FG_BLUE);
                } else {
                    Draw(i, j, PIXEL_SOLID, FG_DARK_YELLOW);
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