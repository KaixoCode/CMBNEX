#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class BackgroundEffect : public CView
    {
    public:
        BackgroundEffect(const CRect& c, bool dark = false)
            : CView(c), dark(dark)
        {}

        bool pressed = false;
        bool dark = false;
        bool button = false;

        int sides = 0;

        double edge = 1;

        static inline void draw(CDrawContext* pContext, CRect size, double edge = 1, bool dark = false, bool button = false, bool pressed = false, int sides = 0, bool enabled = true)
        {
            auto a = size;
            a.inset({ sides != 1 ? edge : 0, sides != 2 ? edge : 0 });

            bool corner = !button;

            constexpr static auto MainGreenC = MainGreen;
            constexpr static auto MainGreenB = CColor{
                (int)std::min(MainGreenC.red * 1.1, 255.),
                (int)std::min(MainGreenC.green * 1.1, 255.),
                (int)std::min(MainGreenC.blue * 1.1, 255.), 255 };

            if (sides != 1) a.left -= edge;
            if (sides != 2) a.top -= edge;
            if (sides != 1) a.right -= corner ? edge : 0;
            if (sides != 2) a.bottom -= corner ? edge : 0;
            pContext->setFillColor(dark && !button ?
                pressed ? button ? enabled ? MainGreenB : OffText : MainBackL : MainBackD :
                pressed ? MainBackD : button ? KnobBackL : MainBackL);
            pContext->drawRect(a, kDrawFilled);

            if (sides != 1) a.left += (corner ? 2 : 1) * edge;
            if (sides != 2) a.top += (corner ? 2 : 1) * edge;
            if (sides != 1) a.right += (corner ? 2 : 1) * edge;
            if (sides != 2) a.bottom += (corner ? 2 : 1) * edge;
            pContext->setFillColor(dark && !button ?
                pressed ? MainBackD : button ? KnobBackL : MainBackL :
                pressed ? button ? enabled ? MainGreenB : OffText : MainBackL : MainBackD);
            pContext->drawRect(a, kDrawFilled);

            if (sides != 1) a.left -= corner ? edge : 0;
            if (sides != 2) a.top -= corner ? edge : 0;
            if (sides != 1) a.right -= corner ? edge : edge;
            if (sides != 2) a.bottom -= corner ? edge : edge;
            pContext->setFillColor(button ?
                pressed ? enabled ? MainGreenC : OffText : KnobBack :
                dark ? DarkBack : MainBack);
            pContext->drawRect(a, kDrawFilled);
        }

        void draw(CDrawContext* pContext)
        {
            draw(pContext, getViewSize(), edge, dark, button, pressed, sides);
        }
    };
}