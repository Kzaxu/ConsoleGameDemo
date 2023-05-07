#ifndef __SPLINES_VIDEO_HPP
#define __SPLINES_VIDEO_HPP

#include "olcConsoleGameEngineSDL.hpp"

struct Point2D {
    float x;
    float y;
};

struct Spline {

    std::vector<Point2D> points;
    bool isloop = false;

    Point2D& operator[](int x) {
        return points[x];
    }

    int GetPointNum() {
        return points.size();
    }

    Point2D GetSplinePoint(float t) {
        int p0, p1, p2, p3;
        if (!isloop) {
            p1 = t + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        } else {
            p1 = (int)(t + 1) % points.size();
            p2 = (p1 + 1) % points.size();
            p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
        }

        t = t - (int)t;
        
        float tt = t * t;
		float ttt = tt * t;

		float q1 = -ttt + 2.0f*tt - t;
		float q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
		float q3 = -3.0f*ttt + 4.0f*tt + t;
		float q4 = ttt - tt;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
    }

    Point2D GetSplineGradient(float t) {
        int p0, p1, p2, p3;
        if (!isloop) {
            p1 = t + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        } else {
            p1 = (int)(t + 1) % points.size();
            p2 = (p1 + 1) % points.size();
            p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
        }

        t = t - (int)t;
        
        float tt = t * t;

		float q1 = -3*tt + 4.0f*t - 1;
		float q2 = 9.0f*tt - 10.0f*t;
		float q3 = -9.0f*tt + 8.0f*t + 1;
		float q4 = 3*tt - 2*t;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);
        float r = sqrtf(tx * tx + ty * ty);
		return{ tx / r, ty / r};        
    }
};


class SplineVideo : public olcConsoleGameEngine {

public:
    SplineVideo() {
        m_sAppName = L"Spline";
    }

protected:
    bool OnUserCreate() override {
        float y = ScreenHeight() / 2.f;
        float x = ScreenWidth() / 6.f;
        spline.points = std::vector<Point2D> {
            {x, y}, {2*x, y}, {3*x,y}, 
            {4*x,y}, {5*x, y}
        };
        spline.isloop = true;
        max_cursor = spline.isloop ? spline.GetPointNum() : spline.GetPointNum() - 3;
        return true;
    }

    bool OnUserDestroy() override {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        UpdateStatus(fElapsedTime);
        DrawSplinePoints();
        DrawSplineLine();
        DrawCursorLine();
        return true;
    }

private:
    int point_ind = 0;
    float cursor = 0.f;
    float max_cursor = 0.f;
    Spline spline;

    constexpr static float move_speed = 10.f;
    constexpr static float line_step = 0.05f;
    constexpr static float cursor_speed = 1.5f;

    void UpdateStatus(float fElapsedTime) {
        if (m_keys[L'Z'].bReleased) {
            point_ind--;
        } else if (m_keys[L'X'].bReleased) {
            point_ind++;
        }
        point_ind = (point_ind + spline.GetPointNum()) % spline.GetPointNum();
        
        if (m_keys[L'A'].bHeld) 
            cursor -= cursor_speed * fElapsedTime;
        if (m_keys[L'S'].bHeld) 
            cursor += cursor_speed * fElapsedTime;
        if (cursor < 0) {
            cursor += max_cursor;
        } else if (cursor > max_cursor) {
            cursor -= max_cursor;
        }

        if (m_keys[VK_UP].bHeld) 
            spline[point_ind].y -= move_speed * fElapsedTime;
        if (m_keys[VK_DOWN].bHeld) 
            spline[point_ind].y += move_speed * fElapsedTime;
        if (m_keys[VK_LEFT].bHeld) 
            spline[point_ind].x -= move_speed * fElapsedTime;
        if (m_keys[VK_RIGHT].bHeld) 
            spline[point_ind].x += move_speed * fElapsedTime;

        spline[point_ind].x = std::min(ScreenWidth() - 2.f, std::max(1.f, spline[point_ind].x));
        spline[point_ind].y = std::min(ScreenHeight() - 2.f, std::max(1.f, spline[point_ind].y));

    }

    void DrawSplinePoints() {
        for (int i = 0; i < spline.GetPointNum(); ++i) {
            int x = spline[i].x, y = spline[i].y;
            COLOUR col = i == point_ind ? FG_YELLOW : FG_RED;
            Fill(x - 1, y - 1, x + 2, y + 2, PIXEL_SOLID, col);
            Draw(x, y, L'#', FG_WHITE);
        }
    }

    void DrawSplineLine() {
        for (float t = 0.f; t <= max_cursor; t += line_step) {
            Point2D p = spline.GetSplinePoint(t);
            Draw(p.x, p.y);
        }
    }

    void DrawCursorLine() {
        Point2D cursor_point = spline.GetSplinePoint(cursor);
        Point2D cursor_gradient = spline.GetSplineGradient(cursor);

        int st_x = cursor_point.x + 5.f * cursor_gradient.y;
        int st_y = cursor_point.y - 5.f * cursor_gradient.x;
        int ed_x = cursor_point.x - 5.f * cursor_gradient.y;
        int ed_y = cursor_point.y + 5.f * cursor_gradient.x; 

        DrawLine(st_x, st_y, ed_x, ed_y, PIXEL_SOLID, FG_BLUE);       
    }
};

void TestSplineVideo() {
    SplineVideo game;
    game.ConstructConsole(100, 75, 8, 8);
    game.Start();
}

#endif