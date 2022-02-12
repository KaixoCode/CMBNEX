#include "Views/WaveformView.hpp"

namespace Kaixo
{
    double WaveformView::ValueAt(double p, double step)
    {
        constexpr auto _oversample = 1.;
        double _val = 0;
        for (double v = p; v < p + step; v += step / _oversample)
        {
            osc.phase = std::fmod(v, 1.0);
            osc.settings.frequency = 1;
            double _vs = (LFO ? osc.OffsetSimple(phase) : osc.Offset(phase, true, false)) + offset * 2 - 1;

            // Fold
            if (enableFold)
                _vs = Shapers::fold(_vs * (fold * 15 + 1), bias * 2 - 1);

            // Drive
            if (enableDrive)
                _vs = Shapers::drive(_vs, drivegain * 3 + 1, drive);
            else
                _vs = std::max(std::min(_vs, 1.), -1.);

            _val += _vs;
        }
        return level * (-_val / _oversample);
    }

    void WaveformView::draw(CDrawContext* pContext)
    {
        CColor back = Colors::ItemBack;
        CColor main = Colors::MainGreen;
        if (!enabled) main = Colors::OffText;

        CColor crnr = Colors::OffText;
        pContext->setLineWidth(1);
        pContext->setLineStyle(CLineStyle{ CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound });
        pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

        auto _s = getViewSize();

        BackgroundEffect::draw(pContext, { .size = _s, .dark = true });
        _s.inset({ 1, 1 });
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
        _points.push_back({ _s.right, _s.top + _h / 2 + padding });

        pContext->setFrameColor(main);
        pContext->drawLines(_lines);
        pContext->setFillColor({ main.red, main.green, main.blue, 40 });
        pContext->drawPolygon(_points, kDrawFilled);

        double _bsize = 8;
        _s.inset({ -1, 0 });
        _s.top -= 1;
        _s.bottom += 1;
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
}