#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "ViewFactoryBase.hpp"
#include "myplugincontroller.hpp"

namespace Kaixo
{
    class Knob : public CControl
    {
    public:
        enum Type
        {
            KNOB = 0, SLIDER, NUMBER, BUTTON, GROUP, INTERPOLATE, MULTIGROUP
        };

        static inline Knob* selected = nullptr;

        double min = 0;
        double max = 0;
        double reset = 0;
        int decimals = 1;
        int type = 1;

        double index = 4;

        String name;
        String unit;

        MyEditor* editor;

        Knob(const CRect& size, MyEditor* editor)
            : editor(editor), CControl(size)
        {}

        CMouseEventResult onMouseCancel() override
        {
            if (selected == this)
                selected = nullptr;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
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
                        beginEdit();
                        if (unit == " s")
                            setValue(std::pow((reset - min) / std::max(max - min, 0.0001), 1./3.));
                        else if (unit == " Hz")
                            setValue(std::sqrt((reset - min) / std::max(max - min, 0.0001)));
                        else
                            setValue((reset - min) / std::max(max - min, 0.0001));
                        valueChanged();
                        endEdit();
                        setDirty(true);
                    }
                    break;
                case BUTTON:
                    beginEdit();
                    setValue(getValue() == 0 ? 1 : 0);
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
                    beginEdit();
                    double mult = buttons.isShiftSet() ? 0.25 : 1;
                    double diff = 0;
                    if (type == SLIDER) diff = (where.x - pwhere.x) / getViewSize().getWidth();
                    if (type == INTERPOLATE) diff = (where.x - pwhere.x) / (parts.size() * getViewSize().getWidth());
                    else diff = (pwhere.y - where.y) * 0.005;
                    setValue(getValue() + mult * diff);
                    pwhere = where;
                    valueChanged();
                    endEdit();
                    setDirty(true);
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
                std::string _format = std::format("{:." + std::to_string((int)decimals) + "f}", val);
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
                    std::string _format = std::format("{:." + std::to_string((int)decimals + (val > 10 ? 0 : 1)) + "f}", val);
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


            CColor main = CColor{ 0, 179, 98, 255 };
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

                auto start = 135;
                auto end = 135 + v;
                if (min == -max)
                {
                    v -= 135;
                    start = v < 0 ? 270 + v : 270;
                    end = v < 0 ? 270 : 270 + v;
                }

                a.inset({ 10, 22 });

                pContext->setLineWidth(6);
                pContext->setFrameColor(back);
                pContext->drawArc(a, 135, 135 + 270, kDrawStroked);
                pContext->setFrameColor(main);
                pContext->drawArc(a, start, end, kDrawStroked);
                pContext->setFontColor(text);
                pContext->drawString(str, { a.getCenter().x - w1 / 2, a.getBottomCenter().y + 14 }, true);
                pContext->drawString(name, { a.getCenter().x - w2 / 2, a.getTopCenter().y - 8 }, true);
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
                a.top += 20;
                a.inset({ 2, 2 });
                pContext->setFontColor(main);
                pContext->drawString(str, { a.left, a.getCenter().y + 6 }, true);
                pContext->setFontColor(text);
                pContext->drawString(name, { a.left, a.getTopCenter().y - 8 }, true);
                break;
            }
            case BUTTON:
            {
                pContext->setLineWidth(1);
                pContext->setFrameColor(brdr);
                pContext->setFillColor(getValue() ? main : back);
                a.top += 1;
                a.left += 1;
                pContext->drawRect(a, kDrawFilledAndStroked);
                pContext->setFontColor(getValue() ? text : offt);
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