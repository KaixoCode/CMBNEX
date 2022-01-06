#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"

namespace Kaixo
{
    class OptionMenu : public COptionMenu
    {
    public:
        using COptionMenu::COptionMenu;

        bool center = true;

        void draw(CDrawContext* pContext) override
        {
            CMenuItem* item = getEntry(currentIndex);

            drawBack(pContext);
            if (item)
            {
                auto _w = pContext->getStringWidth(item->getTitle());
                pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);
                pContext->setFontColor(getFontColor());
                pContext->setFont(pContext->getFont(), 14, kNormalFace);
                if (center)
                    pContext->drawString(item->getTitle(), { getViewSize().getCenter().x - _w / 2, getViewSize().getCenter().y + 5 });
                else
                    pContext->drawString(item->getTitle(), { getViewSize().left + 2, getViewSize().getCenter().y + 4 });
            }

            setDirty(false);
        }
    };
}