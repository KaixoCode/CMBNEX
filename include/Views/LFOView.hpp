#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/BackgroundEffect.hpp"
#include "Components/DragThing.hpp"
#include "Components/SwitchThing.hpp"
#include "Views/WaveformView.hpp"

namespace Kaixo
{
    class LFOView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;

        CPoint pwhere;
        WaveformView* curve;
        Parameter* rate, * amnt, * posi, * offs, * shpr, * sync, * retr;
        Label* lfo1, * lfo2, * lfo3, * lfo4, * lfo5;
        DragThing* nvd1, * nvd2, * nvd3, * nvd4, * nvd5;
        SwitchThing* swtc;

        bool pressed = false;
        bool modulateChange = false;
        bool wtloaded = false;

        void UpdateIndex();
        void onIdle() override;
        void valueChanged(CControl* pControl) override;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        LFOView(const CRect& size, IControlListener* listener, MyEditor* editor);
        ~LFOView();
    };
}