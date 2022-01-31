#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/BackgroundEffect.hpp"

namespace Kaixo
{
    // Combine toggle, the view that gets selected when you press
    // a CombineView.
    class CombineToggle : public CViewContainer
    {
    public:
        Label
            * ftrl, * fldl, * drvl, * dcol, * outl;

        Parameter
            * enbr, * fltr, * freq, * reso, * addm, * minm, * mulm, 
            * ponm, * maxm, * modm, * andm, * inlm, * orrm, * xorm, 
            * enbf, * fold, * bias, * enbd, * driv, * dgai, * dest, 
            * dcof;

        int index = 0;

        CombineToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor);
        ~CombineToggle();
    };

    // Combine view is the view that's always visible, when pressed
    // it will display its CombineToggle view.
    class CombineView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;
        Parameter* gmix, * gain, * pgai;
        Label* titl, * inp1, * inp2;

        CombineToggle* toggle;

        bool selected = false;

        void Select();
        void Unselect();
        void valueChanged(CControl* pControl) override;

        CombineView(CombineToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor);
        ~CombineView();
    };

    // View that contains all the CombineView and CombineToggle views.
    class CombinePart : public CViewContainer
    {
    public:
        CombineView* views[3];
        CombineToggle* toggles[3];

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;

        CombinePart(const CRect& size, IControlListener* listener, MyEditor* editor);
    };
}