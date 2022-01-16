#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "ViewFactoryBase.hpp"
#include "myplugincontroller.hpp"

namespace Kaixo
{
    class Knob : public CControl, public IDropTarget
    {
    public:
        enum Type
        {
            KNOB = 0, SLIDER, NUMBER, BUTTON, GROUP, INTERPOLATE, MULTIGROUP
        };

        static inline Knob* selected = nullptr;

        int dragIndex = -1;

        int ModIndexPos(CPoint pos) 
        {
            if (!modable) return -1;
            auto a = getViewSize();
            if (type == KNOB)
            {
                if (pos.y > a.bottom - 16 && pos.y < a.bottom)
                {
                    int _index = std::floor((pos.x - (a.getCenter().x - 13 * ModAmt * 0.5)) / 13.);
                    if (_index >= 0 && _index < ModAmt)
                        return _index;
                    return -1;
                }
                return -1;
            }
            else if (type == NUMBER)
            {
                if (pos.y > a.bottom - 16 && pos.y < a.bottom)
                {
                    int modded = 0;
                    for (int i = 0; i < ModAmt; i++)
                        if (editor->modSource(getTag(), i) != ModSources::None) modded++;

                    int _index = std::floor((pos.x - (a.left + 2 * modded)) / 13.);
                    if (_index >= 0 && _index < ModAmt)
                        return _index;
                    return -1;
                }
                return -1;

            }

            return -1;
        }

        DragOperation onDragEnter(DragEventData data) 
        {
            dragIndex = ModIndexPos(data.pos);
            setDirty(true);
            return dragIndex == -1 ? DragOperation::None : DragOperation::Copy;
        };

        DragOperation onDragMove(DragEventData data) 
        {
            dragIndex = ModIndexPos(data.pos);
            setDirty(true);
            return dragIndex == -1 ? DragOperation::None : DragOperation::Copy;
        };

        void onDragLeave(DragEventData data) 
        {
            dragIndex = -1;
            setDirty(true);
        };

        bool onDrop(DragEventData data)
        {
            int* _data;
            IDataPackage::Type type;
            data.drag->getData(0, (const void*&)_data, type);

            ModSources _value = (ModSources) _data[0];

            editor->modSource(getTag(), dragIndex, _value);
            editor->modAmount(getTag(), dragIndex, 0.5);
            setDirty(true);
            dragIndex = -1;
            return true;
        };

        double min = 0;
        double max = 0;
        double reset = 0;
        int decimals = 1;
        int type = 1;

        bool modable = true;

        double index = 4;

        int editIndex = -1;

        CColor color = MainMain;

        String name;
        String unit;

        MyEditor* editor;

        Knob(const CRect& size, MyEditor* editor)
            : editor(editor), CControl(size)
        {
            setDropTarget(this);
        }

        CMouseEventResult onMouseCancel() override
        {
            if (selected == this)
                selected = nullptr;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            editIndex = ModIndexPos(where);

            if (getViewSize().pointInside(where))
            {
                switch (type) 
                {
                case KNOB:
                case SLIDER:
                case NUMBER:
                case INTERPOLATE:
                    if (buttons.isDoubleClick())
                    {
                        if (editIndex == -1)
                        {
                            beginEdit();
                            if (unit == " s")
                                setValue(std::pow((reset - min) / std::max(max - min, 0.0001), 1. / 3.));
                            else if (unit == " Hz")
                                setValue(std::sqrt((reset - min) / std::max(max - min, 0.0001)));
                            else
                                setValue((reset - min) / std::max(max - min, 0.0001));
                            valueChanged();
                            endEdit();
                            setDirty(true);
                        }
                        else
                        {
                            editor->modAmount(getTag(), editIndex, 0.5);
                            setDirty(true);
                        }
                    }
                    else
                    {
                        if (buttons.isControlSet() && editIndex != -1)
                        {
                            editor->modSource(getTag(), editIndex, ModSources::None);
                            setDirty(true);
                        }
                    }
                    break;
                case BUTTON:
                    beginEdit();
                    setValue(getValue() > 0.5 ? 0 : 1);
                    valueChanged();
                    endEdit();
                    setDirty(true);
                    break;
                case GROUP:
                    if (max <= 1) break;
                    beginEdit();
                    setValue(reset == 0  
                        ? (std::floor(max * (where.y - getViewSize().top) / getViewSize().getHeight()) / (max - 1)) 
                        : (std::floor(max * (where.x - getViewSize().left) / getViewSize().getWidth()) / (max - 1)));
                    valueChanged();
                    endEdit();
                    setDirty(true);
                    break;
                case MULTIGROUP:
                    beginEdit();
                    size_t value = reset == 0 ? std::floor(max * (where.y - getViewSize().top) / getViewSize().getHeight())
                        : std::floor(max * (where.x - getViewSize().left) / getViewSize().getWidth());
                    
                    size_t current = getValue() * std::pow(2, max);
                    current ^= 1ULL << value;

                    setValue(current / std::pow(2, max));
                    valueChanged();
                    endEdit();
                    setDirty(true);
                    break;
                }

                if (buttons.isLeftButton())
                    pressed = true, pwhere = where;

                selected = this;
                return kMouseEventHandled;
            }

            return kMouseEventNotHandled;
        }

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
        {
            pressed = false;
            switch (type)
            {
            case KNOB:
            case SLIDER:
            case NUMBER:
            case INTERPOLATE:
                return kMouseEventHandled;
            }
            return kMouseEventNotHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
        {
            if (pressed)
            {
                switch (type)
                {
                case KNOB:
                case SLIDER:
                case NUMBER:
                case INTERPOLATE:
                {
                    double mult = buttons.isShiftSet() ? 0.25 : 1;
                    double diff = 0;
                    if (type == SLIDER) diff = (where.x - pwhere.x) / getViewSize().getWidth();
                    if (type == INTERPOLATE) diff = (where.x - pwhere.x) / (parts.size() * getViewSize().getWidth());
                    else diff = (pwhere.y - where.y) * 0.005;
                    pwhere = where;

                    if (editIndex == -1)
                    {
                        beginEdit();
                        setValue(getValue() + mult * diff);
                        valueChanged();
                        endEdit();
                        setDirty(true);
                    }
                    else 
                    {
                        double value = editor->modAmount(getTag(), editIndex);
                        editor->modAmount(getTag(), editIndex, value + mult * diff);
                        setDirty(true);
                    }
                    return kMouseEventHandled;
                }
                }
            }
            return kMouseEventNotHandled;
        }

        void draw(CDrawContext* pContext) override
        { 
            double _v = getValue();
            double val = _v * (max - min) + min;
            if (unit == " dB")
            {
                val = lin2db(val);
                std::string _format = std::format("{:." + std::to_string((int)decimals + (val < -10 ? -1 : 0)) + "f}", val);
                str = _format.c_str() + unit;
            }
            else if (unit == " Hz")
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
            else if (unit == " s")
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
            else if (unit == "time")
            {
                size_t _type = std::floor(_v * (TimesAmount - 1));
                str = TimesString[_type];
            }
            else if (unit == "pan")
            {
                int _iv = _v * 100 - 50;
                std::string _nmr = std::to_string(std::abs(_iv));
                if (_iv == 0) str = "C";
                else if (_iv < 0) str = _nmr.c_str() + String{ "L" };
                else if (_iv > 0) str = _nmr.c_str() + String{ "R" };
            }
            else if (unit == " %")
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
            pContext->setFont(pContext->getFont(), 14, kNormalFace);
            pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

            auto a = getViewSize();
            auto w1 = pContext->getStringWidth(str);
            auto w2 = pContext->getStringWidth(name);

            int modded = 0;

            CColor main = color;;
            //    index == 0 ? CColor{ 179, 0, 0, 255 } 
            //: index == 1 ? CColor{ 0, 119, 179, 255 } 
            //: index == 2 ? CColor{ 179, 116, 0, 255 } 
            //: index == 3 ? CColor{ 0, 179, 98, 255 }
            //: CColor{ 76, 24, 181, 255 };
            constexpr CColor text{ 200, 200, 200, 255 };
            constexpr CColor back{ 40, 40, 40, 255 };
            constexpr CColor brdr{ 30, 30, 30, 255 };
            constexpr CColor offt{ 128, 128, 128, 255 };

            switch (type)
            {
            case KNOB:
            {
                auto v = getValueNormalized() * 270;
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
                        pContext->setFrameColor(brdr);
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


                break;
            }
            case SLIDER: // Slider horizontal
            {
                auto v = getValueNormalized() * (getWidth() - 4);

                pContext->setLineWidth(1);
                pContext->setFrameColor(brdr);
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
                break;
            }
            case NUMBER: // Slider vertical
            {
                a = getViewSize();
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
                        pContext->setFrameColor(brdr);
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
                            pContext->setFrameColor({ 128, 128, 128, 255 });
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


                break;
            }
            case BUTTON:
            {
                pContext->setLineWidth(1);
                pContext->setFrameColor(brdr);
                pContext->setFillColor(getValue() > 0.5 ? main : back);
                a.top += 1;
                a.left += 1;
                pContext->drawRect(a, kDrawFilledAndStroked);
                pContext->setFontColor(getValue() > 0.5 ? text : offt);
                pContext->drawString(name, { a.getCenter().x - w2 / 2, a.getCenter().y + 6 }, true);
                break;
            }
            case GROUP:
            {
                std::string_view _view = name.operator const Steinberg::char8 * ();
                pContext->setLineWidth(1);
                pContext->setFrameColor(brdr);

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
                        auto _w = pContext->getStringWidth(_n);

                        pContext->setFillColor(std::abs(getValue() * (max - 1) - i) < 0.5 ? main : back);
                        pContext->setFontColor(std::abs(getValue() * (max - 1) - i) < 0.5 ? text : offt);
                        pContext->drawRect({ a.left, _y, a.right, _y + _h - min - 1 }, kDrawFilledAndStroked);
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
                        auto _sw = pContext->getStringWidth(_n);

                        pContext->setFillColor(std::abs(getValue() * (max - 1) - i) < 0.5 ? main : back);
                        pContext->setFontColor(std::abs(getValue() * (max - 1) - i) < 0.5 ? text : offt);
                        pContext->drawRect({ _x, a.top, _x + _w - min - 1, a.bottom }, kDrawFilledAndStroked);
                        pContext->drawString(_n, { _x + (_w - min - 2) / 2 - _sw / 2, _y + 6 }, true);

                        if (_c != std::string_view::npos)
                            _view = _view.substr(_c + 1);

                        _x += _w;
                    }
                }
                break;
            }
            case MULTIGROUP:
            {
                std::string_view _view = name.operator const Steinberg::char8 * ();
                pContext->setLineWidth(1);
                pContext->setFrameColor(brdr);

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
                        pContext->setFontColor(_set ? text : offt);
                        pContext->drawRect({ a.left, _y, a.right, _y + _h - min - 1 }, kDrawFilledAndStroked);
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
                        pContext->setFontColor(_set ? text : offt);
                        pContext->drawRect({ _x, a.top, _x + _w - min - 1, a.bottom }, kDrawFilledAndStroked);
                        pContext->drawString(_n, { _x + (_w - min - 2) / 2 - _sw / 2, _y + 6 }, true);

                        if (_c != std::string_view::npos)
                            _view = _view.substr(_c + 1);

                        _x += _w;
                    }
                }
                break;
            }
            case INTERPOLATE:
            {
                size_t v = std::floor(getValue() * (parts.size() - 1) + 0.5);
                double d = (getValue() * (parts.size() - 1) + 0.5) - v;
                String _n = parts[v].c_str();
                a = getViewSize();
                a.top += 20;
                a.inset({ 2, 2 });
                auto w2 = pContext->getStringWidth(_n);
                pContext->setFontColor(text);
                pContext->drawString(_n, { a.getCenter().x - w2 / 2, a.getCenter().y - 6 }, true);
                pContext->setFillColor(back);
                pContext->drawRect(a, kDrawFilled);
                pContext->setFillColor(main);
                if (d < 0.5)
                    pContext->drawRect({ a.getCenter().x - (0.5 - d) * a.getWidth(), a.top, a.getCenter().x, a.bottom }, kDrawFilled);
                else
                    pContext->drawRect({ a.getCenter().x, a.top, a.getCenter().x + (d - 0.5) * a.getWidth(), a.bottom }, kDrawFilled);
                break;
            }
            }

            if (type == NUMBER || type == KNOB)
            {
                pContext->setLineWidth(1);
                a = getViewSize();
                a.top = a.bottom - 15;
                auto _p = 5.;
                auto _w = 13;
                if (type == KNOB)
                    a.left = std::floor(a.getCenter().x - _w * (ModAmt * 0.5));
                else
                    a.left = a.left + modded * 3;

                for (int i = 0; i < ModAmt; i++)
                {
                    if (dragIndex == i)
                    {
                        pContext->setFrameColor({ 70, 70, 70, 255 });
                        pContext->setFillColor({ 18, 18, 18, 255 });
                    }
                    else
                    {
                        pContext->setFrameColor({ 30, 30, 30, 255 });
                        pContext->setFillColor({ 18, 18, 18, 255 });
                    }

                    std::string _v = std::to_string((int)editor->modSource(getTag(), i));
                    if (editor->modSource(getTag(), i) >= ModSources::Osc1)
                    {
                        pContext->setFontColor(main);
                        _v = (char)('A' + (int)editor->modSource(getTag(), i) - (int)ModSources::Osc1);
                    } else if (editor->modSource(getTag(), i) >= ModSources::Env1)
                    {
                        pContext->setFontColor(main);
                        _v = std::to_string((int)editor->modSource(getTag(), i) - (int)ModSources::Env1 + 1);
                    }
                    else if (editor->modSource(getTag(), i) >= ModSources::LFO1)
                    {
                        pContext->setFontColor(main);
                        _v = std::to_string((int)editor->modSource(getTag(), i) - (int)ModSources::LFO1 + 1);
                    }
                    else
                    {
                        pContext->setFontColor({ 128, 128, 128, 255 });
                    }
                    pContext->drawRect({ a.left, a.top, a.left + _w, a.bottom }, kDrawFilledAndStroked);

                    if (editor->modSource(getTag(), i) != ModSources::None)
                    {
                        String _s = _v.c_str();
                        auto _sw = pContext->getStringWidth(_s);
                        pContext->drawString(_s, { a.left + _w / 2 - _sw / 2, a.bottom - 3 });
                    }

                    a.left += _w;
                }
            }

            setDirty(false);
        }

        std::vector<std::string> parts;

        bool pressed = false;
        CPoint pwhere;

        String str;

        CLASS_METHODS(Knob, CControl)
    };

    struct KnobAttributes
    {
        static inline CPoint Size = { 55, 80 };

        static inline auto Name = "CustomKnob";
        static inline auto BaseView = UIViewCreator::kCControl;

        static inline std::tuple Attributes
        {
            Attr{ "max", &Knob::max },
            Attr{ "min", &Knob::min },
            Attr{ "type", &Knob::type },
            Attr{ "reset", &Knob::reset },
            Attr{ "decimals", &Knob::decimals },
            Attr{ "unit", &Knob::unit },
            Attr{ "displayName", &Knob::name },
        };
    };

    using KnobViewFactory = ViewFactoryBase<Knob, KnobAttributes>;

    static inline KnobViewFactory knobViewFactory;
}