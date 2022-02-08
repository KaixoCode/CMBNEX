#include "Components/BackgroundEffect.hpp"

namespace Kaixo
{
    void BackgroundEffect::draw(CDrawContext* pContext, Settings settings)
    {
        auto a = settings.size;
        a.inset({ settings.sides != 1 ? settings.edge : 0, settings.sides != 2 ? settings.edge : 0 });

        bool corner = !settings.button;
        corner = false;

        auto MainGreenC = Colors::MainGreen;
        auto MainGreenB = CColor{
            (uint8_t)std::min(MainGreenC.red * 1.1, 255.),
            (uint8_t)std::min(MainGreenC.green * 1.1, 255.),
            (uint8_t)std::min(MainGreenC.blue * 1.1, 255.), 255 };

        if (settings.sides != 1) a.left -= settings.edge;
        if (settings.sides != 2) a.top -= settings.edge;
        if (settings.sides != 1) a.right -= corner ? settings.edge : 0;
        if (settings.sides != 2) a.bottom -= corner ? settings.edge : 0;
        pContext->setFillColor(settings.dark && !settings.button ?
            settings.pressed ? settings.button ? settings.enabled ? MainGreenB : Colors::OffText : Colors::MainBackL : Colors::MainBackD :
            settings.pressed ? Colors::MainBackD : settings.button ? Colors::KnobBackL : Colors::MainBackL);
        pContext->drawRect(a, kDrawFilled);

        if (settings.sides != 1) a.left += (corner ? 2 : 1) * settings.edge;
        if (settings.sides != 2) a.top += (corner ? 2 : 1) * settings.edge;
        if (settings.sides != 1) a.right += (corner ? 2 : 1) * settings.edge;
        if (settings.sides != 2) a.bottom += (corner ? 2 : 1) * settings.edge;
        pContext->setFillColor(settings.dark && !settings.button ?
            settings.pressed ? Colors::MainBackD : settings.button ? Colors::KnobBackL : Colors::MainBackL :
            settings.pressed ? settings.button ? settings.enabled ? MainGreenB : Colors::OffText : Colors::MainBackL : Colors::MainBackD);
        pContext->drawRect(a, kDrawFilled);

        if (settings.sides != 1) a.left -= corner ? settings.edge : 0;
        if (settings.sides != 2) a.top -= corner ? settings.edge : 0;
        if (settings.sides != 1) a.right -= corner ? settings.edge : settings.edge;
        if (settings.sides != 2) a.bottom -= corner ? settings.edge : settings.edge;
        pContext->setFillColor(settings.button ?
            settings.pressed ? settings.enabled ? 
                                settings.hovering ? MainGreenB : MainGreenC :
                                settings.hovering ? Colors::OffTextL : Colors::OffText :
                                settings.hovering ? Colors::KnobBackL : Colors::KnobBack :
            settings.dark ? Colors::DarkBack : Colors::MainBack);
        pContext->drawRect(a, kDrawFilled);
    }

    void BackgroundEffect::draw(CDrawContext* pContext)
    {
        draw(pContext, settings);
    }
}