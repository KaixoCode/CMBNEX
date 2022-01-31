#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/BackgroundEffect.hpp"

namespace Kaixo
{
    // XY Panel with 2 Parameters.
    class XYPanel : public CView, public IControlListener
    {
    public:
        Parameter* x;
        Parameter* y;

        bool pressed = false;
        bool enabled = true;

        XYPanel(Parameter* x, Parameter* y, const CRect& rect)
            : CView(rect), x(x), y(y)
        {   // Register as control listener to know when value update, so setDirty
            x->registerControlListener(this);
            y->registerControlListener(this);
        }

        void valueChanged(CControl* pControl) { setDirty(true); }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        void draw(CDrawContext* pContext) override;
    };
}