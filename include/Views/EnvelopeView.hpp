#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/BackgroundEffect.hpp"
#include "Components/DragThing.hpp"
#include "Components/SwitchThing.hpp"
#include "Processing/Modules.hpp"

namespace Kaixo
{
    // Display the envelope curve, uses an actual ADSR instance 
    // to make it as accurate as possible
    class EnvelopeCurve : public CView
    {
    public:
        // Different sections in an envelope.
        enum Section { Attack, AttackCurve, Decay, DecayCurve, Sustain, SustainLine, Release, ReleaseCurve, None } section;
        
        using CView::CView;

        ADSR env; // The envelope
        CColor color = MainGreen; 
        Section editing = None; // Section we're currently editing.
        CPoint pwhere{ 0, 0 };
        double pvaluea;
        double pvalueb;
        bool pressed = false;

        Section getSection(double x, double y); // Get section at position.
        double ValueAt(double p, double step); // Get value at phase

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseEntered(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseExited(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        void draw(CDrawContext* pContext) override;
    };

    class EnvelopeView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;
        bool pressed = false;

        EnvelopeCurve* curve;
        Parameter* attk, * atkc, * attl, * decy, * dcyc, * decl, * sust, * rels, * rlsc;
        DragThing* nvd1, * nvd2, * nvd3, * nvd4, * nvd5;
        Label* time, * slpe, * vlue, * env1, * env2, * env3, * env4, * env5;
        SwitchThing* swtc;

        void valueChanged(CControl* pControl) override;
        void UpdateIndex();
     
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;

        EnvelopeView(const CRect& size, IControlListener* listener, MyEditor* editor);
        ~EnvelopeView();
    };
}