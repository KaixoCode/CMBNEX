#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "ViewFactoryBase.hpp"
#include "myplugincontroller.hpp"
#include "DragThing.hpp"
#include "SwitchThing.hpp"
#include "BackgroundEffect.hpp"

namespace Kaixo
{
    class Knob : public CControl, public IDropTarget
    {
    public:
        enum Type { KNOB = 0, SLIDER, NUMBER, BUTTON, GROUP, INTERPOLATE, MULTIGROUP, SMALLSLIDER };

        std::vector<std::string> parts;
        double modulation = 0;
        bool dark = false;
        double min = 0;
        double max = 0;
        double reset = 0;
        int decimals = 1;
        int type = 1;
        bool modable = true;
        bool enabled = true;
        String name;
        String unit;

    private:
        MyEditor* editor;
        CColor main = MainGreen;
        CColor text = MainText;
        CColor back = KnobBack;
        int modDragIndex = -1;
        int modEditIndex = -1;
        int modded = 0;
        bool pressed = false;
        CPoint pwhere;
        String str;

    public:

        Knob(const CRect& size, MyEditor* editor, bool dark = false);
        ~Knob() override;

        double getModValue() { return constrain(modulation, 0., 1.); }
        int ModIndexPos(CPoint pos) const;

        DragOperation onDragEnter(DragEventData data) override;
        DragOperation onDragMove(DragEventData data) override;
        void onDragLeave(DragEventData data) override;
        bool onDrop(DragEventData data) override;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        void UpdateUnitText();

        void drawKnob(CDrawContext* pContext);
        void drawSlider(CDrawContext* pContext);
        void drawNumber(CDrawContext* pContext);
        void drawButton(CDrawContext* pContext);
        void drawGroup(CDrawContext* pContext);
        void drawInterpolate(CDrawContext* pContext);
        void drawMultigroup(CDrawContext* pContext);
        void drawSmallSlider(CDrawContext* pContext);
        void drawModBoxes(CDrawContext* pContext);
        void draw(CDrawContext* pContext) override;

        CLASS_METHODS(Knob, CControl)
    };
}