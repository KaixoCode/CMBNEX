#include "Components/XYPanel.hpp"

namespace Kaixo
{
    CMouseEventResult XYPanel::onMouseDown(CPoint& where, const CButtonState& buttons) 
    {
        pressed = true;
        return kMouseEventHandled;
    }

    CMouseEventResult XYPanel::onMouseUp(CPoint& where, const CButtonState& buttons) 
    {
        pressed = false;
        return kMouseEventHandled;
    }

    CMouseEventResult XYPanel::onMouseMoved(CPoint& where, const CButtonState& buttons) 
    {
        if (pressed)
        {
            auto a = getViewSize();
            double _padding = 12;
            double _x = constrain((where.x - a.left - _padding) / (getWidth() - 2 * _padding), 0., 1.);
            double _y = constrain(1 - (where.y - a.top - _padding) / (getHeight() - 2 * _padding), 0., 1.);
            x->beginEdit();
            x->setValue(_x);
            x->valueChanged();
            x->endEdit();
            y->beginEdit();
            y->setValue(_y);
            y->valueChanged();
            y->endEdit();
            setDirty(true);
            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    void XYPanel::draw(CDrawContext* pContext)
    {
        auto a = getViewSize();
        BackgroundEffect::draw(pContext, { .size = a });
        a.inset({ 1, 1 });
        pContext->setFillColor(MainBack);
        pContext->drawRect(a, kDrawFilled);
        pContext->setLineStyle(CLineStyle{ CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound });

        pContext->setDrawMode(kAntiAliasing);
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);
        constexpr static double nmr = 10;
        for (int i = 0; i < nmr; i++)
        {
            double x = a.left + i * a.getWidth() / nmr;
            double y = a.top + i * a.getHeight() / nmr;
            pContext->drawLine({ x, a.top }, { x, a.bottom - 1 });
            pContext->drawLine({ a.left, y }, { a.right - 1, y });
        }

        pContext->setFrameColor(KnobBack);

        pContext->drawLine({ a.getCenter().x, a.top }, { a.getCenter().x, a.bottom - 1 });
        pContext->drawLine({ a.left, a.getCenter().y }, { a.right - 1, a.getCenter().y });

        pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);
        double _padding = 8;
        double _psize = 12;
        double _x = a.left + x->getValue() * (a.getWidth() - 2 * _padding) + _padding;
        double _y = a.top + (1 - y->getValue()) * (a.getHeight() - 2 * _padding) + _padding;
        if (enabled) pContext->setFrameColor(MainGreen);
        else pContext->setFrameColor(OffText);
        pContext->setFillColor(MainBack);
        pContext->setLineWidth(3);
        pContext->drawRect({ _x - _psize / 2, _y - _psize / 2, _x + _psize / 2, _y + _psize / 2 }, kDrawFilledAndStroked);

        auto _s = getViewSize();
        double _bsize = 8;
        pContext->setLineWidth(1);
        pContext->setFrameColor(OffText);
        pContext->drawLine({ _s.left, _s.top }, { _s.left,  _s.top + _bsize });
        pContext->drawLine({ _s.left, _s.top }, { _s.left + _bsize, _s.top });

        pContext->drawLine({ _s.right - 1, _s.top }, { _s.right - 1,  _s.top + _bsize });
        pContext->drawLine({ _s.right - 1, _s.top }, { _s.right - _bsize - 1, _s.top });

        pContext->drawLine({ _s.right - 1, _s.bottom - 1 }, { _s.right - 1,  _s.bottom - _bsize - 1 });
        pContext->drawLine({ _s.right - 1, _s.bottom - 1 }, { _s.right - _bsize - 1, _s.bottom - 1 });

        pContext->drawLine({ _s.left, _s.bottom - 1 }, { _s.left,  _s.bottom - _bsize - 1 });
        pContext->drawLine({ _s.left, _s.bottom - 1 }, { _s.left + _bsize, _s.bottom - 1 });
    }
}