#include "Components/Label.hpp"

namespace Kaixo
{
    void Label::draw(CDrawContext* pContext)
    {
        pContext->setLineWidth(2);
        pContext->setFont(pContext->getFont(), settings.fontsize);
        pContext->setFontColor(settings.enabled ? MainText : OffText);
        pContext->setFrameColor(settings.enabled ? MainText : OffText);
        auto _w = pContext->getStringWidth(settings.value);
        auto _s = getViewSize().getTopCenter();
        _s.x -= _w / 2;
        _s.y += settings.fontsize;

        if (!settings.center)
            _s.x = getViewSize().left;

        pContext->drawString(settings.value, _s, true);
        if (settings.line)
            pContext->drawLine({ _s.x, _s.y + 4 }, { _s.x + _w, _s.y + 4 });
        setDirty(false);
    }
}