#pragma once
#include "pch.hpp"

namespace Kaixo
{
    // 3d background effect by adding dark and light border.
    class BackgroundEffect : public CView
    {
    public:
        struct Settings
        {
            CRect size; // Initial size, don't edit here, no effect!
            bool pressed = false; // Is pressed, switches light/dark edges, if button: display color.
            bool hovering = false; // Is hovering, switches light/dark edges, if button: display color.
            bool dark = false;   // Is dark theme?
            bool button = false; // Is for button? Displays color when pressed = true
            bool enabled = true; // enabled for button only, displays gray instead of color when false.
            int sides = 0;   // 0 = all sides, 1 = only top/bottom, 2 = only left/right
            double edge = 1; // How wide is 3d effect border
        } settings;

        BackgroundEffect(Settings settings)
            : CView(settings.size), settings(settings)
        {}

        static void draw(CDrawContext* pContext, Settings settings);
        void draw(CDrawContext* pContext) override;
    };
}