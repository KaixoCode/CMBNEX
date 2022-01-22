#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "Label.hpp"
#include "WaveformView.hpp"

namespace Kaixo
{
    class XYPanel : public CView, public IControlListener
    {
    public:
        Knob* x;
        Knob* y;

        void valueChanged(CControl* pControl)
        {
            setDirty(true);
        }

        bool pressed = false;

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            pressed = true;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
        {
            pressed = false;
            return kMouseEventHandled;
        }

        CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override
        {
            if (pressed)
            {
                auto a = getViewSize();
                double _padding = 12;
                double _x = constrain((where.x - a.left - _padding) / (getWidth() - 2 * _padding), 0., 1.);
                double _y = constrain(1 - (where.y - a.top - _padding) / (getHeight() - 2 * _padding), 0., 1.);
                x->beginEdit();
                x->setValue(_x);
                x->valueChanged();
                x->endEdit();
                y->beginEdit();
                y->setValue(_y);
                y->valueChanged();
                y->endEdit();
                setDirty(true);
                return kMouseEventHandled;
            }
            return kMouseEventNotHandled;
        }

        XYPanel(Knob* x, Knob* y, const CRect& rect)
            : CView(rect), x(x), y(y)
        {
            x->registerControlListener(this);
            y->registerControlListener(this);
        }

        bool enabled = true;

        BackgroundEffect bg{ getViewSize() };

        void draw(CDrawContext* pContext) override
        {
            auto a = getViewSize();
            bg.draw(pContext);
            a.inset({ 1, 1 });
            pContext->setFillColor(MainBack);
            pContext->drawRect(a, kDrawFilled);
            pContext->setLineStyle(CLineStyle{ CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound });

            pContext->setDrawMode(kAntiAliasing);
            pContext->setFrameColor(Border); 
            constexpr static double nmr = 10;
            for (int i = 0; i < nmr; i++)
            {
                double x = a.left + i * a.getWidth() / nmr;
                double y = a.top + i * a.getHeight() / nmr;
                pContext->drawLine({ x, a.top }, { x, a.bottom - 1 });
                pContext->drawLine({ a.left, y }, { a.right - 1, y });
            }

            pContext->setFrameColor(KnobBack);
            pContext->setLineWidth(1);

            pContext->drawLine({ a.getCenter().x, a.top }, { a.getCenter().x, a.bottom - 1 });
            pContext->drawLine({ a.left, a.getCenter().y }, { a.right - 1, a.getCenter().y });

            pContext->setDrawMode(kAntiAliasing | kNonIntegralMode);
            double _padding = 12;
            double _x = a.left + x->getValue() * (getWidth() - 2 * _padding + 1) + _padding;
            double _y = a.top + (1 - y->getValue()) * (getHeight() - 2 * _padding + 1) + _padding;
            if (enabled) pContext->setFrameColor(MainGreen);
            else pContext->setFrameColor(OffText);
            pContext->setFillColor(MainBack);
            pContext->setLineWidth(3);
            pContext->drawRect({ _x - _padding / 2, _y - _padding / 2, _x + _padding / 2, _y + _padding / 2 }, kDrawFilledAndStroked);

            auto _s = getViewSize();
            double _bsize = 8;
            pContext->setLineWidth(1);
            pContext->setFrameColor(OffText);
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

    class OscillatorToggle : public CViewContainer
    {
    public:

        Label* outl = nullptr;
        Knob* dest = nullptr;

        Knob* phse = nullptr; // Phase
        Knob* sync = nullptr; // Sync
        Knob* plsw = nullptr; // Pulse Width
        Knob* wtps = nullptr; // WTP
        Knob* bend = nullptr; // Bend
        Knob* dcof = nullptr; // DC Offset

        Label* shpl = nullptr; // Shaper title
        Knob* enbs = nullptr; // Enable Shaper
        Knob* shpr = nullptr; // Shaper 1       
        Knob* shpm = nullptr; // Shaper 1 Mix
        Knob* shp2 = nullptr; // Shaper 2
        Knob* sh2m = nullptr; // Shaper 2 Mix
        Knob* shmr = nullptr; // Shaper morph

        Label* nsel = nullptr; // Noise title
        Knob* enbn = nullptr; // Enable Noise
        Knob* nois = nullptr; // Noise
        Knob* nscl = nullptr; // Noise Color

        Label* fldl = nullptr; // Fold title
        Knob* enbf = nullptr; // Enable fold
        Knob* flda = nullptr; // Fold Amount
        Knob* bias = nullptr; // Fold Bias

        Label* drvl = nullptr; // Drive title
        Knob* enbd = nullptr; // Enable Drive
        Knob* drvg = nullptr; // Drive Gain
        Knob* drve = nullptr; // Drive amount

        Label* ftrl = nullptr; // Filter title
        Knob* enbr = nullptr; // Enable Filter
        Knob* fltr = nullptr; // Filter type
        Knob* freq = nullptr; // Filter Frequency  
        Knob* reso = nullptr; // Filter Resonance  

        XYPanel* panel;

        int index;
        OscillatorToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            setBackgroundColor({ 0, 0, 0, 0 });

            double y = 2;

            auto* v = new BackgroundEffect{ { 0, 5 + y, getWidth(), getHeight() - 5 - y } };
            addView(v);

            CColor brdclr = MainBack;
            if (index == 0)
            {
                auto* v = new BackgroundEffect{ { 0, 0, getWidth() / 2 - 2, 8 } };
                v->sides = 2;
                addView(v);
            }
            else if (index == 1)
            {
                auto* v = new BackgroundEffect{ { getWidth() / 2 + 3, 0, getWidth(), 8 } };
                v->sides = 2;
                addView(v);
            }
            else if (index == 2)
            {
                auto* v = new BackgroundEffect{ { 0, getHeight() - 8, getWidth() / 2 - 2, getHeight() } };
                v->sides = 2;
                addView(v);
            }
            else if (index == 3)
            {
                auto* v = new BackgroundEffect{ { getWidth() / 2 + 3, getHeight() - 8, getWidth(), getHeight() } };
                v->sides = 2;
                addView(v);
            }

            auto* v1 = new BackgroundEffect{ {   5, 10 + y,   5 + 179, 10 + 200 + y }, true };
            addView(v1);

            auto* v2 = new BackgroundEffect{ { 189, 10 + y, 189 + 206, 10 + 200 + y}, true };
            addView(v2);

            auto* v3 = new BackgroundEffect{ { 400, 10 + y, 400 +  70, 10 + 200 + y }, true };
            addView(v3);

            auto* v4 = new BackgroundEffect{ { 475, 10 + y, 475 +  70, 10 + 200 + y }, true };
            addView(v4);

            auto* v5 = new BackgroundEffect{ { 550, 10 + y, 550 +  70, 10 + 200 + y }, true };
            addView(v5);

            auto* v6 = new BackgroundEffect{ { 625, 10 + y, 625 +  70, 10 + 200 + y }, true };
            addView(v6);

            auto* v7 = new BackgroundEffect{ { 700, 10 + y, 700 +  30, 10 + 200 + y }, true };
            addView(v7);

            outl = new Label{ { 699,  13 + y, 699 + 30,  13 + 18 + y  } };
            outl->fontsize = 14, outl->center = true, outl->value = "Out";
            dest = new Knob{ { 704,  39 + y, 704 + 20,  39 + 169 + y }, editor };
            dest->name = "I,II,III,IV,V,VI,O";
            dest->min = 4;
            dest->max = 7;
            dest->reset = 0;
            dest->decimals = 0;
            dest->unit = "";
            dest->type = 6;

            dest->setListener(listener); dest->setTag(Params::DestA + index);

            phse = new Knob{ {  10,  15 + y,  10 +  55,  15 +  90 + y }, editor, true }; phse->setListener(listener); phse->setTag(Params::Phase1 + index);
            plsw = new Knob{ {  67,  15 + y,  67 +  55,  15 +  90 + y }, editor, true }; plsw->setListener(listener); plsw->setTag(Params::PulseW1 + index);
            bend = new Knob{ { 124,  15 + y, 124 +  55,  15 +  90 + y }, editor, true }; bend->setListener(listener); bend->setTag(Params::Bend1 + index);
            wtps = new Knob{ {  10, 115 + y,  10 +  55, 115 +  90 + y }, editor, true }; wtps->setListener(listener); wtps->setTag(Params::WTPos1 + index);
            sync = new Knob{ {  67, 115 + y,  67 +  55, 115 +  90 + y }, editor, true }; sync->setListener(listener); sync->setTag(Params::Sync1 + index);
            dcof = new Knob{ { 124, 115 + y, 124 +  55, 115 +  90 + y }, editor, true }; dcof->setListener(listener); dcof->setTag(Params::DCOff1 + index);

            shpl = new Label{ { 209,  13 + y, 209 + 90,  13 + 18 + y } };
            shpl->fontsize = 14, shpl->center = false, shpl->value = "Waveshaper";
            enbs = new Knob{ { 193,  14 + y, 193 +  14,  14 +  14 + y }, editor }; enbs->setListener(listener); enbs->setTag(Params::ENBShaper1 + index);
            shpr = new Knob{ { 194,  35 + y, 194 +  65,  35 +  53 + y }, editor }; shpr->setListener(listener); shpr->setTag(Params::Shaper1 + index);
            shpm = new Knob{ { 259,  35 + y, 259 +  65,  35 +  53 + y }, editor }; shpm->setListener(listener); shpm->setTag(Params::ShaperMix1 + index);
            shp2 = new Knob{ { 324, 152 + y, 324 + 65, 152 + 53 + y }, editor }; shp2->setListener(listener); shp2->setTag(Params::Shaper21 + index);
            sh2m = new Knob{ { 324,  95 + y, 324 + 65,  95 + 53 + y }, editor }; sh2m->setListener(listener); sh2m->setTag(Params::Shaper2Mix1 + index);
            shmr = new Knob{ { 324,  35 + y, 324 + 65,  35 + 53 + y }, editor }; shmr->setListener(listener); shmr->setTag(Params::ShaperMorph1 + index);
            panel = new XYPanel{ shpr, shp2, { 194, 93 + y, 194 + 125, 93 + 112 + y } };

            fldl = new Label{ { 420,  13 + y, 420 + 40,  13 + 18 + y } };
            fldl->fontsize = 14, fldl->center = false, fldl->value = "Fold";
            enbf = new Knob{ { 404,  14 + y, 404 + 14,  14 + 14 + y }, editor }; enbf->setListener(listener); enbf->setTag(Params::ENBFold1 + index);
            flda = new Knob{ { 405,  52 + y, 405 + 60,  52 + 95 + y }, editor }; flda->setListener(listener); flda->setTag(Params::Fold1 + index);
            bias = new Knob{ { 405, 152 + y, 405 + 65, 152 + 53 + y }, editor }; bias->setListener(listener); bias->setTag(Params::Bias1 + index);

            nsel = new Label{ { 495,  13 + y, 495 + 40,  13 + 18 + y  } };
            nsel->fontsize = 14, nsel->center = false, nsel->value = "Noise";
            enbn = new Knob{ { 479,  14 + y, 479 + 14,  14 + 14 + y }, editor }; enbn->setListener(listener); enbn->setTag(Params::ENBNoise1 + index);
            nois = new Knob{ { 480,  52 + y, 480 + 60,  52 + 95 + y }, editor }; nois->setListener(listener); nois->setTag(Params::Noise1 + index);
            nscl = new Knob{ { 480, 152 + y, 480 + 65, 152 + 53 + y }, editor }; nscl->setListener(listener); nscl->setTag(Params::Color1 + index);

            drvl = new Label{ { 570,  13 + y, 570 + 40,  13 + 18 + y } };
            drvl->fontsize = 14, drvl->center = false, drvl->value = "Drive";
            enbd = new Knob{ { 554,  14 + y, 554 + 14,  14 + 14 + y }, editor }; enbd->setListener(listener); enbd->setTag(Params::ENBDrive1 + index);
            drvg = new Knob{ { 555,  52 + y, 555 + 60,  52 + 95 + y }, editor }; drvg->setListener(listener); drvg->setTag(Params::DriveGain1 + index);
            drve = new Knob{ { 555, 152 + y, 555 + 65, 152 + 53 + y }, editor }; drve->setListener(listener); drve->setTag(Params::DriveAmt1 + index);

            ftrl = new Label{ { 645,  13 + y, 645 + 40,  13 + 18 + y } };
            ftrl->fontsize = 14, ftrl->center = false, ftrl->value = "Filter";
            enbr = new Knob{ { 629,  14 + y, 629 + 14,  14 + 14 + y }, editor }; enbr->setListener(listener); enbr->setTag(Params::ENBFilter1 + index);
            fltr = new Knob{ { 629,  32 + y, 629 + 63,  32 + 17 + y }, editor }; fltr->setListener(listener); fltr->setTag(Params::Filter1 + index);
            freq = new Knob{ { 630,  52 + y, 630 + 60,  52 + 95 + y }, editor }; freq->setListener(listener); freq->setTag(Params::Freq1 + index);
            reso = new Knob{ { 630, 152 + y, 630 + 65, 152 + 53 + y }, editor }; reso->setListener(listener); reso->setTag(Params::Reso1 + index);

            phse->name = "Phase"; plsw->name = "PW";  bend->name = "Bend"; wtps->name = "WTP"; sync->name = "Sync"; dcof->name = "Offset"; 
            phse->min = 0;        plsw->min = -100;   bend->min = -100;    wtps->min = 0;      sync->min = 100;     dcof->min = -1;
            phse->max = 100;      plsw->max = 100;    bend->max = 100;     wtps->max = 100;    sync->max = 800;     dcof->max = 1;      
            phse->reset = 0;      plsw->reset = 0;    bend->reset = 0;     wtps->reset = 0;    sync->reset = 100;   dcof->reset = 0;      
            phse->decimals = 1;   plsw->decimals = 1; bend->decimals = 1;  wtps->decimals = 1; sync->decimals = 1;  dcof->decimals = 2;   
            phse->unit = " %";    plsw->unit = " %";  bend->unit = " %";   wtps->unit = " %";  sync->unit = " %";   dcof->unit = "";    
            phse->type = 0;       plsw->type = 0;     bend->type = 0;      wtps->type = 0;     sync->type = 0;      dcof->type = 0;       

            enbs->name = "";    shpr->name = "SHP-X"; shpm->name = "Mix-X"; shp2->name = "SHP-Y"; sh2m->name = "Mix-Y"; shmr->name = "Morph";
            enbs->min = 0;      shpr->min = -100;     shpm->min = 0;        shp2->min = -100;     sh2m->min = 0;        shmr->min = 0;
            enbs->max = 1;      shpr->max = 100;      shpm->max = 100;      shp2->max = 100;      sh2m->max = 100;      shmr->max = 100;
            enbs->reset = 0;    shpr->reset = 0;      shpm->reset = 50;     shp2->reset = 0;      sh2m->reset = 50;     shmr->reset = 0;
            enbs->decimals = 1; shpr->decimals = 1;   shpm->decimals = 1;   shp2->decimals = 1;   sh2m->decimals = 1;   shmr->decimals = 1;
            enbs->unit = "";    shpr->unit = " %";    shpm->unit = " %";    shp2->unit = " %";    sh2m->unit = " %";    shmr->unit = " %";
            enbs->type = 3;     shpr->type = 2;       shpm->type = 2;       shp2->type = 2;       sh2m->type = 2;       shmr->type = 2;

            enbn->name = "";    nois->name = "Amount"; nscl->name = "Color";
            enbn->min = 0;      nois->min = 0;         nscl->min = -100;
            enbn->max = 1;      nois->max = 100;       nscl->max = 100;
            enbn->reset = 0;    nois->reset = 0;       nscl->reset = 0;
            enbn->decimals = 1; nois->decimals = 1;    nscl->decimals = 0;
            enbn->unit = "";    nois->unit = " %";     nscl->unit = "";
            enbn->type = 3;     nois->type = 0;        nscl->type = 2;
            
            enbf->name = "";    flda->name = "Gain";   bias->name = "Bias";
            enbf->min = 0;      flda->min = 1;         bias->min = -1;
            enbf->max = 1;      flda->max = 15.9;      bias->max = 1;
            enbf->reset = 0;    flda->reset = 1;       bias->reset = 0;
            enbf->decimals = 1; flda->decimals = 1;    bias->decimals = 2;
            enbf->unit = "";    flda->unit = " dB";    bias->unit = "";
            enbf->type = 3;     flda->type = 0;        bias->type = 2;

            enbd->name = "";    drvg->name = "Gain"; drve->name = "Drive";
            enbd->min = 0;      drvg->min = 1;       drve->min = 0;
            enbd->max = 1;      drvg->max = 4;       drve->max = 100;
            enbd->reset = 0;    drvg->reset = 1;     drve->reset = 0;
            enbd->decimals = 1; drvg->decimals = 1;  drve->decimals = 1;
            enbd->unit = "";    drvg->unit = " dB";  drve->unit = " %";
            enbd->type = 3;     drvg->type = 0;      drve->type = 2;

            enbr->name = "";    fltr->name = "L,H,B"; freq->name = "Freq"; reso->name = "Reso";
            enbr->min = 0;      fltr->min = 4;        freq->min = 20;      reso->min = 0;
            enbr->max = 1;      fltr->max = 3;        freq->max = 22000;   reso->max = 125;
            enbr->reset = 0;    fltr->reset = 1;      freq->reset = 0;     reso->reset = 0;
            enbr->decimals = 1; fltr->decimals = 0;   freq->decimals = 1;  reso->decimals = 1;
            enbr->unit = "";    fltr->unit = "";      freq->unit = " Hz";  reso->unit = " %";
            enbr->type = 3;     fltr->type = 4;       freq->type = 0;      reso->type = 2;

            addView(phse);
            addView(sync);
            addView(plsw);
            addView(wtps);
            addView(bend);
            addView(dcof);

            addView(shpl);
            addView(enbs);
            addView(shpr);
            addView(shpm);
            addView(shp2);
            addView(sh2m);
            addView(shmr);

            addView(nsel);
            addView(enbn);
            addView(nois);
            addView(nscl);

            addView(fldl);
            addView(enbf);
            addView(flda);
            addView(bias);

            addView(drvl);
            addView(enbd);
            addView(drvg);
            addView(drve);

            addView(ftrl);
            addView(enbr);
            addView(fltr);
            addView(freq);
            addView(reso);

            addView(dest);
            addView(outl);

            addView(panel);
        }

        ~OscillatorToggle()
        {
            phse->forget();
            sync->forget();
            plsw->forget();
            wtps->forget();
            bend->forget();
            dcof->forget();

            enbs->forget();
            shpr->forget();
            shpm->forget();
            shp2->forget();
            sh2m->forget();
            shmr->forget();
            
            enbn->forget();
            nois->forget();
            nscl->forget();
            
            enbf->forget();
            flda->forget();
            bias->forget();

            enbd->forget();
            drvg->forget();
            drve->forget();

            enbr->forget();
            fltr->forget();
            freq->forget();
            reso->forget();

            dest->forget();
        }
    };

    class OscillatorView : public CViewContainer, public IControlListener
    {
    public:
        OscillatorToggle* toggle;
        bool selected = false;

        Knob* enbl = nullptr; // Enable            
        Knob* tune = nullptr; // Tune              
        Knob* pann = nullptr; // Pos               
        Knob* detn = nullptr; // Detune            
        Knob* volm = nullptr; // Gain              
        Label* titl = nullptr; // Oscillator title
        WaveformView* wfrm = nullptr; // Waveform view

        int index = 0;
        bool viewWave = false;
        
        bool modulateChange = false;

        void Select()
        {
            if (!selected)
            {
                toggle->setVisible(true);
                //bgef->pressed = true;
                //bgef->setDirty(true);
                //enbl->setViewSize(((CRect)enbl->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //tune->setViewSize(((CRect)tune->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //pann->setViewSize(((CRect)pann->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //detn->setViewSize(((CRect)detn->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //volm->setViewSize(((CRect)volm->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //titl->setViewSize(((CRect)titl->getViewSize()).offset({ bgef->edge, bgef->edge }));
                //wfrm->setViewSize(((CRect)wfrm->getViewSize()).offset({ bgef->edge, bgef->edge }));
            }
            selected = true;
        }

        void Unselect()
        {
            if (selected)
            {
                toggle->setVisible(false);
                //bgef->pressed = false;
                //bgef->setDirty(true);
                //enbl->setViewSize(((CRect)enbl->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //tune->setViewSize(((CRect)tune->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //pann->setViewSize(((CRect)pann->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //detn->setViewSize(((CRect)detn->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //volm->setViewSize(((CRect)volm->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //titl->setViewSize(((CRect)titl->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
                //wfrm->setViewSize(((CRect)wfrm->getViewSize()).offset({ -bgef->edge, -bgef->edge }));
            }
            selected = false;
        }

        void onIdle() 
        {
            if (modulateChange)
            {
                modulateChange = false;

                wfrm->phase = toggle->phse->getModValue();
                wfrm->osc.settings.wtpos = toggle->wtps->getModValue();
                wfrm->osc.settings.sync = toggle->sync->getModValue() ;
                wfrm->osc.settings.bend = toggle->bend->getModValue() ;
                wfrm->osc.settings.shaper = toggle->shpr->getModValue();
                wfrm->osc.settings.shaperMix = toggle->shpm->getModValue() * (toggle->enbs->getValue() > 0.5);
                wfrm->osc.settings.shaper2 = toggle->shp2->getModValue();
                wfrm->osc.settings.shaper2Mix = toggle->sh2m->getModValue() * (toggle->enbs->getValue() > 0.5);;
                wfrm->osc.settings.pw = toggle->plsw->getModValue();
                wfrm->osc.settings.shaperMorph = toggle->shmr->getModValue();
                wfrm->enableDrive = toggle->enbd->getValue() > 0.5;
                wfrm->drive = toggle->drve->getModValue();
                wfrm->drivegain = toggle->drvg->getModValue();
                wfrm->enableFold = toggle->enbf->getValue() > 0.5;
                wfrm->fold = toggle->flda->getModValue();
                wfrm->bias = toggle->bias->getModValue();
                wfrm->offset = toggle->dcof->getModValue();

                wfrm->setDirty(true);
            }
        }

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            CPoint where2(where);
            where2.offset(-getViewSize().left, -getViewSize().top);
            getTransform().inverse().transform(where2);
            if (wfrm->getViewSize().pointInside(where2))
            {
                if (buttons.isDoubleClick())
                {
                    viewWave ^= true;
                    if (viewWave) wfrm->setViewSize({ 1, 1, getWidth() - 1, getHeight() - 1 });
                    else wfrm->setViewSize({ 140, 35, 140 + 87, 35 + 75 });
                }
            }
            else if (_r != kMouseEventHandled)
            {
                int* _data = new int[1];
                _data[0] = (int)ModSources::Osc1 + index;
                doDrag(DragDescription{ CDropSource::create((void*)_data, sizeof(int) * 1, IDataPackage::Type::kBinary) });
                return kMouseEventHandled;
            }

            return _r;
        }

        void valueChanged(CControl* pControl)
        {
            modulateChange = true;
            onIdle();
            if (pControl == enbl || pControl == toggle->enbd || pControl == toggle->enbf || pControl == toggle->enbn 
                || pControl == toggle->enbr || pControl == toggle->enbs)
            {
                titl->enabled = enbl->getValue() > 0.5;

                bool _clr = enbl->getValue() > 0.5;
                wfrm->enabled = _clr;
                tune->enabled = _clr;
                volm->enabled = _clr;
                detn->enabled = _clr;
                pann->enabled = _clr;

                toggle->dest->enabled = _clr;
                toggle->phse->enabled = _clr;
                toggle->sync->enabled = _clr;
                toggle->plsw->enabled = _clr;
                toggle->wtps->enabled = _clr;
                toggle->bend->enabled = _clr;
                toggle->dcof->enabled = _clr;

                toggle->enbs->enabled = _clr;
                bool _c1 = toggle->enbs->getValue() > 0.5 && _clr;
                toggle->shpl->enabled = _c1;
                toggle->shpr->enabled = _c1;
                toggle->shpm->enabled = _c1;
                toggle->shp2->enabled = _c1;
                toggle->sh2m->enabled = _c1;
                toggle->shmr->enabled = _c1;
                toggle->panel->enabled = _c1;

                toggle->enbn->enabled = _clr;
                bool _c2 = toggle->enbn->getValue() > 0.5 && _clr;
                toggle->nsel->enabled = _c2;
                toggle->nois->enabled = _c2;
                toggle->nscl->enabled = _c2;

                toggle->enbf->enabled = _clr;
                bool _c3 = toggle->enbf->getValue() > 0.5 && _clr;
                toggle->fldl->enabled = _c3;
                toggle->flda->enabled = _c3;
                toggle->bias->enabled = _c3;

                toggle->enbd->enabled = _clr;
                bool _c4 = toggle->enbd->getValue() > 0.5 && _clr;
                toggle->drvl->enabled = _c4;
                toggle->drvg->enabled = _c4;
                toggle->drve->enabled = _c4;

                toggle->enbr->enabled = _clr;
                bool _c5 = toggle->enbr->getValue() > 0.5 && _clr;
                toggle->ftrl->enabled = _c5;
                toggle->fltr->enabled = _c5;
                toggle->freq->enabled = _c5;
                toggle->reso->enabled = _c5;

                toggle->outl->enabled = _clr;


                tune->setDirty(true);
                volm->setDirty(true);
                detn->setDirty(true);
                pann->setDirty(true);
                toggle->dest->setDirty(true);
                toggle->phse->setDirty(true);
                toggle->sync->setDirty(true);
                toggle->plsw->setDirty(true);
                toggle->wtps->setDirty(true);
                toggle->bend->setDirty(true);
                toggle->dcof->setDirty(true);
                toggle->enbs->setDirty(true);
                toggle->shpl->setDirty(true);
                toggle->shpr->setDirty(true);
                toggle->shpm->setDirty(true);
                toggle->shp2->setDirty(true);
                toggle->sh2m->setDirty(true);
                toggle->shmr->setDirty(true);
                toggle->panel->setDirty(true);
                toggle->enbn->setDirty(true);
                toggle->nsel->setDirty(true);
                toggle->nois->setDirty(true);
                toggle->nscl->setDirty(true);
                toggle->fldl->setDirty(true);
                toggle->enbf->setDirty(true);
                toggle->flda->setDirty(true);
                toggle->bias->setDirty(true);
                toggle->drvl->setDirty(true);
                toggle->enbd->setDirty(true);
                toggle->drvg->setDirty(true);
                toggle->drve->setDirty(true);
                toggle->ftrl->setDirty(true);
                toggle->enbr->setDirty(true);
                toggle->fltr->setDirty(true);
                toggle->freq->setDirty(true);
                toggle->reso->setDirty(true);
                toggle->outl->setDirty(true);

                wfrm->setDirty(true);
            }
        }
        BackgroundEffect* bgef;
        OscillatorView(OscillatorToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), toggle(toggle), index(index)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            bgef = new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } };
            addView(bgef);

            setWantsIdle(true);

            enbl = new Knob{ { 169,   3     , 169 + 28,   3 + 28  }, editor };
            tune = new Knob{ {  65,   5     ,  65 + 70,   5 + 96 + 10      }, editor };
            volm = new Knob{ { 232,   5     , 232 + 70,   5 + 96 + 10      }, editor };
            detn = new Knob{ {   5,  16     ,   5 + 60,  21 + 80 + 10      }, editor };
            pann = new Knob{ { 302,  16     , 302 + 60,  21 + 80 + 10       }, editor };

            wfrm = new WaveformView{ { 140, 35, 140 + 87, 35 + 75 } };
            wfrm->index = index;

            titl = new Label{ { 151,   3, 151 + 65,   3 + 30 } };
            titl->setMouseEnabled(false);
            titl->fontsize = 24;
            titl->center = true;
            std::string a;
            a += (char)('A' + index);
            titl->value = a.c_str();

            enbl->setListener(listener);
            tune->setListener(listener);
            pann->setListener(listener);
            detn->setListener(listener);
            volm->setListener(listener);

            enbl->registerControlListener(this);

            enbl->setTag(Params::Enable1 + index);
            tune->setTag(Params::Pitch1 + index);
            pann->setTag(Params::Pan1 + index);
            detn->setTag(Params::Detune1 + index);
            volm->setTag(Params::Volume1 + index);

            tune->name = "Pitch";    pann->name = "Pan";
            tune->min = -24;         pann->min = -50;
            tune->max = 24;          pann->max = 50;
            tune->reset = 0;         pann->reset = 0;
            tune->decimals = 1;      pann->decimals = 1;
            tune->unit = " st";      pann->unit = "pan";
            tune->type = 0;          pann->type = 0;

            detn->name = "Detune";   volm->name = "Gain";
            detn->min = -200;        volm->min = 0;
            detn->max = 200;         volm->max = 1;
            detn->reset = 0;         volm->reset = 1;
            detn->decimals = 0;      volm->decimals = 1;
            detn->unit = " ct";      volm->unit = " dB";
            detn->type = 0;          volm->type = 0;

            enbl->name = "";
            enbl->min = 0;
            enbl->max = 1;
            enbl->reset = 0;
            enbl->decimals = 0;
            enbl->unit = "";
            enbl->type = 3;

            addView(enbl);
            addView(tune);
            addView(pann);
            addView(detn);
            addView(volm);

            addView(titl);

            addView(wfrm);
        }

        ~OscillatorView() 
        {
            enbl->forget();
            tune->forget();
            pann->forget();
            detn->forget();
            volm->forget();
        }
    };

    class OscillatorPart : public CViewContainer
    {
    public:
        OscillatorView* views[4];
        OscillatorToggle* toggles[4];

        CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
        {
            auto _r = CViewContainer::onMouseDown(where, buttons);
            CPoint where2(where);
            where2.offset(-getViewSize().left, -getViewSize().top);
            getTransform().inverse().transform(where2);
            void* selected = nullptr;
            for (auto& o : views)
            {
                if (o->getViewSize().pointInside(where2))
                {
                    o->Select();
                    selected = o;
                    break;
                }
            }

            if (selected)
            {
                for (auto& o : views)
                    if (o != selected) o->Unselect();
            }

            return _r;
        }

        OscillatorPart(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });

            toggles[0] = new OscillatorToggle{ { 5, 168, 5 + 670 + 65, 168 + 224 }, 0, listener, editor };
            toggles[1] = new OscillatorToggle{ { 5, 168, 5 + 670 + 65, 168 + 224 }, 1, listener, editor };
            toggles[2] = new OscillatorToggle{ { 5, 168, 5 + 670 + 65, 168 + 224 }, 2, listener, editor };
            toggles[3] = new OscillatorToggle{ { 5, 168, 5 + 670 + 65, 168 + 224 }, 3, listener, editor };

            toggles[0]->setVisible(false);
            toggles[1]->setVisible(false);
            toggles[2]->setVisible(false);
            toggles[3]->setVisible(false);

            views[0] = new OscillatorView{ toggles[0], {   5,  55,   5 + 335 + 30,  55 + 115 }, 0, listener, editor };
            views[1] = new OscillatorView{ toggles[1], { 375,  55, 375 + 335 + 30,  55 + 115 }, 1, listener, editor };
            views[2] = new OscillatorView{ toggles[2], {   5, 390,   5 + 335 + 30, 390 + 115 }, 2, listener, editor };
            views[3] = new OscillatorView{ toggles[3], { 375, 390, 375 + 335 + 30, 390 + 115 }, 3, listener, editor };

            for (auto& i : views)
            {
                i->toggle->phse->registerControlListener(i);
                i->toggle->wtps->registerControlListener(i);
                i->toggle->sync->registerControlListener(i);
                i->toggle->bend->registerControlListener(i);
                i->toggle->shpr->registerControlListener(i);
                i->toggle->shpm->registerControlListener(i);
                i->toggle->shp2->registerControlListener(i);
                i->toggle->sh2m->registerControlListener(i);
                i->toggle->plsw->registerControlListener(i);
                i->toggle->enbs->registerControlListener(i);
                i->toggle->enbd->registerControlListener(i);
                i->toggle->enbf->registerControlListener(i);
                i->toggle->enbn->registerControlListener(i);
                i->toggle->enbr->registerControlListener(i);
                i->toggle->shmr->registerControlListener(i);
                i->toggle->flda->registerControlListener(i);
                i->toggle->bias->registerControlListener(i);
                i->toggle->drve->registerControlListener(i);
                i->toggle->drvg->registerControlListener(i);
                i->toggle->dcof->registerControlListener(i);

                editor->controller->wakeupCalls.emplace_back(Params::WTPos1 + i->index, i->toggle->wtps->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Sync1 + i->index, i->toggle->sync->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Phase1 + i->index, i->toggle->phse->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::PulseW1 + i->index, i->toggle->plsw->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Bend1 + i->index, i->toggle->bend->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Shaper1 + i->index, i->toggle->shpr->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::ShaperMix1 + i->index, i->toggle->shpm->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Shaper21 + i->index, i->toggle->shp2->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Shaper2Mix1 + i->index, i->toggle->sh2m->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::ShaperMorph1 + i->index, i->toggle->shmr->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::DriveAmt1 + i->index, i->toggle->drve->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::DriveGain1 + i->index, i->toggle->drvg->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Bias1 + i->index, i->toggle->bias->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::Fold1 + i->index, i->toggle->flda->modulation, i->modulateChange);
                editor->controller->wakeupCalls.emplace_back(Params::DCOff1 + i->index, i->toggle->dcof->modulation, i->modulateChange);
            }

            views[0]->Select();

            addView(views[0]);
            addView(views[1]);
            addView(views[2]);
            addView(views[3]);

            addView(toggles[0]);
            addView(toggles[1]);
            addView(toggles[2]);
            addView(toggles[3]);
        }
    };
}