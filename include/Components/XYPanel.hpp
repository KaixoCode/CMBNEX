#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/BackgroundEffect.hpp"

namespace Kaixo
{
    class XYPanel : public CView, public IControlListener
    {
    public:
        Parameter* x;
        Parameter* y;

        void valueChanged(CControl* pControl)
        {
            setDirty(true);
        }

        bool pressed = false;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            pressed = true;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
        {
            pressed = false;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
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

        XYPanel(Parameter* x, Parameter* y, const CRect& rect)
            : CView(rect), x(x), y(y)
        {
            x->registerControlListener(this);
            y->registerControlListener(this);
        }

        bool enabled = true;

        BackgroundEffect bg{ getViewSize() };

        void draw(CDrawContext* pContext) override
        {
            auto a = getViewSize();
            bg.draw(pContext);
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
            double _padding = 12;
            double _x = a.left + x->getValue() * (getWidth() - 2 * _padding + 1) + _padding;
            double _y = a.top + (1 - y->getValue()) * (getHeight() - 2 * _padding + 1) + _padding;
            if (enabled) pContext->setFrameColor(MainGreen);
            else pContext->setFrameColor(OffText);
            pContext->setFillColor(MainBack);
            pContext->setLineWidth(3);
            pContext->drawRect({ _x - _padding / 2, _y - _padding / 2, _x + _padding / 2, _y + _padding / 2 }, kDrawFilledAndStroked);

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
    };
}