#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Views/WaveformView.hpp"
#include "Components/XYPanel.hpp"

namespace Kaixo
{    
    // OscillatorToggle, the view that gets selected when you press
    // a OscillatorView.
    class OscillatorToggle : public CViewContainer
    {
    public:
        Label* outl, * shpl, * nsel, * fldl, * drvl, * ftrl, * frzl;
        Parameter* dest, * rphs, * phse, * sync, * plsw, * wtps, * bend, 
            * dcof, * enbs, * shpr, * shpm, * shp2, * sh2m, * shmr, * enbn, 
            * nois, * nscl, * enbf, * flda, * bias, * enbd, * drvg, * drve, 
            * enbr, * fltr, * freq, * reso, * frez;
        XYPanel* panel;

        int index;

        OscillatorToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor);
        ~OscillatorToggle();
    };

    // Oscillator view is the view that's always visible, when pressed
    // it will display its OscillatorToggle view.
    class OscillatorView : public CViewContainer, public IControlListener
    {
    public:
        OscillatorToggle* toggle;
        bool selected = false;

        bool wtloaded = false;

        Parameter* enbl, * tune, * pann, * detn, * volm;
        Label* titl = nullptr; // Oscillator title
        WaveformView* wfrm = nullptr; // Waveform view

        int index = 0;
        bool viewWave = false;
        
        bool modulateChange = false;

        void Select();
        void Unselect();

        void onIdle() override;
        void valueChanged(CControl* pControl) override;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;

        OscillatorView(OscillatorToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor);
        ~OscillatorView();
    };

    // View that contains all the OscillatorView and OscillatorToggle views.
    class OscillatorPart : public CViewContainer
    {
    public:
        OscillatorView* views[4];
        OscillatorToggle* toggles[4];

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;

        OscillatorPart(const CRect& size, IControlListener* listener, MyEditor* editor);
    };
}