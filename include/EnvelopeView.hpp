#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "OptionMenu.hpp"

namespace Kaixo
{
    class EnvelopeCurve : public CView
    {
    public:
        using CView::CView;

        ADSR env;

        bool pressed = false;

        CColor color = MainMain;
        
        enum Section { Attack, AttackCurve, Decay, DecayCurve, Sustain, SustainLine, Release, ReleaseCurve, None } section;
        Section editing = None;
        CPoint pwhere{ 0, 0 };
        double pvaluea;
        double pvalueb;

        Section getSection(double x, double y)
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

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
        {
            pressed = false;
            editing = None;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
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

        CMouseEventResult onMouseEntered(CPoint& where, const CButtonState& buttons) override
        {
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseExited(CPoint& where, const CButtonState& buttons) override
        {
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
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

        double ValueAt(double p, double step)
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

        void draw(CDrawContext* pContext) override
        {
            constexpr CColor back{ 15, 15, 15, 255 };
            CColor main = color;
            CColor brgt = color;
            constexpr CColor crnr{ 128, 128, 128, 255 };
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
            pContext->drawRect(_s, kDrawFilled);
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
            //pContext->setFrameColor(section == AttackCurve ? brgt : main);
            //pContext->drawLines(_lines);
            //_lines.clear();
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
            //pContext->setFrameColor(section == DecayCurve ? brgt : main);
            //pContext->drawLines(_lines);
            //_lines.clear();
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
            //pContext->setFrameColor(section == Sustain ? brgt : main);
            //pContext->drawLines(_lines);
            //_lines.clear();
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
            _points.push_back({_s.left + padding, _s.bottom });
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
            //pContext->drawEllipse(attackCurvePoint, kDrawFilledAndStroked);
            pContext->setFrameColor(section == Decay ? brgt : main);
            pContext->drawRect(decayPoint, kDrawFilledAndStroked);
            //pContext->drawEllipse(decayCurvePoint, kDrawFilledAndStroked);
            pContext->setFrameColor(section == Sustain ? brgt : main);
            pContext->drawRect(sustainPoint, kDrawFilledAndStroked);
            pContext->drawRect(sustainPoint1, kDrawFilledAndStroked);
            pContext->setFrameColor(section == Release ? brgt : main);
            pContext->drawRect(releasePoint, kDrawFilledAndStroked);
            //pContext->drawEllipse(releaseCurvePoint, kDrawFilledAndStroked);

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

    class EnvelopeView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;

        int dragIndex;

        EnvelopeCurve* curve;
        Knob* attk;
        Knob* atkc;
        Knob* attl;
        Knob* decy;
        Knob* dcyc;
        Knob* decl;
        Knob* sust;
        Knob* rels;
        Knob* rlsc;

        Label* time;
        Label* slpe;
        Label* vlue;

        Label* env1;
        Label* env2;
        Label* env3;
        Label* env4;
        Label* env5;

        void valueChanged(CControl* pControl)
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

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseMoved(where, buttons);

            if (dragIndex != -1)
            {
                int* _data = new int[1];
                _data[0] = (int)ModSources::Env1 + dragIndex;
                doDrag(DragDescription{ CDropSource::create((void*)_data, sizeof(int) * 1, IDataPackage::Type::kBinary) });
                dragIndex = -1;
                return kMouseEventHandled;
            }

            if (curve->pressed)
            {
                attk->beginEdit();
                atkc->beginEdit();
                attl->beginEdit();
                decy->beginEdit();
                dcyc->beginEdit();
                decl->beginEdit();
                sust->beginEdit();
                rels->beginEdit();
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

                attk->valueChanged();
                atkc->valueChanged();
                attl->valueChanged();
                decy->valueChanged();
                dcyc->valueChanged();
                decl->valueChanged();
                sust->valueChanged();
                rels->valueChanged();
                rlsc->valueChanged();

                attk->endEdit();
                atkc->endEdit();
                attl->endEdit();
                decy->endEdit();
                dcyc->endEdit();
                decl->endEdit();
                sust->endEdit();
                rels->endEdit();
                rlsc->endEdit();

                curve->setDirty(true);
            }

            return _r;
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            if (where.y - getViewSize().top > 0 && where.y - getViewSize().top < 30 && where.x - getViewSize().left < 335 && where.x - getViewSize().left > 0)
            {
                index = std::floor((where.x - getViewSize().left) / 65);
                dragIndex = index;
                env1->color = env2->color = env3->color = env4->color = env5->color = { 128, 128, 128, 255 };
                if (index == 0) env1->color = { 200, 200, 200, 255 };
                if (index == 1) env2->color = { 200, 200, 200, 255 };
                if (index == 2) env3->color = { 200, 200, 200, 255 };
                if (index == 3) env4->color = { 200, 200, 200, 255 };
                if (index == 4) env5->color = { 200, 200, 200, 255 };

                attk->setTag(Params::Env1A + index);
                atkc->setTag(Params::Env1AC + index);
                attl->setTag(Params::Env1AL + index);
                decy->setTag(Params::Env1D + index);
                dcyc->setTag(Params::Env1DC + index);
                decl->setTag(Params::Env1DL + index);
                sust->setTag(Params::Env1S + index);
                rels->setTag(Params::Env1R + index);
                rlsc->setTag(Params::Env1RC + index);
                
                //for (int i = 0; i < 5; i++)
                //    mod[i]->setTag(Params::Env1M1 + index + (i * 10)), moda[i]->setTag(Params::Env1M1A + index + (i * 10));
                
                setDirty(true);

                return kMouseEventHandled;
            }

            if (where.y - getViewSize().top > 125 && where.y - getViewSize().top < 150)
            {
                int _page = std::floor((where.x - getViewSize().left) / 112);

                time->color = vlue->color = slpe->color = { 128, 128, 128, 255 };
                switch (_page)
                {
                case 0:
                {             
                    attk->setVisible(true);
                    decy->setVisible(true);
                    rels->setVisible(true);
                    sust->setVisible(true);

                    atkc->setVisible(false);
                    dcyc->setVisible(false);
                    rlsc->setVisible(false);

                    attl->setVisible(false);
                    decl->setVisible(false);

                    time->color = { 200, 200, 200, 255 };
                    break;
                }
                case 1:
                {                
                    attk->setVisible(false);
                    decy->setVisible(false);
                    rels->setVisible(false);
                    sust->setVisible(true);

                    atkc->setVisible(true);
                    dcyc->setVisible(true);
                    rlsc->setVisible(true);

                    attl->setVisible(false);
                    decl->setVisible(false);

                    slpe->color = { 200, 200, 200, 255 };
                    break;
                }
                case 2:
                {              
                    attk->setVisible(false);
                    decy->setVisible(false);
                    rels->setVisible(false);
                    sust->setVisible(true);

                    atkc->setVisible(false);
                    dcyc->setVisible(false);
                    rlsc->setVisible(false);

                    attl->setVisible(true);
                    decl->setVisible(true);

                    vlue->color = { 200, 200, 200, 255 };
                    break;
                }
                }

                setDirty(true);
                
                return kMouseEventHandled;
            }
            return _r;
        }

        void createControls(IControlListener* listener, MyEditor* editor)
        {
            attk = new Knob{ {   5, 150,   5 + 65, 155 + 50 }, editor };
            atkc = new Knob{ {   5, 150,   5 + 65, 155 + 50 }, editor };
            attl = new Knob{ {   5, 150,   5 + 65, 155 + 50 }, editor };
            decy = new Knob{ {  70, 150,  70 + 65, 155 + 50 }, editor };
            dcyc = new Knob{ {  70, 150,  70 + 65, 155 + 50 }, editor };
            decl = new Knob{ {  70, 150,  70 + 65, 155 + 50 }, editor };
            sust = new Knob{ { 135, 150, 135 + 65, 155 + 50 }, editor };
            rels = new Knob{ { 200, 150, 200 + 65, 155 + 50 }, editor };
            rlsc = new Knob{ { 200, 150, 200 + 65, 155 + 50 }, editor };
            curve = new EnvelopeCurve{ {  5,  30, 5 + 325, 30 + 95 } };

            attk->color =  MainEnv;
            atkc->color =  MainEnv;
            attl->color =  MainEnv;
            decy->color =  MainEnv;
            dcyc->color =  MainEnv;
            decl->color =  MainEnv;
            sust->color =  MainEnv;
            rels->color =  MainEnv;
            rlsc->color =  MainEnv;
            curve->color = MainEnv;

            time = new Label{ {   5, 130,   5 + 110, 130 + 20 } };
            time->fontsize = 14;
            time->center = true;
            time->color = { 200, 200, 200, 255 };
            time->value = "Time";
            slpe = new Label{ { 115, 130, 115 + 110, 130 + 20 } };
            slpe->fontsize = 14;
            slpe->center = true;
            slpe->color = { 128, 128, 128, 255 };
            slpe->value = "Slope";
            vlue = new Label{ { 225, 130, 225 + 110, 130 + 20 } };
            vlue->fontsize = 14;
            vlue->center = true;
            vlue->color = { 128, 128, 128, 255 };
            vlue->value = "Value";

            env1 = new Label{ {   5,   5,   5 + 65,   5 + 20 } };
            env1->fontsize = 14;
            env1->center = true;
            env1->color = { 200, 200, 200, 255 };
            env1->value = "Gain";
            env2 = new Label{ {  70,   5,  70 + 65,   5 + 20 } };
            env2->fontsize = 14;
            env2->center = true;
            env2->color = { 128, 128, 128, 255 };
            env2->value = "Env 2";
            env3 = new Label{ { 135,   5, 135 + 65,   5 + 20 } };
            env3->fontsize = 14;
            env3->center = true;
            env3->color = { 128, 128, 128, 255 };
            env3->value = "Env 3";
            env4 = new Label{ { 200,   5, 200 + 65,   5 + 20 } };
            env4->fontsize = 14;
            env4->center = true;
            env4->color = { 128, 128, 128, 255 };
            env4->value = "Env 4";
            env5 = new Label{ { 265,   5, 265 + 65,   5 + 20 } };
            env5->fontsize = 14;
            env5->center = true;
            env5->color = { 128, 128, 128, 255 };
            env5->value = "Env 5";

            attk->setListener(listener);
            atkc->setListener(listener);
            attl->setListener(listener);
            decy->setListener(listener);
            dcyc->setListener(listener);
            decl->setListener(listener);
            sust->setListener(listener);
            rels->setListener(listener);
            rlsc->setListener(listener);

            attk->registerControlListener(this);
            atkc->registerControlListener(this);
            attl->registerControlListener(this);
            decy->registerControlListener(this);
            dcyc->registerControlListener(this);
            decl->registerControlListener(this);
            sust->registerControlListener(this);
            rels->registerControlListener(this);
            rlsc->registerControlListener(this);

            attk->setTag(Params::Env1A + index);
            atkc->setTag(Params::Env1AC + index);
            attl->setTag(Params::Env1AL + index);
            decy->setTag(Params::Env1D + index);
            dcyc->setTag(Params::Env1DC + index);
            decl->setTag(Params::Env1DL + index);
            sust->setTag(Params::Env1S + index);
            rels->setTag(Params::Env1R + index);
            rlsc->setTag(Params::Env1RC + index);

            atkc->setVisible(false);
            attl->setVisible(false);
            dcyc->setVisible(false);
            decl->setVisible(false);
            rlsc->setVisible(false);

            attk->name = "Attack";   atkc->name = "Attack";   attl->name = "Attack";
            attk->min = 1;           atkc->min = -100;        attl->min = 0;
            attk->max = 5000;        atkc->max = 100;         attl->max = 1;
            attk->reset = 1;         atkc->reset = 0;         attl->reset = 0;
            attk->decimals = 1;      atkc->decimals = 1;      attl->decimals = 1;
            attk->unit = " s";       atkc->unit = " %";       attl->unit = " dB";
            attk->type = 2;          atkc->type = 2;          attl->type = 2;

            decy->name = "Decay";    dcyc->name = "Decay";    decl->name = "Decay";
            decy->min = 1;           dcyc->min = -100;        decl->min = 0;
            decy->max = 5000;        dcyc->max = 100;         decl->max = 1;
            decy->reset = 600;       dcyc->reset = -50;       decl->reset = 1;
            decy->decimals = 1;      dcyc->decimals = 1;      decl->decimals = 1;
            decy->unit = " s";       dcyc->unit = " %";       decl->unit = " dB";
            decy->type = 2;          dcyc->type = 2;          decl->type = 2;

            rels->name = "Release";  rlsc->name = "Release";
            rels->min = 1;           rlsc->min = -100;
            rels->max = 5000;        rlsc->max = 100;
            rels->reset = 600;       rlsc->reset = -100;
            rels->decimals = 1;      rlsc->decimals = 1;
            rels->unit = " s";       rlsc->unit = " %";
            rels->type = 2;          rlsc->type = 2;

            sust->name = "Sustain";
            sust->min = 0;
            sust->max = 1;
            sust->reset = 0.5;
            sust->decimals = 1;
            sust->unit = " dB";
            sust->type = 2;

            addView(attk);
            addView(atkc);
            addView(attl);
            addView(decy);
            addView(dcyc);
            addView(decl);
            addView(sust);
            addView(rels);
            addView(rlsc);

            addView(curve);

            addView(time);
            addView(slpe);
            addView(vlue);

            addView(env1);
            addView(env2);
            addView(env3);
            addView(env4);
            addView(env5);
        }

        EnvelopeView(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            createControls(listener, editor);
            setBackgroundColor({ 23, 23, 23, 255 });
        }
    };

    struct EnvelopeAttributes
    {
        static inline CPoint Size = { 335, 200 };

        static inline auto Name = "Envelope";
        static inline auto BaseView = UIViewCreator::kCViewContainer;

        static inline std::tuple Attributes
        {
            Attr{ "index", &EnvelopeView::index },
        };
    };

    class EnvelopeViewFactory : public ViewFactoryBase<EnvelopeView, EnvelopeAttributes>
    {
    public:
        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            CRect _size{ CPoint{ 45, 45 }, EnvelopeAttributes::Size };
            int _index = 0;
            attributes.getIntegerAttribute("index", _index);
            MyEditor* _editor = dynamic_cast<MyEditor*>(description->getController());
            auto* _value = new EnvelopeView(_size, _index, description->getControlListener(""), _editor);
            apply(_value, attributes, description);
            return _value;
        }
    };

    static inline EnvelopeViewFactory envelopeViewFactory;
}