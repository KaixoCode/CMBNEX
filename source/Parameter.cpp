#include "Components/Parameter.hpp"

namespace Kaixo
{

    Parameter::Parameter(Settings settings)
        : CControl(settings.size), settings(settings)
    {
        if (settings.editor) setListener(settings.editor->getControlListener(""));
        if (settings.tag >= 0 && settings.tag < Params::Size) setTag(settings.tag);
        if (settings.listener) registerControlListener(settings.listener);
        
        setDropTarget(this);
    }

    Parameter::~Parameter()
    {   // If a parameter gets destroyed we know the entire frame 
        // is closed, so we clear all wakeup calls. This is done here
        // because this is the only place we can do this in time.
        if (settings.editor->controller->wakeupCalls.empty()) return;
        std::lock_guard _(settings.editor->controller->lock);
        settings.editor->controller->wakeupCalls.clear();
    }

    int Parameter::ModIndexPos(CPoint pos) const
    {
        if (!settings.modable) return -1; // If not modable, always -1

        auto a = getViewSize();
        switch (settings.type) {
        case KNOB: // Knob has mod boxes centered at bottom
            if (pos.y > a.bottom - 16 && pos.y < a.bottom)
            {   // Calculate possible index
                int _index = std::floor((pos.x - (a.getCenter().x - 13 * ModAmt * 0.5)) / 13.);
                if (_index >= 0 && _index < ModAmt) return _index; // If in range, return
            }
            return -1; // Otherwise return -1
        case NUMBER:      // These types have mod boxes
        case INTERPOLATE: // aligned to the left, but they get offset
        case SMALLSLIDER: // along with mod amount displays on the left.
            if (pos.y > a.bottom - 16 && pos.y < a.bottom)
            {
                int modded = 0; // Count amount of modded sources
                for (int i = 0; i < ModAmt; i++) if (settings.editor->modSource(getTag(), i) != ModSources::None) modded++;

                // Calculate possible index
                int _index = std::floor((pos.x - (a.left + 3 * modded)) / 13.);
                if (_index >= 0 && _index < ModAmt) return _index; // If in range, return
            }
            return -1; // Otherwise -1
        default: return -1; // Any other type always -1
        }
    }

    DragOperation Parameter::onDragEnter(DragEventData data)
    {   // Determine the mod box we're dragging over
        modDragIndex = ModIndexPos(data.pos); 
        setDirty(true); 
        return modDragIndex == -1 ? DragOperation::None : DragOperation::Copy;
    };

    DragOperation Parameter::onDragMove(DragEventData data)
    {   // Determine the mod box we're dragging over
        modDragIndex = ModIndexPos(data.pos);
        setDirty(true);
        return modDragIndex == -1 ? DragOperation::None : DragOperation::Copy;
    };

    void Parameter::onDragLeave(DragEventData data)
    {   // On leave, reset to -1
        modDragIndex = -1;
        setDirty(true);
    };

    bool Parameter::onDrop(DragEventData data)
    {
        int* _data; // Try to retrieve the mod source from the data
        IDataPackage::Type type;
        data.drag->getData(0, (const void*&)_data, type);

        // If the data type is binary, the data should be correct.
        if (type == IDataPackage::Type::kBinary)
        {   // Cast to the mod source enum
            ModSources _value = (ModSources)_data[0];
            settings.editor->modSource(getTag(), modDragIndex, _value); // Set the source
            settings.editor->modAmount(getTag(), modDragIndex, 0.5); // And reset its amount to the center
            setDirty(true); 
            modDragIndex = -1; // We're done with dragging, so reset to -1
            return true;
        }

        return false;
    };

    CMouseEventResult Parameter::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        modEditIndex = ModIndexPos(where); // Determine if hovering over a mod box.

        if (getViewSize().pointInside(where))
        {
            switch (settings.type)
            {
            case KNOB:        // All these types have dragging behaviour
            case SLIDER:      // plus double clicking resets to the default
            case NUMBER:      // value.
            case INTERPOLATE: 
            case SMALLSLIDER: 
                // If we're double clicking on any of these knob types
                if (buttons.isDoubleClick())
                {   // And we're not editing a modulation
                    if (modEditIndex == -1)
                    {   // Reset to the default value.
                        beginEdit(); // Some units have unique reset, seconds is to power 1/3, Hz to power 1/2 (sqrt)
                        if (settings.unit == " s") setValue(std::pow((settings.reset - settings.min) / std::max(settings.max - settings.min, 0.0001), 1. / 3.));
                        else if (settings.unit == " Hz") setValue(std::sqrt((settings.reset - settings.min) / std::max(settings.max - settings.min, 0.0001)));
                        else setValue((settings.reset - settings.min) / std::max(settings.max - settings.min, 0.0001)); // Otherwise normal normalized value.
                        valueChanged();
                        endEdit();
                        setDirty(true);
                    }
                    else
                    {
                        // If we're editing a modulation, reset its value to 0.5
                        settings.editor->modAmount(getTag(), modEditIndex, 0.5);
                        setDirty(true);
                    }
                }
                else
                {
                    // If normal click, and we're holding control, and editing a modulation
                    if (buttons.isControlSet() && modEditIndex != -1)
                    {   // We remove the modulation.
                        settings.editor->modSource(getTag(), modEditIndex, ModSources::None);
                        setDirty(true);
                    }
                }
                break;
            case BUTTON: // Button is simple toggle
                beginEdit(); // Edit the value depending on current value.
                setValue(getValue() > 0.5 ? 0 : 1);
                valueChanged();
                endEdit();
                setDirty(true);
                break;
            case GROUP: // Group is similar to button, except we need to determine which button in group.
                if (settings.parts.size() <= 1) break;
                beginEdit();
                setValue(settings.vertical
                    ? (std::floor(settings.parts.size() * (where.y - getViewSize().top) / getViewSize().getHeight()) / (settings.parts.size() - 1))
                    : (std::floor(settings.parts.size() * (where.x - getViewSize().left) / getViewSize().getWidth()) / (settings.parts.size() - 1)));
                valueChanged();
                endEdit();
                setDirty(true);
                break;
            case MULTIGROUP: // Multigroup uses powers of 2 to store bits of data in a double.
                beginEdit(); // First retrieve the currently edited index
                size_t value = settings.vertical ? std::floor(settings.parts.size() * (where.y - getViewSize().top) / getViewSize().getHeight())
                    : std::floor(settings.parts.size() * (where.x - getViewSize().left) / getViewSize().getWidth());

                size_t current = getValue() * std::pow(2, settings.parts.size()); // Get the current bits set
                current ^= 1ULL << value; // Toggle the current index

                setValue(current / std::pow(2, settings.parts.size())); // Set the new value
                valueChanged();
                endEdit();
                setDirty(true);
                break;
            }

            if (buttons.isLeftButton()) // If left button, it means we can do dragging
                pressed = true, pwhere = where; // So keep track that we've pressed, and where

            return kMouseEventHandled;
        }

        return kMouseEventNotHandled;
    }

    CMouseEventResult Parameter::onMouseUp(CPoint& where, const CButtonState& buttons)
    {   // Reset pressed
        pressed = false;
        return kMouseEventHandled;
    }

    CMouseEventResult Parameter::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {   // Only handle mouse moved when pressed, and thus dragging
        if (pressed)
        {
            switch (settings.type)
            {
            case KNOB:        // All these knob types
            case SLIDER:      // have drag behaviour
            case NUMBER:      // 
            case INTERPOLATE: // 
            case SMALLSLIDER: // 
            {   // Get multiplier depending on if shift is set.
                double mult = buttons.isShiftSet() ? 0.25 : 1;
                double diff = (pwhere.y - where.y) * 0.005; // Calculate the difference using mouse coords.
                if (modEditIndex == -1) switch (settings.type) 
                { // These types have special cases for mouse movement
                case SLIDER:      // Sliders are horizontal
                case SMALLSLIDER: 
                    diff = (where.x - pwhere.x) / getViewSize().getWidth(); break;
                case INTERPOLATE: // Interpolate is horizontal, but slides slower.
                    diff = (where.x - pwhere.x) / (settings.parts.size() * getViewSize().getWidth()); break;
                }

                pwhere = where; // Set new pwhere

                if (modEditIndex == -1)
                {   // If not modulation dragging, edit value with calculated diff
                    beginEdit();
                    setValue(getValue() + mult * diff);
                    valueChanged();
                    endEdit();
                    setDirty(true);
                }
                else
                {   // Otherwise edit the mod amount of the mod edit index.
                    double value = settings.editor->modAmount(getTag(), modEditIndex);
                    settings.editor->modAmount(getTag(), modEditIndex, value + mult * diff);
                    setDirty(true);
                }
                return kMouseEventHandled;
            }
            }
        }
        return kMouseEventNotHandled;
    }

    void Parameter::UpdateUnitText()
    {
        double _v = getValue();
        double val = _v * (settings.max - settings.min) + settings.min;
        if (settings.unit == " dB") // Special case for dB: convert linear multiplier to logarithmic decibel number
        {
            val = lin2db(val);
            std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val < -10 ? -1 : 0)) + "f}", val);
            valueString = _format.c_str() + settings.unit;
        }
        else if (settings.unit == " Hz") // Special case for Hz: convert to kHz when > 1000
        {
            val = std::pow(_v, 2) * (settings.max - settings.min) + settings.min;
            if (val > 1000)
            {
                val /= 1000;
                std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val > 10 ? 0 : 1)) + "f}", val);
                valueString = _format.c_str() + String{ " kHz" };
            }
            else
            {
                std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val > 10 ? 0 : 1)) + "f}", val);
                valueString = _format.c_str() + settings.unit;
            }
        }
        else if (settings.unit == " s") // Special case for seconds: convert to ms when < 1000
        {
            val = std::pow(_v, 3) * (settings.max - settings.min) + settings.min;
            if (val > 1000)
            {
                val /= 1000;
                std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val > 10 ? 0 : 1)) + "f}", val);
                valueString = _format.c_str() + String{ " s" };
            }
            else
            {
                std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val > 10 ? val > 100 ? -1 : 0 : 1)) + "f}", val);
                valueString = _format.c_str() + String{ " ms" };
            }
        }
        else if (settings.unit == "time") // Special case for time: display time string: 1/4, 1/8 etc.
        {
            size_t _type = std::floor(_v * (TimesAmount - 1));
            valueString = TimesString[_type];
        }
        else if (settings.unit == "pan") // Special case for pan: display L/R with number
        {
            int _iv = _v * 100 - 50;
            std::string _nmr = std::to_string(std::abs(_iv));
            if (_iv == 0) valueString = "C";
            else if (_iv < 0) valueString = _nmr.c_str() + String{ "L" };
            else if (_iv > 0) valueString = _nmr.c_str() + String{ "R" };
        }
        else if (settings.unit == " %") // Special case for percentage: round to 1 less decimals when > 100
        {
            if (settings.decimals == 0 && std::round(val) == 0) val = 0;
            std::string _format = std::format("{:." + std::to_string((int)settings.decimals + (val >= 100 || val <= -100 ? -1 : 0)) + "f}", val);
            valueString = _format.c_str() + settings.unit;
        }
        else
        {
            if (settings.decimals == 0 && std::round(val) == 0) val = 0;
            std::string _format = std::format("{:." + std::to_string((int)settings.decimals) + "f}", val);
            valueString = _format.c_str() + settings.unit;
        }
    }

    void Parameter::drawKnob(CDrawContext* pContext)
    {   // Prepare size
        auto a = getViewSize();
        if (settings.modable) a.bottom -= 10;
        a.inset({ 10, 22 });

        // Prepare string widths, to align to center
        auto w1 = pContext->getStringWidth(valueString);
        auto w2 = pContext->getStringWidth(settings.name);

        // Get the angles to draw the semi-circles
        auto v = getValueNormalized() * 270;
        auto start = 135;
        auto end = 135 + v;
        if (settings.min == -settings.max) 
        {   // If min == -max, it's double sided, so 0 is middle top instead of bottom left.
            v -= 135;
            start = v < 0 ? 270 + v : 270;
            end = v < 0 ? 270 : 270 + v;
        }

        // Draw the value and name
        pContext->setFontColor(text);
        pContext->drawString(valueString, { a.getCenter().x - w1 / 2, a.getBottomCenter().y + 12 }, true);
        pContext->drawString(settings.name, { a.getCenter().x - w2 / 2, a.getTopCenter().y - 8 }, true);
        pContext->setLineWidth(2);

        if (settings.modable)
        {   // Add the modulation semi-circles
            a.inset({ -2, -2 });
            v = getValueNormalized() * 270; // Center is always at value
            for (int i = 0; i < ModAmt; i++)
            {   // Check if source != none, display nothing if none
                auto source = settings.editor->modSource(getTag(), i);
                if (source == ModSources::None) continue;

                // Gray background circle
                pContext->setFrameColor(Border);
                pContext->drawArc(a, 135, 135 + 270, kDrawStroked);

                // Two sided modulation if LFO
                bool two = source >= ModSources::LFO1 && source <= ModSources::LFO5; 
                pContext->setFrameColor(main);

                // Calculate start and end angle of modulation, and draw the arc
                double amount = settings.editor->modAmount(getTag(), i) * 2 - 1;
                double start = std::max(135 + v + ((amount < 0) ? amount * 270 : 0), 135.);
                double end = std::min(135 + v + ((amount > 0) ? amount * 270 : 0), 135 + 270.);
                pContext->drawArc(a, start, end, kDrawStroked);

                if (two)
                {   // Draw the other side if two sided
                    double start = std::max(135 + v - ((amount > 0) ? amount * 270 : 0), 135.);
                    double end = std::min(135 + v - ((amount < 0) ? amount * 270 : 0), 135 + 270.);
                    pContext->setFrameColor(OffText);
                    pContext->drawArc(a, start, end, kDrawStroked);
                }

                a.inset({ 3, 3 }); // Each arc is inset 3
            }

            a.inset({ 2, 2 }); // Final arc is wider so inset more
        }

        // Draw value arc
        pContext->setLineWidth(6);
        pContext->setFrameColor(back);
        pContext->drawArc(a, 135, 135 + 270, kDrawStroked);
        pContext->setFrameColor(main);
        pContext->drawArc(a, start, end, kDrawStroked);
    }

    void Parameter::drawSlider(CDrawContext* pContext)
    {
        auto a = getViewSize();
        auto v = getValueNormalized() * (getWidth() - 1);
        auto w1 = pContext->getStringWidth(valueString);

        pContext->setLineWidth(1);
        pContext->setFillColor(back);
        a.top += 20;
        a.left += 1;
        pContext->drawRect(a, kDrawFilled);
        //a.left -= 1;
        //a.top -= 1;
        //a.inset({ 2, 2 });

        pContext->setFillColor(main);
        if (settings.min == -settings.max)
        {
            v -= 0.5 * (getWidth() - 4);
            if (v < 0)
            {
                a.right -= 0.5 * (getWidth() - 4);
                a.left = a.right + v;
            }
            else
            {
                a.left += 0.5 * (getWidth() - 4);
                a.right = a.left + v;
            }
            pContext->drawRect(a, kDrawFilled);
        }
        else
        {
            a.right = a.left + v;
            pContext->drawRect(a, kDrawFilled);
        }

        pContext->setFontColor(text);
        a = getViewSize();
        a.top += 20;
        a.inset({ 2, 2 });
        pContext->drawString(valueString, { a.getCenter().x - w1 / 2, a.getCenter().y + 6 }, true);
        pContext->drawString(settings.name, { a.left, a.getTopCenter().y - 7 }, true);
    }

    void Parameter::drawNumber(CDrawContext* pContext)
    {
        auto a = getViewSize();
        a.left += 1;
        if (settings.modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = settings.editor->modSource(getTag(), i);
                if (source == ModSources::None)
                    continue;
                modded++;
                pContext->setLineWidth(2);
                pContext->setFrameColor(Border);
                pContext->drawLine({ a.left, a.bottom }, { a.left, a.top });

                bool two = false;
                if (source >= ModSources::Env1)
                    pContext->setFrameColor(main);
                else if (source >= ModSources::LFO1)
                    pContext->setFrameColor(main), two = true;

                double amount = settings.editor->modAmount(getTag(), i) * 2 - 1;
                double start = std::max(v + ((amount < 0) ? amount * _w : 0), 0.);
                double end = std::min(v + ((amount > 0) ? amount * _w : 0), _w);

                pContext->setLineWidth(2);
                pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });

                if (two)
                {
                    double start = std::max(v - ((amount > 0) ? amount * _w : 0), 0.);
                    double end = std::min(v - ((amount < 0) ? amount * _w : 0), _w);
                    pContext->setFrameColor(OffText);
                    pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });
                }

                a.left += 3;
            }
        }

        a.bottom -= 10;
        a.top += 20;
        a.inset({ 2, 2 });

        pContext->setFontColor(main);
        pContext->drawString(valueString, { a.left, a.getCenter().y + 1 }, true);
        pContext->setFontColor(text);
        pContext->drawString(settings.name, { a.left, a.getTopCenter().y - 8 }, true);
    }

    void Parameter::drawButton(CDrawContext* pContext)
    {
        auto a = getViewSize();
        auto w2 = pContext->getStringWidth(settings.name);
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);
        pContext->setFillColor(getValue() > 0.5 ? main : back);
        a.top += 1;
        a.left += 1;
        bool pressed = getValue() > 0.5;
        BackgroundEffect::draw(pContext, a, 0, settings.dark, true, pressed, 0, settings.enabled);
        pContext->setFontColor(getValue() > 0.5 ? text : OffText);
        pContext->drawString(settings.name, { a.getCenter().x - w2 / 2, a.getCenter().y + 6 }, true);
    }

    void Parameter::drawGroup(CDrawContext* pContext)
    {
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);

        auto a = getViewSize();
        a.top++;
        a.left++;

        if (settings.vertical)
        {
            auto _y = a.top;
            auto _h = std::ceil(a.getHeight() / settings.parts.size());
            for (int i = 0; i < settings.parts.size(); i++)
            {
                String& _n = settings.parts[i];
                auto _x = a.getCenter().x;

                auto _w = std::floor(pContext->getStringWidth(_n));

                bool pressed = std::abs(getValue() * (settings.parts.size() - 1) - i) < 0.5;

                pContext->setFillColor(pressed ? main : back);
                pContext->setFontColor(pressed ? text : OffText);

                BackgroundEffect::draw(pContext, { a.left, _y, a.right, _y + _h - settings.padding - 1 }, 0, settings.dark, true, pressed, 0, settings.enabled);
                pContext->drawString(_n, { _x - _w / 2, _y + 6 + (_h - settings.padding - 1) / 2 }, true);

                _y += _h;
            }
        }
        else
        {
            auto _x = a.left;
            auto _w = std::ceil(a.getWidth() / settings.parts.size());
            for (int i = 0; i < settings.parts.size(); i++)
            {
                String& _n = settings.parts[i];
                auto _y = a.getCenter().y;

                auto _sw = std::floor(pContext->getStringWidth(_n));

                bool pressed = std::abs(getValue() * (settings.parts.size() - 1) - i) < 0.5;

                pContext->setFillColor(pressed ? main : back);
                pContext->setFontColor(pressed ? text : OffText);
                BackgroundEffect::draw(pContext, { _x, a.top, _x + _w - settings.padding - 1, a.bottom }, 0, settings.dark, true, pressed, 0, settings.enabled);
                pContext->drawString(_n, { _x + (_w - settings.padding - 2) / 2 - _sw / 2, _y + 6 }, true);

                _x += _w;
            }
        }
    }

    void Parameter::drawInterpolate(CDrawContext* pContext)
    {
        size_t v = std::floor(getValue() * (settings.parts.size() - 1) + 0.5);
        double d = (getValue() * (settings.parts.size() - 1) + 0.5) - v;
        String& _n = settings.parts[v];

        auto a = getViewSize();
        a.left += 1;

        if (settings.modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = settings.editor->modSource(getTag(), i);
                if (source == ModSources::None)
                    continue;
                modded++;
                pContext->setLineWidth(2);
                pContext->setFrameColor(Border);
                pContext->drawLine({ a.left, a.bottom }, { a.left, a.top });

                bool two = false;
                if (source >= ModSources::Env1)
                    pContext->setFrameColor(main);
                else if (source >= ModSources::LFO1)
                    pContext->setFrameColor(main), two = true;

                double amount = settings.editor->modAmount(getTag(), i) * 2 - 1;
                double start = std::max(v + ((amount < 0) ? amount * _w : 0), 0.);
                double end = std::min(v + ((amount > 0) ? amount * _w : 0), _w);

                pContext->setLineWidth(2);
                pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });

                if (two)
                {
                    double start = std::max(v - ((amount > 0) ? amount * _w : 0), 0.);
                    double end = std::min(v - ((amount < 0) ? amount * _w : 0), _w);
                    pContext->setFrameColor(OffText);
                    pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });
                }

                a.left += 3;
            }
        }

        a.bottom -= 10;
        a.inset({ 0, 2 });
        auto w2 = pContext->getStringWidth(_n);
        pContext->setFontColor(text);
        pContext->drawString(_n, { a.getCenter().x - w2 / 2, a.top + 10 }, true);
        pContext->setFillColor(back);
        pContext->drawRect({ a.left, a.top + 13, a.right, a.top + 15 }, kDrawFilled);
        pContext->setFillColor(main);
        if (d < 0.5)
            pContext->drawRect({ a.getCenter().x - (0.5 - d) * a.getWidth(), a.top + 13, a.getCenter().x, a.top + 15 }, kDrawFilled);
        else
            pContext->drawRect({ a.getCenter().x, a.top + 13, a.getCenter().x + (d - 0.5) * a.getWidth(), a.top + 15 }, kDrawFilled);
    }

    void Parameter::drawMultigroup(CDrawContext* pContext)
    {
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);

        auto a = getViewSize();
        a.top++;
        a.left++;

        size_t current = getValue() * std::pow(2, settings.parts.size());
        if (settings.vertical)
        {
            auto _y = a.top;
            auto _h = std::ceil(a.getHeight() / settings.parts.size());
            for (int i = 0; i < settings.parts.size(); i++)
            {
                bool _set = (current >> i) & 1U;;
                auto _x = a.getCenter().x;

                String& _n = settings.parts[i];
                auto _w = pContext->getStringWidth(_n);

                pContext->setFillColor(_set ? main : back);
                pContext->setFontColor(_set ? text : OffText);
                BackgroundEffect::draw(pContext, { a.left, _y, a.right, _y + _h - settings.padding - 1 }, 0, settings.dark, true, _set, 0, settings.enabled);
                pContext->drawString(_n, { _x - _w / 2, _y + 6 + (_h - settings.padding - 1) / 2 }, true);

                _y += _h;
            }
        }
        else
        {
            auto _x = a.left;
            auto _w = std::ceil(a.getWidth() / settings.parts.size());
            for (int i = 0; i < settings.parts.size(); i++)
            {
                bool _set = (current >> i) & 1U;;
                auto _y = a.getCenter().y;

                String& _n = settings.parts[i];
                auto _sw = pContext->getStringWidth(_n);

                pContext->setFillColor(_set ? main : back);
                pContext->setFontColor(_set ? text : OffText);
                BackgroundEffect::draw(pContext, { _x, a.top, _x + _w - settings.padding - 1, a.bottom }, 0, settings.dark, true, _set, 0, settings.enabled);
                pContext->drawString(_n, { _x + (_w - settings.padding - 2) / 2 - _sw / 2, _y + 6 }, true);

                _x += _w;
            }
        }
    }

    void Parameter::drawSmallSlider(CDrawContext* pContext)
    {
        double d = getValue();
        auto a = getViewSize();
        a.left += 1;

        if (settings.modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = settings.editor->modSource(getTag(), i);
                if (source == ModSources::None)
                    continue;
                modded++;
                pContext->setLineWidth(2);
                pContext->setFrameColor(Border);
                pContext->drawLine({ a.left, a.bottom }, { a.left, a.top });

                bool two = false;
                if (source >= ModSources::Env1)
                    pContext->setFrameColor(main);
                else if (source >= ModSources::LFO1)
                    pContext->setFrameColor(main), two = true;

                double amount = settings.editor->modAmount(getTag(), i) * 2 - 1;
                double start = std::max(v + ((amount < 0) ? amount * _w : 0), 0.);
                double end = std::min(v + ((amount > 0) ? amount * _w : 0), _w);

                pContext->setLineWidth(2);
                pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });

                if (two)
                {
                    double start = std::max(v - ((amount > 0) ? amount * _w : 0), 0.);
                    double end = std::min(v - ((amount < 0) ? amount * _w : 0), _w);
                    pContext->setFrameColor(OffText);
                    pContext->drawLine({ a.left, a.bottom - end }, { a.left, a.bottom - start });
                }

                a.left += 3;
            }
        }

        a.bottom -= 10;
        a.inset({ 0, 2 });
        auto w2 = pContext->getStringWidth(settings.name);
        pContext->setFontColor(text);
        pContext->drawString(settings.name, { a.getCenter().x - w2 / 2, a.top + 10 }, true);
        pContext->setFillColor(back);
        pContext->drawRect({ a.left - 1, a.top + 13, a.right - 1, a.top + 15 }, kDrawFilled);
        pContext->setFillColor(main);
        pContext->drawRect({ a.left - 1, a.top + 13, a.left + d * a.getWidth() - 1, a.top + 15 }, kDrawFilled);
    }

    void Parameter::drawModBoxes(CDrawContext* pContext)
    {
        pContext->setLineWidth(1);
        auto a = getViewSize();
        a.top = a.bottom - 15;
        auto _p = 5.;
        auto _w = 13;
        if (settings.type == KNOB) a.left = std::floor(a.getCenter().x - _w * (ModAmt * 0.5));
        else a.left = a.left + modded * 3;

        for (int i = 0; i < ModAmt; i++)
        {
            if (modDragIndex == i)
            {
                pContext->setFillColor(MainBackD);
                pContext->drawRect({ a.left, a.top, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
                pContext->setFillColor(settings.dark ? MainBack : MainBackL);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w, a.bottom }, kDrawFilled);
                pContext->setFillColor(settings.dark ? DarkBack : DarkBackH);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
            }
            else
            {
                pContext->setFillColor(MainBackD);
                pContext->drawRect({ a.left, a.top, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
                pContext->setFillColor(settings.dark ? MainBack : MainBackL);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w, a.bottom }, kDrawFilled);
                pContext->setFillColor(settings.dark ? DarkBackD : DarkBack);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
            }

            std::string _v = std::to_string((int)settings.editor->modSource(getTag(), i));
            pContext->setFontColor(main);
            if (settings.editor->modSource(getTag(), i) == ModSources::Ran) _v = (char)('R');
            else if (settings.editor->modSource(getTag(), i) == ModSources::Mod) _v = (char)('M');
            else if (settings.editor->modSource(getTag(), i) == ModSources::Vel) _v = (char)('V');
            else if (settings.editor->modSource(getTag(), i) == ModSources::Key) _v = (char)('K');
            else if (settings.editor->modSource(getTag(), i) >= ModSources::Mac1) _v = (char)('A' + (int)settings.editor->modSource(getTag(), i) - (int)ModSources::Mac1);
            else if (settings.editor->modSource(getTag(), i) >= ModSources::Env1) _v = std::to_string((int)settings.editor->modSource(getTag(), i) - (int)ModSources::Env1 + 1);
            else if (settings.editor->modSource(getTag(), i) >= ModSources::LFO1) _v = std::to_string((int)settings.editor->modSource(getTag(), i) - (int)ModSources::LFO1 + 1);
            else pContext->setFontColor(OffText);

            if (settings.editor->modSource(getTag(), i) != ModSources::None)
            {
                String _s = _v.c_str();
                auto _sw = pContext->getStringWidth(_s);
                pContext->drawString(_s, { a.left + _w / 2 - _sw / 2, a.bottom - 3 });
            }

            a.left += _w;
        }
    }

    void Parameter::draw(CDrawContext* pContext)
    {
        UpdateUnitText(); // Update the text for the unit.

        // Draw modes
        pContext->setFont(pContext->getFont(), 14, kNormalFace);
        pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

        // Depending on knob theme, set colors
        if (settings.dark) back = KnobBackDark; else back = KnobBack;
        if (!settings.enabled) main = OffText, text = OffText; else main = MainGreen, text = MainText;

        modded = 0; // Count how many modulations there are, set to 0 at start.

        switch (settings.type) { // Draw knob type
        case KNOB: drawKnob(pContext); break;
        case SLIDER: drawSlider(pContext); break;
        case NUMBER: drawNumber(pContext); break;
        case BUTTON: drawButton(pContext); break;
        case GROUP: drawGroup(pContext); break;
        case MULTIGROUP: drawMultigroup(pContext); break;
        case INTERPOLATE: drawInterpolate(pContext); break;
        case SMALLSLIDER: drawSmallSlider(pContext); break;
        }

        // If modable knob, draw mod boxes
        if (settings.modable && (settings.type == NUMBER || settings.type == KNOB || settings.type == INTERPOLATE || settings.type == SMALLSLIDER))
            drawModBoxes(pContext);

        setDirty(false); // No longer dirty!
    }
}