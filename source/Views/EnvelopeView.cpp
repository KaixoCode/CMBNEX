#include "Views/EnvelopeView.hpp"

namespace Kaixo
{
    EnvelopeCurve::Section EnvelopeCurve::getSection(double x, double y)
    {
        constexpr auto padding = 8;
        auto _s = getViewSize();
        double _x = x - padding - _s.left;
        double _y = y - padding - _s.top;
        auto _w = _s.getWidth() - padding * 2;
        auto _h = _s.getHeight() - padding * 2;

        const double _space = (_w * 0.30);

        double _apx = 0;
        double _apy = _h * (1 - env.settings.attackLevel);

        if (std::abs(_apx - _x) < 12 && std::abs(_apy - _y) < 12)
            return Attack;

        double _dpx = _space * env.settings.attack / 5.;
        double _dpy = _h * (1 - env.settings.decayLevel);

        if (std::abs(_dpx - _x) < 12 && std::abs(_dpy - _y) < 12)
            return Decay;

        double _spx = _space * (env.settings.decay + env.settings.attack) / 5.;
        double _spy = _h * (1 - env.settings.sustain);

        if (std::abs(_spx - _x) < 12 && std::abs(_spy - _y) < 12)
            return Sustain;

        double _rpx = (_w * 0.70) + _space * (env.settings.release) / 5.;
        double _rpy = _h;

        if (std::abs(_rpx - _x) < 12 && std::abs(_rpy - _y) < 12)
            return Release;

        double _as = 0;
        double _ds = _space * (env.settings.attack) / 5.;
        double _ss = _space * (env.settings.decay + env.settings.attack) / 5.;
        double _rs = (_w * 0.70);
        double _re = _rs + _space * (env.settings.release) / 5.;
        if (_x > _as && _x < _ds) return AttackCurve;
        if (_x > _ds && _x < _ss) return DecayCurve;
        if (_x > _ss && _x < _rs) return SustainLine;
        if (_x > _rs && _x < _re) return ReleaseCurve;
        return None;
    }

    CMouseEventResult EnvelopeCurve::onMouseUp(CPoint& where, const CButtonState& buttons)
    {
        pressed = false;
        editing = None;
        return kMouseEventHandled;
    }

    CMouseEventResult EnvelopeCurve::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        if (getViewSize().pointInside(where))
        {
            pressed = true;
            editing = getSection(where.x, where.y);

            pwhere = where;
            switch (editing)
            {
            case Attack: pvaluea = env.settings.attackLevel; break;
            case AttackCurve: pvaluea = env.settings.attackCurve; break;
            case Decay: pvaluea = env.settings.decayLevel; pvalueb = env.settings.attack; break;
            case DecayCurve: pvaluea = env.settings.decayCurve; break;
            case Sustain: pvaluea = env.settings.sustain; pvalueb = env.settings.decay; break;
            case SustainLine: pvaluea = env.settings.sustain; break;
            case Release: pvalueb = env.settings.release; break;
            case ReleaseCurve: pvaluea = env.settings.releaseCurve; break;
            }

            return kMouseEventHandled;
        }
        return kMouseEventNotHandled;
    }

    CMouseEventResult EnvelopeCurve::onMouseEntered(CPoint& where, const CButtonState& buttons)
    {
        return kMouseEventHandled;
    }

    CMouseEventResult EnvelopeCurve::onMouseExited(CPoint& where, const CButtonState& buttons)
    {
        return kMouseEventHandled;
    }

    CMouseEventResult EnvelopeCurve::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {
        Section b = section;
        section = getSection(where.x, where.y);
        if (b != section)
            setDirty(true);

        if (pressed)
        {
            constexpr auto padding = 8;
            auto _s = getViewSize();
            auto _dx = (where.x - pwhere.x);
            auto _dy = (where.y - pwhere.y);
            auto _w = _s.getWidth() - padding * 2;
            auto _h = _s.getHeight() - padding * 2;
            const double _space = (_w * 0.30);
            auto _dxr = 5 * _dx / _space;
            auto _dyr = -_dy / _h;

            if (editing == AttackCurve && env.settings.attackLevel < env.settings.decayLevel) _dyr *= -1;
            if (editing == DecayCurve && env.settings.decayLevel < env.settings.sustain) _dyr *= -1;

            if (buttons.isShiftSet() && (editing == AttackCurve || editing == DecayCurve || editing == ReleaseCurve))
                _dxr *= 0.25, _dyr *= 0.25;

            switch (editing)
            {
            case Attack:       env.settings.attackLevel = pvaluea + _dyr; break;
            case AttackCurve:  env.settings.attackCurve = pvaluea + _dyr * 2; break;
            case Decay:        env.settings.attack = pvalueb + _dxr, env.settings.decayLevel = pvaluea + _dyr; break;
            case DecayCurve:   env.settings.decayCurve = pvaluea + _dyr * 2; break;
            case Sustain:      env.settings.decay = pvalueb + _dxr, env.settings.sustain = pvaluea + _dyr; break;
            case SustainLine:  env.settings.sustain = pvaluea + _dyr; break;
            case Release:      env.settings.release = pvalueb + _dxr; break;
            case ReleaseCurve: env.settings.releaseCurve = pvaluea + _dyr * 2; break;
            }

            return kMouseEventHandled;
        }

        return kMouseEventNotHandled;
    }

    double EnvelopeCurve::ValueAt(double p, double step)
    {
        constexpr auto _oversample = 1.;
        double _val = 0;
        if (step == 0)
            _val = env.Offset(p);
        else for (double v = p; v < p + step; v += step / _oversample)
        {
            double now = env.Offset(p);
            if (now > _val)
                _val = now;
        }
        return _val;
    }

    void EnvelopeCurve::draw(CDrawContext* pContext)
    {
        CColor back = Colors::ItemBack;
        CColor main = Colors::MainGreen;
        CColor brgt = Colors::MainGreen;
        CColor crnr = Colors::OffText;
        pContext->setLineWidth(1);
        pContext->setLineStyle(CLineStyle{ CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound });
        pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);

        auto _s = getViewSize();

        constexpr auto padding = 5;
        auto _w = _s.getWidth() - padding * 2;
        auto _h = _s.getHeight() - padding * 2 + 1;

        double _px = _s.left + padding;
        double _py = _h - env.settings.attackLevel * _h + _s.top + padding;

        CDrawContext::LineList _lines;
        CRect attackPoint, attackCurvePoint,
            decayPoint, decayCurvePoint, sustainPoint, sustainPoint1,
            releasePoint, releaseCurvePoint;

        env.Trigger();
        pContext->setFillColor(back);
        BackgroundEffect::draw(pContext, { .size = _s, .dark = true });
        pContext->setFrameColor(main);
        double _rsize = 8;
        double _ssize = 6;
        attackPoint = { _px - _rsize / 2, _py - _rsize / 2, _px + _rsize / 2, _py + _rsize / 2 };
        double _x = _s.left + padding;
        const double _space = 5 / (_w * 0.30);

        // Attack curve
        for (double i = 0; i < env.settings.attack; i += _space)
        {
            double _y = _h - ValueAt(i, _space) * _h + _s.top + padding;

            _lines.push_back({ { _px, _py }, { _x, _y } });

            if (std::abs((i) / env.settings.attack - 0.4 - env.settings.attackCurve * 0.1) < _space)
                attackCurvePoint = { _x - _ssize / 2, _y - _ssize / 2, _x + _ssize / 2, _y + _ssize / 2 };

            _px = _x;
            _py = _y;
            _x++;
        }
        { // Decay level
            double _y = _h - env.settings.decayLevel * _h + _s.top + padding;
            _lines.push_back({ { _px, _py }, { _x, _y } });
            _px = _x;
            _py = _y;
            decayPoint = { _px - _rsize / 2, _py - _rsize / 2, _px + _rsize / 2, _py + _rsize / 2 };
            _lines.push_back({ { _px, _py }, { _x, _y } });
        }
        // Decay curve
        for (double i = env.settings.attack; i < env.settings.attack + env.settings.decay; i += _space)
        {
            double _y = _h - ValueAt(i, _space) * _h + _s.top + padding;

            _lines.push_back({ { _px, _py }, { _x, _y } });

            if (std::abs((i - env.settings.attack) / env.settings.decay - 0.4 - env.settings.decayCurve * 0.1) < _space)
                decayCurvePoint = { _x - _ssize / 2, _y - _ssize / 2, _x + _ssize / 2, _y + _ssize / 2 };

            _px = _x;
            _py = _y;
            _x++;
        }
        { // Sustain
            double _y = _h - env.settings.sustain * _h + _s.top + padding;
            _lines.push_back({ { _px, _py }, { _x, _y } });
            _px = _x;
            _py = _y;
            _x = _w * 0.70 + padding;
            sustainPoint = { _px - _rsize / 2, _py - _rsize / 2, _px + _rsize / 2, _py + _rsize / 2 };
            _lines.push_back({ { std::floor(_px), std::floor(_py) }, { std::floor(_x), std::floor(_y) } });
            _px = _x;
            sustainPoint1 = { _px - _rsize / 2, _py - _rsize / 2, _px + _rsize / 2, _py + _rsize / 2 };
        }
        // Release curve
        for (double i = env.settings.attack + env.settings.decay; i < env.settings.attack + env.settings.decay + env.settings.release; i += _space)
        {
            double _y = _h - ValueAt(i, _space) * _h + _s.top + padding;

            _lines.push_back({ { _px, _py }, { _x, _y } });

            if (std::abs((i - env.settings.attack - env.settings.decay) / env.settings.release - 0.4 - env.settings.releaseCurve * 0.1) < _space)
                releaseCurvePoint = { _x - _ssize / 2, _y - _ssize / 2, _x + _ssize / 2, _y + _ssize / 2 };

            _px = _x;
            _py = _y;
            _x++;
        }
        { // Release
            double _y = _h + _s.top + padding;
            _lines.push_back({ { _px, _py }, { _x, _y } });
            _px = _x;
            _py = _y;
        }
        releasePoint = { _px - _rsize / 2, _py - _rsize / 2, _px + _rsize / 2, _py + _rsize / 2 };
        CDrawContext::PointList _points;
        _points.push_back({ _s.left + padding, _s.bottom });
        for (auto& i : _lines)
            _points.push_back(i.first);
        _points.push_back({ _points.back().x, _s.bottom });
        pContext->setFrameColor(main);
        pContext->drawLines(_lines);
        pContext->setFillColor({ main.red, main.green, main.blue, 40 });
        pContext->drawPolygon(_points, kDrawFilled);

        _lines.clear();

        pContext->setLineWidth(2);
        pContext->setFillColor(back);
        pContext->setFrameColor(section == Attack ? brgt : main);
        pContext->drawRect(attackPoint, kDrawFilledAndStroked);
        pContext->setFrameColor(section == Decay ? brgt : main);
        pContext->drawRect(decayPoint, kDrawFilledAndStroked);
        pContext->setFrameColor(section == Sustain ? brgt : main);
        pContext->drawRect(sustainPoint, kDrawFilledAndStroked);
        pContext->drawRect(sustainPoint1, kDrawFilledAndStroked);
        pContext->setFrameColor(section == Release ? brgt : main);
        pContext->drawRect(releasePoint, kDrawFilledAndStroked);

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

    void EnvelopeView::valueChanged(CControl* pControl)
    {
        bool _f = false;
        if (pControl == attk) curve->env.settings.attack = 0.01 + attk->getValue() * 5, _f = true;
        else if (pControl == atkc) curve->env.settings.attackCurve = atkc->getValue() * 2 - 1, _f = true;
        else if (pControl == attl) curve->env.settings.attackLevel = attl->getValue(), _f = true;
        else if (pControl == decy) curve->env.settings.decay = 0.01 + decy->getValue() * 5, _f = true;
        else if (pControl == dcyc) curve->env.settings.decayCurve = dcyc->getValue() * 2 - 1, _f = true;
        else if (pControl == decl) curve->env.settings.decayLevel = decl->getValue(), _f = true;
        else if (pControl == sust) curve->env.settings.sustain = sust->getValue(), _f = true;
        else if (pControl == rels) curve->env.settings.release = 0.01 + rels->getValue() * 5, _f = true;
        else if (pControl == rlsc) curve->env.settings.releaseCurve = rlsc->getValue() * 2 - 1, _f = true;

        if (_f)
            curve->setDirty(true);
    }

    CMouseEventResult EnvelopeView::onMouseMoved(CPoint& where, const CButtonState& buttons)
    {
        auto _r = CViewContainer::onMouseMoved(where, buttons);

        CPoint where2(where);
        where2.offset(-getViewSize().left, -getViewSize().top);
        getTransform().inverse().transform(where2);

        if (pressed && pwhere != where)
        {
            pressed = false;
            nvd1->onMouseMoved(where2, buttons); nvd2->onMouseMoved(where2, buttons);
            nvd3->onMouseMoved(where2, buttons); nvd4->onMouseMoved(where2, buttons);
            nvd5->onMouseMoved(where2, buttons);
        }

        if (curve->pressed)
        {
            attk->beginEdit(); atkc->beginEdit(); attl->beginEdit(); decy->beginEdit();
            dcyc->beginEdit(); decl->beginEdit(); sust->beginEdit(); rels->beginEdit();
            rlsc->beginEdit();

            attk->setValue((curve->env.settings.attack - 0.01) / 5);
            atkc->setValue((curve->env.settings.attackCurve + 1) / 2);
            attl->setValue(curve->env.settings.attackLevel);
            decy->setValue((curve->env.settings.decay - 0.01) / 5);
            dcyc->setValue((curve->env.settings.decayCurve + 1) / 2);
            decl->setValue(curve->env.settings.decayLevel);
            sust->setValue(curve->env.settings.sustain);
            rels->setValue((curve->env.settings.release - 0.01) / 5);
            rlsc->setValue((curve->env.settings.releaseCurve + 1) / 2);

            attk->valueChanged(); atkc->valueChanged(); attl->valueChanged(); decy->valueChanged();
            dcyc->valueChanged(); decl->valueChanged(); sust->valueChanged(); rels->valueChanged();
            rlsc->valueChanged();

            attk->endEdit(); atkc->endEdit(); attl->endEdit(); decy->endEdit(); dcyc->endEdit();
            decl->endEdit(); sust->endEdit(); rels->endEdit(); rlsc->endEdit();

            curve->setDirty(true);
        }

        return kMouseEventHandled;
    }

    void EnvelopeView::UpdateIndex()
    {
        env1->settings.enabled = env2->settings.enabled = env3->settings.enabled =
            env4->settings.enabled = env5->settings.enabled = false;
        if (index == 0) env1->settings.enabled = true;
        if (index == 1) env2->settings.enabled = true;
        if (index == 2) env3->settings.enabled = true;
        if (index == 3) env4->settings.enabled = true;
        if (index == 4) env5->settings.enabled = true;

        attk->setTag(Params::Env1A + index);
        atkc->setTag(Params::Env1AC + index);
        attl->setTag(Params::Env1AL + index);
        decy->setTag(Params::Env1D + index);
        dcyc->setTag(Params::Env1DC + index);
        decl->setTag(Params::Env1DL + index);
        sust->setTag(Params::Env1S + index);
        rels->setTag(Params::Env1R + index);
        rlsc->setTag(Params::Env1RC + index);

        setDirty(true);
    }

    CMouseEventResult EnvelopeView::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        auto _r = CViewContainer::onMouseDown(where, buttons);

        pressed = true;
        pwhere = where;
        if (where.y - getViewSize().top > 125 && where.y - getViewSize().top < 150)
        {
            int _page = std::floor((where.x - getViewSize().left) / 112);

            time->settings.enabled = vlue->settings.enabled = slpe->settings.enabled = false;
            switch (_page)
            {
            case 0:
            {
                attk->setVisible(true); decy->setVisible(true); rels->setVisible(true); sust->setVisible(true); 
                atkc->setVisible(false); dcyc->setVisible(false); rlsc->setVisible(false);
                attl->setVisible(false); decl->setVisible(false); 

                time->settings.enabled = true;
                break;
            }
            case 1:
            {
                attk->setVisible(false); decy->setVisible(false); rels->setVisible(false); sust->setVisible(true);
                atkc->setVisible(true); dcyc->setVisible(true); rlsc->setVisible(true);
                attl->setVisible(false); decl->setVisible(false); 

                slpe->settings.enabled = true;
                break;
            }
            case 2:
            {
                attk->setVisible(false); decy->setVisible(false); rels->setVisible(false); sust->setVisible(true);
                atkc->setVisible(false); dcyc->setVisible(false); rlsc->setVisible(false); 
                attl->setVisible(true); decl->setVisible(true);

                vlue->settings.enabled = true;
                break;
            }
            }

            setDirty(true);

            return kMouseEventHandled;
        }
        return kMouseEventHandled;
    }

    EnvelopeView::EnvelopeView(const CRect& size, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size)
    {
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });

        swtc = new SwitchThing{ { 0, 0, 335, 30 } };
        swtc->setIndex = [&](int i) {
            if (i < 0 || i > 4) return;
            index = i;
            UpdateIndex();
        };

        attk = new Parameter{ {
            .tag = Params::Env1A + index,
            .editor = editor,
            .listener = this,
            .size = {   5, 150,   5 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Attack",
            .min = 0, .max = 5000, .reset = 1, .decimals = 1,
            .unit = " s",
        } };

        atkc = new Parameter{ {
            .tag = Params::Env1AC + index,
            .editor = editor,
            .listener = this,
            .size = {   5, 150,   5 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Attack",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        attl = new Parameter{ {
            .tag = Params::Env1AL + index,
            .editor = editor,
            .listener = this,
            .size = {   5, 150,   5 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Attack",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        decy = new Parameter{ {
            .tag = Params::Env1D + index,
            .editor = editor,
            .listener = this,
            .size = {  70, 150,  70 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Decay",
            .min = 0, .max = 5000, .reset = 600, .decimals = 1,
            .unit = " s",
        } };

        dcyc = new Parameter{ {
            .tag = Params::Env1DC + index,
            .editor = editor,
            .listener = this,
            .size = {  70, 150,  70 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Decay",
            .min = -100, .max = 100, .reset = -50, .decimals = 1,
            .unit = " %",
        } };

        decl = new Parameter{ {
            .tag = Params::Env1DL + index,
            .editor = editor,
            .listener = this,
            .size = {  70, 150,  70 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Decay",
            .min = 0, .max = 100, .reset = 100, .decimals = 1,
            .unit = " %",
        } };

        sust = new Parameter{ {
            .tag = Params::Env1S + index,
            .editor = editor,
            .listener = this,
            .size = { 135, 150, 135 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Sustain",
            .min = 0, .max = 100, .reset = 50, .decimals = 1,
            .unit = " %",
        } };

        rels = new Parameter{ {
            .tag = Params::Env1R + index,
            .editor = editor,
            .listener = this,
            .size = { 200, 150, 200 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Release",
            .min = 0, .max = 5000, .reset = 600, .decimals = 1,
            .unit = " s",
        } };

        rlsc = new Parameter{ {
            .tag = Params::Env1RC + index,
            .editor = editor,
            .listener = this,
            .size = { 200, 150, 200 + 65, 155 + 50 },
            .type = Parameter::NUMBER, .name = "Release",
            .min = -100, .max = 100, .reset = -50, .decimals = 1,
            .unit = " %",
        } };

        curve = new EnvelopeCurve{ {  5,  30, 5 + 325, 30 + 95 } };

        nvd1 = new DragThing{ {   5,   5,   5 + 60,   5 + 18 } };
        nvd2 = new DragThing{ {  71,   5,  71 + 60,   5 + 18 } };
        nvd3 = new DragThing{ { 137,   5, 137 + 60,   5 + 18 } };
        nvd4 = new DragThing{ { 203,   5, 203 + 60,   5 + 18 } };
        nvd5 = new DragThing{ { 269,   5, 269 + 60,   5 + 18 } };

        nvd1->source = ModSources::Env1;
        nvd2->source = ModSources::Env2;
        nvd3->source = ModSources::Env3;
        nvd4->source = ModSources::Env4;
        nvd5->source = ModSources::Env5;

        time = new Label{ {
            .size = {   5, 130,   5 + 110, 130 + 20 },
            .value = "Time", .center = true, .fontsize = 14,
            .enabled = true,
        } };

        slpe = new Label{ {
            .size = { 115, 130, 115 + 110, 130 + 20 },
            .value = "Slope", .center = true, .fontsize = 14,
            .enabled = false,
        } };

        vlue = new Label{ {
            .size = { 225, 130, 225 + 110, 130 + 20 },
            .value = "Value", .center = true, .fontsize = 14,
            .enabled = false,
        } };

        env1 = new Label{ {
            .size = {   5,   5,   5 + 65,   5 + 20 },
            .value = "Gain", .center = true, .fontsize = 14,
            .enabled = true,
        } };

        env2 = new Label{ {
            .size = {  71,   5,  71 + 65,   5 + 20 },
            .value = "Env 2", .center = true, .fontsize = 14,
            .enabled = false,
        } };

        env3 = new Label{ {
            .size = { 137,   5, 137 + 65,   5 + 20 },
            .value = "Env 3", .center = true, .fontsize = 14,
            .enabled = false,
        } };
        env4 = new Label{ {
            .size = { 203,   5, 203 + 65,   5 + 20 },
            .value = "Env 4", .center = true, .fontsize = 14,
            .enabled = false,
        } };

        env5 = new Label{ {
            .size = { 269,   5, 269 + 65,   5 + 20 },
            .value = "Env 5", .center = true, .fontsize = 14,
            .enabled = false,
        } };

        atkc->setVisible(false);
        attl->setVisible(false);
        dcyc->setVisible(false);
        decl->setVisible(false);
        rlsc->setVisible(false);

        addView(attk); addView(atkc); addView(attl); addView(decy); addView(dcyc); addView(decl); 
        addView(sust); addView(rels); addView(rlsc); addView(curve); addView(time); addView(slpe); 
        addView(vlue); addView(env1); addView(env2); addView(env3); addView(env4); addView(env5); 
        addView(nvd1); addView(nvd2); addView(nvd3); addView(nvd4); addView(nvd5); addView(swtc);
    }

    EnvelopeView::~EnvelopeView()
    {
        attk->forget(); atkc->forget(); attl->forget(); decy->forget(); 
        dcyc->forget(); decl->forget(); sust->forget(); rels->forget(); rlsc->forget();
    }
}