#pragma once
#include "pch.hpp"

namespace Kaixo
{
    // Simple label, displays text.
    class Label : public CView
    {
    public:
        struct Settings
        {
            CRect size; // Initial size, don't edit, no effect!
            String value = ""; // Text
            int center = true; // Align to center? Otherwise left.
            int fontsize = 20; // Font size
            bool line = false; // Display line below
            bool enabled = true; // Enabled, if false, OffText color, otherwise MainText
        } settings;

        Label(Settings settings)
            : CView(settings.size), settings(settings)
        {}

        void draw(CDrawContext* pContext) override;
    };
}