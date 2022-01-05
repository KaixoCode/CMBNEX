#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class Label : public CView
    {
    public:
        using CView::CView;

        String value;
        int center = true;
        int fontsize = 20;
        bool line = false;

        CColor color = { 200, 200, 200, 255 };

        void draw(CDrawContext* pContext) override
        {
            pContext->setLineWidth(2);
            pContext->setFont(pContext->getFont(), fontsize);
            pContext->setFontColor(color);
            pContext->setFrameColor(color);
            auto _w = pContext->getStringWidth(value);
            auto _s = getViewSize().getTopCenter();
            _s.x -= _w / 2;
            _s.y += fontsize;

            if (!center)
                _s.x = getViewSize().left;

            pContext->drawString(value, _s, true);
            if (line)
                pContext->drawLine({ _s.x, _s.y + 4 }, { _s.x + _w, _s.y + 4 });
            setDirty(false);
        }
    };
}