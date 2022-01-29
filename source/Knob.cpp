#include "Knob.hpp"

namespace Kaixo
{

    Knob::Knob(const CRect& size, MyEditor* editor, bool dark)
        : editor(editor), CControl(size), dark(dark)
    {
        setDropTarget(this);
    }

    Knob::~Knob()
    {
        if (editor->controller->wakeupCalls.empty()) return;
        std::lock_guard _(editor->controller->lock);
        editor->controller->wakeupCalls.clear();
    }

    int Knob::ModIndexPos(CPoint pos) const
    {
        if (!modable) return -1; // If not modable, always -1

        auto a = getViewSize();
        switch (type) {
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
                for (int i = 0; i < ModAmt; i++) if (editor->modSource(getTag(), i) != ModSources::None) modded++;

                // Calculate possible index
                int _index = std::floor((pos.x - (a.left + 3 * modded)) / 13.);
                if (_index >= 0 && _index < ModAmt) return _index; // If in range, return
            }
            return -1; // Otherwise -1
        default: return -1; // Any other type always -1
        }
    }

    DragOperation Knob::onDragEnter(DragEventData data)
    {   // Determine the mod box we're dragging over
        modDragIndex = ModIndexPos(data.pos); 
        setDirty(true); 
        return modDragIndex == -1 ? DragOperation::None : DragOperation::Copy;
    };

    DragOperation Knob::onDragMove(DragEventData data)
    {   // Determine the mod box we're dragging over
        modDragIndex = ModIndexPos(data.pos);
        setDirty(true);
        return modDragIndex == -1 ? DragOperation::None : DragOperation::Copy;
    };

    void Knob::onDragLeave(DragEventData data)
    {   // On leave, reset to -1
        modDragIndex = -1;
        setDirty(true);
    };

    bool Knob::onDrop(DragEventData data)
    {
        int* _data; // Try to retrieve the mod source from the data
        IDataPackage::Type type;
        data.drag->getData(0, (const void*&)_data, type);

        // If the data type is binary, the data should be correct.
        if (type == IDataPackage::Type::kBinary)
        {   // Cast to the mod source enum
            ModSources _value = (ModSources)_data[0];
            editor->modSource(getTag(), modDragIndex, _value); // Set the source
            editor->modAmount(getTag(), modDragIndex, 0.5); // And reset its amount to the center
            setDirty(true); 
            modDragIndex = -1; // We're done with dragging, so reset to -1
            return true;
        }

        return false;
    };

    CMouseEventResult Knob::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        modEditIndex = ModIndexPos(where); // Determine if hovering over a mod box.

        if (getViewSize().pointInside(where))
        {
            switch (type)
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
                        if (unit == " s") setValue(std::pow((reset - min) / std::max(max - min, 0.0001), 1. / 3.));
                        else if (unit == " Hz") setValue(std::sqrt((reset - min) / std::max(max - min, 0.0001)));
                        else setValue((reset - min) / std::max(max - min, 0.0001)); // Otherwise normal normalized value.
                        valueChanged();
                        endEdit();
                        setDirty(true);
                    }
                    else
                    {
                        // If we're editing a modulation, reset its value to 0.5
                        editor->modAmount(getTag(), modEditIndex, 0.5);
                        setDirty(true);
                    }
                }
                else
                {
                    // If normal click, and we're holding control, and editing a modulation
                    if (buttons.isControlSet() && modEditIndex != -1)
                    {   // We remove the modulation.
                        editor->modSource(getTag(), modEditIndex, ModSources::None);
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
                if (max <= 1) break;
                beginEdit();
                setValue(reset == 0 // Reset == 0 means vertical, otherwise horizontal
                    ? (std::floor(max * (where.y - getViewSize().top) / getViewSize().getHeight()) / (max - 1))
                    : (std::floor(max * (where.x - getViewSize().left) / getViewSize().getWidth()) / (max - 1)));
                valueChanged();
                endEdit();
                setDirty(true);
                break;
            case MULTIGROUP: // Multigroup uses powers of 2 to store bits of data in a double.
                beginEdit(); // First retrieve the currently edited index
                size_t value = reset == 0 ? std::floor(max * (where.y - getViewSize().top) / getViewSize().getHeight())
                    : std::floor(max * (where.x - getViewSize().left) / getViewSize().getWidth());

                size_t current = getValue() * std::pow(2, max); // Get the current bits set
                current ^= 1ULL << value; // Toggle the current index

                setValue(current / std::pow(2, max)); // Set the new value
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

    CMouseEventResult Knob::onMouseUp(CPoint& where, const CButtonState& buttons)
    {   // Reset pressed
        pressed = false;
        return kMouseEventHandled;
    }

    CMouseEventResult Knob::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {
        if (pressed)
        {
            switch (type)
            {
            case KNOB:
            case SLIDER:
            case NUMBER:
            case INTERPOLATE:
            case SMALLSLIDER:
            {
                double mult = buttons.isShiftSet() ? 0.25 : 1;
                double diff = 0;
                if ((type == SLIDER || type == SMALLSLIDER) && modEditIndex == -1) diff = (where.x - pwhere.x) / getViewSize().getWidth();
                else if (type == INTERPOLATE && modEditIndex == -1) diff = (where.x - pwhere.x) / (parts.size() * getViewSize().getWidth());
                else diff = (pwhere.y - where.y) * 0.005;
                pwhere = where;

                if (modEditIndex == -1)
                {
                    beginEdit();
                    setValue(getValue() + mult * diff);
                    valueChanged();
                    endEdit();
                    setDirty(true);
                }
                else
                {
                    double value = editor->modAmount(getTag(), modEditIndex);
                    editor->modAmount(getTag(), modEditIndex, value + mult * diff);
                    setDirty(true);
                }
                return kMouseEventHandled;
            }
            }
        }
        return kMouseEventNotHandled;
    }

    void Knob::UpdateUnitText()
    {
        double _v = getValue();
        double val = _v * (max - min) + min;
        if (unit == " dB") // Special case for dB: convert linear multiplier to logarithmic decibel number
        {
            val = lin2db(val);
            std::string _format = std::format("{:." + std::to_string((int)decimals + (val < -10 ? -1 : 0)) + "f}", val);
            str = _format.c_str() + unit;
        }
        else if (unit == " Hz") // Special case for Hz: convert to kHz when > 1000
        {
            val = std::pow(_v, 2) * (max - min) + min;
            if (val > 1000)
            {
                val /= 1000;
                std::string _format = std::format("{:." + std::to_string((int)decimals + (val > 10 ? 0 : 1)) + "f}", val);
                str = _format.c_str() + String{ " kHz" };
            }
            else
            {
                std::string _format = std::format("{:." + std::to_string((int)decimals + (val > 10 ? 0 : 1)) + "f}", val);
                str = _format.c_str() + unit;
            }
        }
        else if (unit == " s") // Special case for seconds: convert to ms when < 1000
        {
            val = std::pow(_v, 3) * (max - min) + min;
            if (val > 1000)
            {
                val /= 1000;
                std::string _format = std::format("{:." + std::to_string((int)decimals + (val > 10 ? 0 : 1)) + "f}", val);
                str = _format.c_str() + String{ " s" };
            }
            else
            {
                std::string _format = std::format("{:." + std::to_string((int)decimals + (val > 10 ? val > 100 ? -1 : 0 : 1)) + "f}", val);
                str = _format.c_str() + String{ " ms" };
            }
        }
        else if (unit == "time") // Special case for time: display time string: 1/4, 1/8 etc.
        {
            size_t _type = std::floor(_v * (TimesAmount - 1));
            str = TimesString[_type];
        }
        else if (unit == "pan") // Special case for pan: display L/R with number
        {
            int _iv = _v * 100 - 50;
            std::string _nmr = std::to_string(std::abs(_iv));
            if (_iv == 0) str = "C";
            else if (_iv < 0) str = _nmr.c_str() + String{ "L" };
            else if (_iv > 0) str = _nmr.c_str() + String{ "R" };
        }
        else if (unit == " %") // Special case for percentage: round to 1 less decimals when > 100
        {
            if (decimals == 0 && std::round(val) == 0) val = 0;
            std::string _format = std::format("{:." + std::to_string((int)decimals + (val >= 100 || val <= -100 ? -1 : 0)) + "f}", val);
            str = _format.c_str() + unit;
        }
        else
        {
            if (decimals == 0 && std::round(val) == 0) val = 0;
            std::string _format = std::format("{:." + std::to_string((int)decimals) + "f}", val);
            str = _format.c_str() + unit;
        }
    }

    void Knob::drawKnob(CDrawContext* pContext)
    {
        auto a = getViewSize();
        auto w1 = pContext->getStringWidth(str);
        auto w2 = pContext->getStringWidth(name);
        auto v = getValueNormalized() * 270;
        if (modable)
            a.bottom -= 10;

        auto start = 135;
        auto end = 135 + v;
        if (min == -max)
        {
            v -= 135;
            start = v < 0 ? 270 + v : 270;
            end = v < 0 ? 270 : 270 + v;
        }

        a.inset({ 10, 22 });

        pContext->setFontColor(text);
        pContext->drawString(str, { a.getCenter().x - w1 / 2, a.getBottomCenter().y + 12 }, true);
        pContext->drawString(name, { a.getCenter().x - w2 / 2, a.getTopCenter().y - 8 }, true);

        if (modable)
        {
            a.inset({ -2, -2 });
            v = getValueNormalized() * 270;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = editor->modSource(getTag(), i);
                if (source == ModSources::None)
                    continue;

                pContext->setLineWidth(2);
                pContext->setFrameColor(Border);
                pContext->drawArc(a, 135, 135 + 270, kDrawStroked);

                bool two = false;
                if (source >= ModSources::Env1)
                    pContext->setFrameColor(main);
                else if (source >= ModSources::LFO1)
                    pContext->setFrameColor(main), two = true;

                double amount = editor->modAmount(getTag(), i) * 2 - 1;
                double start = std::max(135 + v + ((amount < 0) ? amount * 270 : 0), 135.);
                double end = std::min(135 + v + ((amount > 0) ? amount * 270 : 0), 135 + 270.);

                pContext->setLineWidth(2);
                pContext->drawArc(a, start, end, kDrawStroked);

                if (two)
                {
                    double start = std::max(135 + v - ((amount > 0) ? amount * 270 : 0), 135.);
                    double end = std::min(135 + v - ((amount < 0) ? amount * 270 : 0), 135 + 270.);
                    pContext->setFrameColor({ 128, 128, 128, 255 });
                    pContext->drawArc(a, start, end, kDrawStroked);
                }

                a.inset({ 3, 3 });
            }

            a.inset({ 2, 2 });
        }

        pContext->setLineWidth(6);
        pContext->setFrameColor(back);
        pContext->drawArc(a, 135, 135 + 270, kDrawStroked);
        pContext->setFrameColor(main);
        pContext->drawArc(a, start, end, kDrawStroked);
    }

    void Knob::drawSlider(CDrawContext* pContext)
    {
        auto a = getViewSize();
        auto v = getValueNormalized() * (getWidth() - 4);
        auto w1 = pContext->getStringWidth(str);

        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);
        pContext->setFillColor(back);
        a.top += 20;
        a.left += 1;
        pContext->drawRect(a, kDrawFilledAndStroked);
        a.left -= 1;
        a.top -= 1;
        a.inset({ 2, 2 });

        pContext->setFillColor(main);
        if (min == -max)
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
        pContext->drawString(str, { a.getCenter().x - w1 / 2, a.getCenter().y + 4 }, true);
        pContext->drawString(name, { a.left, a.getTopCenter().y - 8 }, true);
    }

    void Knob::drawNumber(CDrawContext* pContext)
    {
        auto a = getViewSize();
        a.left += 1;
        if (modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = editor->modSource(getTag(), i);
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

                double amount = editor->modAmount(getTag(), i) * 2 - 1;
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
        pContext->drawString(str, { a.left, a.getCenter().y + 1 }, true);
        pContext->setFontColor(text);
        pContext->drawString(name, { a.left, a.getTopCenter().y - 8 }, true);
    }

    void Knob::drawButton(CDrawContext* pContext)
    {
        auto a = getViewSize();
        auto w2 = pContext->getStringWidth(name);
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);
        pContext->setFillColor(getValue() > 0.5 ? main : back);
        a.top += 1;
        a.left += 1;
        bool pressed = getValue() > 0.5;
        BackgroundEffect::draw(pContext, a, 0, dark, true, pressed, 0, enabled);
        pContext->setFontColor(getValue() > 0.5 ? text : OffText);
        pContext->drawString(name, { a.getCenter().x - w2 / 2, a.getCenter().y + 6 }, true);
    }

    void Knob::drawGroup(CDrawContext* pContext)
    {
        std::string_view _view = name.operator const Steinberg::char8 * ();
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);

        auto a = getViewSize();
        a.top++;
        a.left++;

        if (reset == 0)
        {
            auto _y = a.top;
            auto _h = std::ceil(a.getHeight() / max);
            for (int i = 0; i < max; i++)
            {
                auto _c = _view.find_first_of(',');
                auto _x = a.getCenter().x;

                String _n{ _view.data(), (int32)_c };
                auto _w = std::floor(pContext->getStringWidth(_n));

                bool pressed = std::abs(getValue() * (max - 1) - i) < 0.5;

                pContext->setFillColor(pressed ? main : back);
                pContext->setFontColor(pressed ? text : OffText);

                BackgroundEffect::draw(pContext, { a.left, _y, a.right, _y + _h - min - 1 }, 0, dark, true, pressed, 0, enabled);
                pContext->drawString(_n, { _x - _w / 2, _y + 6 + (_h - min - 1) / 2 }, true);

                if (_c != std::string_view::npos)
                    _view = _view.substr(_c + 1);

                _y += _h;
            }
        }
        else
        {
            auto _x = a.left;
            auto _w = std::ceil(a.getWidth() / max);
            for (int i = 0; i < max; i++)
            {
                auto _c = _view.find_first_of(',');
                auto _y = a.getCenter().y;

                String _n{ _view.data(), (int32)_c };
                auto _sw = std::floor(pContext->getStringWidth(_n));

                bool pressed = std::abs(getValue() * (max - 1) - i) < 0.5;

                pContext->setFillColor(pressed ? main : back);
                pContext->setFontColor(pressed ? text : OffText);
                BackgroundEffect::draw(pContext, { _x, a.top, _x + _w - min - 1, a.bottom }, 0, dark, true, pressed, 0, enabled);
                pContext->drawString(_n, { _x + (_w - min - 2) / 2 - _sw / 2, _y + 6 }, true);

                if (_c != std::string_view::npos)
                    _view = _view.substr(_c + 1);

                _x += _w;
            }
        }
    }

    void Knob::drawInterpolate(CDrawContext* pContext)
    {
        size_t v = std::floor(getValue() * (parts.size() - 1) + 0.5);
        double d = (getValue() * (parts.size() - 1) + 0.5) - v;
        String _n = parts[v].c_str();

        auto a = getViewSize();
        a.left += 1;

        if (modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = editor->modSource(getTag(), i);
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

                double amount = editor->modAmount(getTag(), i) * 2 - 1;
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

    void Knob::drawMultigroup(CDrawContext* pContext)
    {
        std::string_view _view = name.operator const Steinberg::char8 * ();
        pContext->setLineWidth(1);
        pContext->setFrameColor(Border);

        auto a = getViewSize();
        a.top++;
        a.left++;

        size_t current = getValue() * std::pow(2, max);
        if (reset == 0)
        {
            auto _y = a.top;
            auto _h = std::ceil(a.getHeight() / max);
            for (int i = 0; i < max; i++)
            {
                bool _set = (current >> i) & 1U;;
                auto _c = _view.find_first_of(',');
                auto _x = a.getCenter().x;

                String _n{ _view.data(), (int32)_c };
                auto _w = pContext->getStringWidth(_n);

                pContext->setFillColor(_set ? main : back);
                pContext->setFontColor(_set ? text : OffText);
                BackgroundEffect::draw(pContext, { a.left, _y, a.right, _y + _h - min - 1 }, 0, dark, true, _set, 0, enabled);
                pContext->drawString(_n, { _x - _w / 2, _y + 6 + (_h - min - 1) / 2 }, true);

                if (_c != std::string_view::npos)
                    _view = _view.substr(_c + 1);

                _y += _h;
            }
        }
        else
        {
            auto _x = a.left;
            auto _w = std::ceil(a.getWidth() / max);
            for (int i = 0; i < max; i++)
            {
                bool _set = (current >> i) & 1U;;
                auto _c = _view.find_first_of(',');
                auto _y = a.getCenter().y;

                String _n{ _view.data(), (int32)_c };
                auto _sw = pContext->getStringWidth(_n);

                pContext->setFillColor(_set ? main : back);
                pContext->setFontColor(_set ? text : OffText);
                BackgroundEffect::draw(pContext, { _x, a.top, _x + _w - min - 1, a.bottom }, 0, dark, true, _set, 0, enabled);
                pContext->drawString(_n, { _x + (_w - min - 2) / 2 - _sw / 2, _y + 6 }, true);

                if (_c != std::string_view::npos)
                    _view = _view.substr(_c + 1);

                _x += _w;
            }
        }
    }

    void Knob::drawSmallSlider(CDrawContext* pContext)
    {
        double d = getValue();
        auto a = getViewSize();
        a.left += 1;

        if (modable)
        {
            auto _w = a.getHeight();
            auto v = getValueNormalized() * _w;
            for (int i = 0; i < ModAmt; i++)
            {
                auto source = editor->modSource(getTag(), i);
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

                double amount = editor->modAmount(getTag(), i) * 2 - 1;
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
        auto w2 = pContext->getStringWidth(name);
        pContext->setFontColor(text);
        pContext->drawString(name, { a.getCenter().x - w2 / 2, a.top + 10 }, true);
        pContext->setFillColor(back);
        pContext->drawRect({ a.left - 1, a.top + 13, a.right - 1, a.top + 15 }, kDrawFilled);
        pContext->setFillColor(main);
        pContext->drawRect({ a.left - 1, a.top + 13, a.left + d * a.getWidth() - 1, a.top + 15 }, kDrawFilled);
    }

    void Knob::drawModBoxes(CDrawContext* pContext)
    {
        pContext->setLineWidth(1);
        auto a = getViewSize();
        a.top = a.bottom - 15;
        auto _p = 5.;
        auto _w = 13;
        if (type == KNOB) a.left = std::floor(a.getCenter().x - _w * (ModAmt * 0.5));
        else a.left = a.left + modded * 3;

        for (int i = 0; i < ModAmt; i++)
        {
            if (modDragIndex == i)
            {
                pContext->setFillColor(MainBackD);
                pContext->drawRect({ a.left, a.top, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
                pContext->setFillColor(dark ? MainBack : MainBackL);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w, a.bottom }, kDrawFilled);
                pContext->setFillColor(dark ? DarkBack : DarkBackH);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
            }
            else
            {
                pContext->setFillColor(MainBackD);
                pContext->drawRect({ a.left, a.top, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
                pContext->setFillColor(dark ? MainBack : MainBackL);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w, a.bottom }, kDrawFilled);
                pContext->setFillColor(dark ? DarkBackD : DarkBack);
                pContext->drawRect({ a.left + 1, a.top + 1, a.left + _w - 1, a.bottom - 1 }, kDrawFilled);
            }

            std::string _v = std::to_string((int)editor->modSource(getTag(), i));
            pContext->setFontColor(main);
            if (editor->modSource(getTag(), i) == ModSources::Vel) _v = (char)('V');
            else if (editor->modSource(getTag(), i) == ModSources::Key) _v = (char)('K');
            else if (editor->modSource(getTag(), i) >= ModSources::Mac1) _v = (char)('P' + (int)editor->modSource(getTag(), i) - (int)ModSources::Mac1);
            else if (editor->modSource(getTag(), i) >= ModSources::Env1) _v = std::to_string((int)editor->modSource(getTag(), i) - (int)ModSources::Env1 + 1);
            else if (editor->modSource(getTag(), i) >= ModSources::LFO1) _v = std::to_string((int)editor->modSource(getTag(), i) - (int)ModSources::LFO1 + 1);
            else pContext->setFontColor(OffText);

            if (editor->modSource(getTag(), i) != ModSources::None)
            {
                String _s = _v.c_str();
                auto _sw = pContext->getStringWidth(_s);
                pContext->drawString(_s, { a.left + _w / 2 - _sw / 2, a.bottom - 3 });
            }

            a.left += _w;
        }
    }

    void Knob::draw(CDrawContext* pContext)
    {
        UpdateUnitText(); // Update the text for the unit.

        // Draw modes
        pContext->setFont(pContext->getFont(), 14, kNormalFace);
        pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

        // Depending on knob theme, set colors
        if (dark) back = KnobBackDark; else back = KnobBack;
        if (!enabled) main = OffText, text = OffText; else main = MainGreen, text = MainText;

        modded = 0; // Count how many modulations there are, set to 0 at start.

        switch (type) { // Draw knob type
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
        if (modable && (type == NUMBER || type == KNOB || type == INTERPOLATE || type == SMALLSLIDER))
            drawModBoxes(pContext);

        setDirty(false); // No longer dirty!
    }
}