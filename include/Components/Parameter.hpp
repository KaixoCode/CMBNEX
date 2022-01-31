#pragma once
#include "pch.hpp"
#include "Controller.hpp"
#include "Components/BackgroundEffect.hpp"
#include "Utils/Utils.hpp"

namespace Kaixo
{
    // Parameter ccontrol.
    class Parameter : public CControl, public IDropTarget
    {
    public:
        enum Type { KNOB = 0, SLIDER, NUMBER, BUTTON, GROUP, INTERPOLATE, MULTIGROUP, SMALLSLIDER };

        double modulation = 0; // Current modulation value (auto updated from Processor)

        struct Settings
        {
            int tag = -1; // Parameter tag
            MyEditor* editor = nullptr; // Editor
            IControlListener* listener = nullptr; // Control listener
            CRect size; // Size

            int type = KNOB;     // Type of knob
            bool modable = true; // Display and enable mod behaviour
            bool enabled = true; // Is knob enabled, changes colors

            bool dark = false; // Theme
            String name = "";  // Name to display

            std::vector<String> parts = {}; // Parts of a group/multigroup
            int vertical = false;           // Group vertical
            double padding = 5;             // Padding between buttons

            double min = 0;   // Min value 
            double max = 0;   // Max value
            double reset = 0; // Reset value
            int decimals = 1; // Decimals to display
            String unit = ""; // Unit to display after value

        } settings;

    private:
        CColor main = MainGreen;
        CColor text = MainText;
        CColor back = KnobBack;
        int modDragIndex = -1;
        int modEditIndex = -1;
        int modded = 0;
        bool pressed = false;
        CPoint pwhere;
        String valueString;

    public:

        Parameter(Settings settings = {});
        ~Parameter() override;

        // Get modulation value
        double getModValue() { return constrain(modulation, 0., 1.); }
        int ModIndexPos(CPoint pos) const; // Converts pos to mod index

        // Drag/drop methods
        DragOperation onDragEnter(DragEventData data) override;
        DragOperation onDragMove(DragEventData data) override;
        void onDragLeave(DragEventData data) override;
        bool onDrop(DragEventData data) override;

        // Mouse methods
        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

        void UpdateUnitText(); // Update the unit text

        // Draw methods
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

        CLASS_METHODS(Parameter, CControl)
    };
}