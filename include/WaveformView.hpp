#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"

namespace Kaixo
{
    class WaveformView : public CView
    {
    public:
        using CView::CView;

        double index = 0;
        double repeat = 1;

        CColor color = MainMain;

        double level = 1;
        double sync = 0;
        double shaper = 0;
        double shaper2 = 0;
        double shaper3 = 0.5;
        double pos = 0;
        double phase = 0;
        double pw = 0.5;

        double ValueAt(double p, double step)
        {
            constexpr auto _oversample = 1.;
            double _val = 0;
            for (double v = p; v < p + step; v += step / _oversample)
            {
                double _rp = std::fmod(v, 1.0);
                double _pw = pw * 2 - 1;
                if (_pw > 0)
                {
                    double ph = Shapers::shaper4(_rp, shaper);
                    double _d = std::max(0.000001, 1 - _pw);
                    double _p1 = ph / _d;
                    _val += _p1 > 1 ? 0 : Shapers::simpleshaper(
                        Shapers::shaper24(Wavetables::basic(std::fmod((ph * (sync * 7 + 1) / _d + phase) + 100, 1.), pos)
                            , shaper2), shaper3);
                }
                else
                {
                    double ph = Shapers::shaper4(std::fmod(_rp, 1.), shaper);
                    double _d = std::max(0.000001, 1 + _pw);
                    double _p1 = (1 - ph) / _d;
                    _val += _p1 > 1 ? 0 : Shapers::simpleshaper(
                        Shapers::shaper24(Wavetables::basic(std::fmod(((ph + _pw) * (sync * 7 + 1) / _d + phase) + 100, 1.), pos)
                            , shaper2), shaper3);
                }
            }
            return level * (-_val / _oversample);
        }

        void draw(CDrawContext* pContext) override
        {
            constexpr CColor back{ 15, 15, 15, 255 };
            CColor main = color;

            constexpr CColor crnr{ 128, 128, 128, 255 };
            pContext->setLineWidth(1);
            pContext->setLineStyle(CLineStyle{ CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound });
            pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

            auto _s = getViewSize();

            pContext->setFillColor(back);
            pContext->drawRect(_s, kDrawFilled);
            constexpr auto padding = 4;
            auto _w = _s.getWidth();
            auto _h = _s.getHeight() - padding * 2;
            auto _step = repeat / _w;

            double _px = _s.left;
            double _py = (0.5 + ValueAt(0, _step) * 0.5) * _h + _s.top + padding;

            CDrawContext::LineList _lines;
            CDrawContext::PointList _points;

            _points.push_back({ _s.left, _s.top + _h / 2 + padding });
            for (double i = 0; i < _w; i += 0.25)
            {
                double _p = repeat * i / _w;
                double _x = i + _s.left;
                double _y = (0.5 + ValueAt(_p, _step) * 0.5) * _h + _s.top + padding;

                _lines.push_back({ { _px, _py }, { _x, _y } });
                _points.push_back({ _px, _py });

                _px = _x;
                _py = _y;
            }
            _points.push_back({ _s.right, _s.top + _h / 2 + padding});

            pContext->setFrameColor(main);
            pContext->drawLines(_lines);
            pContext->setFillColor({ main.red, main.green, main.blue, 40 });
            pContext->drawPolygon(_points, kDrawFilled);

            double _bsize = 8;
            pContext->setLineWidth(1);
            pContext->setFrameColor(crnr);
            pContext->drawLine({ _s.left, _s.top }, { _s.left,  _s.top + _bsize });
            pContext->drawLine({ _s.left, _s.top }, { _s.left + _bsize, _s.top });

            pContext->drawLine({ _s.right - 1, _s.top }, { _s.right - 1,  _s.top + _bsize });
            pContext->drawLine({ _s.right - 1, _s.top }, { _s.right - _bsize - 1, _s.top });

            pContext->drawLine({ _s.right - 1, _s.bottom - 1 }, { _s.right - 1,  _s.bottom - _bsize - 1 });
            pContext->drawLine({ _s.right - 1, _s.bottom - 1 }, { _s.right - _bsize - 1, _s.bottom - 1 });

            pContext->drawLine({ _s.left, _s.bottom - 1 }, { _s.left,  _s.bottom - _bsize - 1 });
            pContext->drawLine({ _s.left, _s.bottom - 1 }, { _s.left + _bsize, _s.bottom - 1 });
        }
    };
}