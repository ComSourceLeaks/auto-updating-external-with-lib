#include "helper.hpp"
#include "spoofing.hpp"

void MoveMouse(float targetX, float targetY) {
    SPOOF;
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = targetX;
    input.mi.dy = targetY;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    SendInput(1, &input, sizeof(INPUT));
}

namespace inp {
    class mouse
    {
    public:
        inline void move(fvector2d Head2D) {
            SPOOF;
            static fvector2d CurrentPosition = { 0, 0 }; // Track the current mouse position

            float x = Head2D.x;
            float y = Head2D.y;
            float AimSpeed = globals::g_smooth;
            float offset = 2.2f;

            fvector2d ScreenCenter = { (double)screen_width / 2, (double)screen_height / 2 };
            fvector2d Target;

            if (x != 0)
            {
                if (x > ScreenCenter.x)
                {
                    Target.x = -(ScreenCenter.x - x);
                    Target.x /= AimSpeed;
                    if (Target.x + ScreenCenter.x > ScreenCenter.x * 2) Target.x = 0;
                }

                if (x < ScreenCenter.x)
                {
                    Target.x = x - ScreenCenter.x;
                    Target.x /= AimSpeed;
                    if (Target.x + ScreenCenter.x < 0) Target.x = 0;
                }
            }

            if (y != 0)
            {
                if (y > ScreenCenter.y)
                {
                    Target.y = -(ScreenCenter.y - y);
                    Target.y /= AimSpeed;
                    if (Target.y + ScreenCenter.y > ScreenCenter.y * 2) Target.y = 0;
                }

                if (y < ScreenCenter.y)
                {
                    Target.y = y - ScreenCenter.y;
                    Target.y /= AimSpeed;
                    if (Target.y + ScreenCenter.y < 0) Target.y = 0;
                }
            }


            MoveMouse(Target.x, Target.y);
        }
    };
} static inp::mouse* input = new inp::mouse();
