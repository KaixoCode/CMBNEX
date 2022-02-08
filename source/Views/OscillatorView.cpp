#include "Views/OscillatorView.hpp"

namespace Kaixo
{

    OscillatorToggle::OscillatorToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size), index(index)
    {
        setBackgroundColor({ 0, 0, 0, 0 });

        double y = 2;

        addView(new BackgroundEffect{ {.size = { 0, 5 + y, getWidth(), getHeight() - 5 - y } } });

        CColor brdclr = Colors::MainBack;
        
        switch (index) {
        case 0: addView(new BackgroundEffect{ {.size = { 0, 0, getWidth() / 2 - 2, 8 }, .sides = 2 } }); break;
        case 1: addView(new BackgroundEffect{ {.size = { getWidth() / 2 + 3, 0, getWidth(), 8 }, .sides = 2 } }); break;
        case 2: addView(new BackgroundEffect{ {.size = { 0, getHeight() - 8, getWidth() / 2 - 2, getHeight() }, .sides = 2 } }); break;
        case 3: addView(new BackgroundEffect{ {.size = { getWidth() / 2 + 3, getHeight() - 8, getWidth(), getHeight() }, .sides = 2 } }); break;
        }

        addView(new BackgroundEffect{ {.size = {   5, 10 + y,   5 + 179, 10 + 200 + y }, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 189, 10 + y, 189 + 206, 10 + 200 + y}, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 400, 10 + y, 400 + 70, 10 + 200 + y }, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 475, 10 + y, 475 + 70, 10 + 200 + y }, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 550, 10 + y, 550 + 70, 10 + 200 + y }, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 625, 10 + y, 625 + 70, 10 + 200 + y }, .dark = true } });
        addView(new BackgroundEffect{ {.size = { 700, 10 + y, 700 + 30, 10 + 200 + y }, .dark = true } });

        outl = new Label{ {
            .size{ 699,  13 + y, 699 + 30,  13 + 18 + y  },
            .value = "Out", .center = true, .fontsize = 14,
        } };

        dest = new Parameter{ {
            .tag = Params::DestA + index,
            .editor = editor,
            .size = { 704,  39 + y, 704 + 20,  39 + 169 + y },
            .type = Parameter::MULTIGROUP,
            .parts = { "I", "II", "III", "IV", "V", "VI", "O" },
            .vertical = true, .padding = 4,
        } };

        rphs = new Parameter{ {
            .tag = Params::RandomPhase1 + index,
            .editor = editor,
            .size = {   8,  16 + y,   8 + 14,  16 + 14 + y },
            .type = Parameter::BUTTON, .dark = true, .name = "R",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = "",
        } };

        phse = new Parameter{ {
            .tag = Params::Phase1 + index,
            .editor = editor,
            .size = {  10,  15 + y,  10 + 55,  15 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "   Phase",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        plsw = new Parameter{ {
            .tag = Params::PulseW1 + index,
            .editor = editor,
            .size = {  67,  15 + y,  67 + 55,  15 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "PW",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        bend = new Parameter{ {
            .tag = Params::Bend1 + index,
            .editor = editor,
            .size = { 124,  15 + y, 124 + 55,  15 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Bend",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        wtps = new Parameter{ {
            .tag = Params::WTPos1 + index,
            .editor = editor,
            .size = {  10, 115 + y,  10 + 55, 115 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "WTP",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        sync = new Parameter{ {
            .tag = Params::Sync1 + index,
            .editor = editor,
            .size = {  67, 115 + y,  67 + 55, 115 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Sync",
            .min = 100, .max = 800, .reset = 100, .decimals = 1,
            .unit = " %",
        } };

        dcof = new Parameter{ {
            .tag = Params::DCOff1 + index,
            .editor = editor,
            .size = { 124, 115 + y, 124 + 55, 115 + 90 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Offset",
            .min = -1, .max = 1, .reset = 0, .decimals = 2,
            .unit = "",
        } };

        shpl = new Label{ {
            .size = { 209,  13 + y, 209 + 90,  13 + 18 + y },
            .value = "Waveshaper", .center = false, .fontsize = 14
        } };

        enbs = new Parameter{ {
            .tag = Params::ENBShaper1 + index,
            .editor = editor,
            .size = { 193,  14 + y, 193 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true
        } };        

        frzl = new Label{ {
            .size = { 340,  13 + y, 340 + 50,  13 + 18 + y },
            .value = "Freeze", .center = false, .fontsize = 14
        } };

        frez = new Parameter{ {
            .tag = Params::ShaperFreez1 + index,
            .editor = editor,
            .size = { 324,  14 + y, 324 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true
        } };

        shpr = new Parameter{ {
            .tag = Params::Shaper1 + index,
            .editor = editor,
            .size = { 194,  35 + y, 194 + 65,  35 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "SHP-X",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        shpm = new Parameter{ {
            .tag = Params::ShaperMix1 + index,
            .editor = editor,
            .size = { 259,  35 + y, 259 + 65,  35 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Mix-X",
            .min = 0, .max = 100, .reset = 50, .decimals = 1,
            .unit = " %",
        } };

        shp2 = new Parameter{ {
            .tag = Params::Shaper21 + index,
            .editor = editor,
            .size = { 324, 152 + y, 324 + 65, 152 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "SHP-Y",
            .min = -100, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        sh2m = new Parameter{ {
            .tag = Params::Shaper2Mix1 + index,
            .editor = editor,
            .size = { 324,  95 + y, 324 + 65,  95 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Mix-Y",
            .min = 0, .max = 100, .reset = 50, .decimals = 1,
            .unit = " %",
        } };

        shmr = new Parameter{ {
            .tag = Params::ShaperMorph1 + index,
            .editor = editor,
            .size = { 324,  35 + y, 324 + 65,  35 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Morph",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        panel = new XYPanel{ shpr, shp2, { 194, 93 + y, 194 + 125, 93 + 112 + y } };

        fldl = new Label{ {
            .size{ 420,  13 + y, 420 + 40,  13 + 18 + y },
            .value = "Fold", .center = false, .fontsize = 14
        } };

        enbf = new Parameter{ {
            .tag = Params::ENBFold1 + index,
            .editor = editor,
            .size = { 404,  14 + y, 404 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true,
        } };

        flda = new Parameter{ {
            .tag = Params::Fold1 + index,
            .editor = editor,
            .size = { 405,  52 + y, 405 + 60,  52 + 95 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Gain",
            .min = 1, .max = 15.9, .reset = 1, .decimals = 1,
            .unit = " dB",
        } };

        bias = new Parameter{ {
            .tag = Params::Bias1 + index,
            .editor = editor,
            .size = { 405, 152 + y, 405 + 65, 152 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Bias",
            .min = -1, .max = 1, .reset = 0, .decimals = 2,
            .unit = "",
        } };

        nsel = new Label{ {
            .size = { 495,  13 + y, 495 + 40,  13 + 18 + y  },
            .value = "Noise", .center = false, .fontsize = 14,
        } };

        enbn = new Parameter{ {
            .tag = Params::ENBNoise1 + index,
            .editor = editor,
            .size = { 479,  14 + y, 479 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true
        } };

        nois = new Parameter{ {
            .tag = Params::Noise1 + index,
            .editor = editor,
            .size = { 480,  52 + y, 480 + 60,  52 + 95 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Amount",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        nscl = new Parameter{ {
            .tag = Params::Color1 + index,
            .editor = editor,
            .size = { 480, 152 + y, 480 + 65, 152 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Color",
            .min = -100, .max = 100, .reset = 0, .decimals = 0,
            .unit = "",
        } };

        drvl = new Label{ {
            .size = { 570,  13 + y, 570 + 40,  13 + 18 + y },
            .value = "Drive", .center = false, .fontsize = 14,
        } };

        enbd = new Parameter{ {
            .tag = Params::ENBDrive1 + index,
            .editor = editor,
            .size = { 554,  14 + y, 554 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true,
        } };

        drvg = new Parameter{ {
            .tag = Params::DriveGain1 + index,
            .editor = editor,
            .size = { 555,  52 + y, 555 + 60,  52 + 95 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Gain",
            .min = 1, .max = 4, .reset = 1, .decimals = 1,
            .unit = " dB",
        } };

        drve = new Parameter{ {
            .tag = Params::DriveAmt1 + index,
            .editor = editor,
            .size = { 555, 152 + y, 555 + 65, 152 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Shape",
            .min = 0, .max = 100, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        ftrl = new Label{ {
            .size { 645,  13 + y, 645 + 40,  13 + 18 + y },
            .value = "Filter", .center = false, .fontsize = 14,
        } };

        enbr = new Parameter{ {
            .tag = Params::ENBFilter1 + index,
            .editor = editor,
            .size = { 629,  14 + y, 629 + 14,  14 + 14 + y },
            .type = Parameter::BUTTON, .dark = true
        } };

        fltr = new Parameter{ {
            .tag = Params::Filter1 + index,
            .editor = editor,
            .size = { 629,  32 + y, 629 + 63,  32 + 17 + y },
            .type = Parameter::GROUP, .dark = true,
            .parts = { "L", "H", "B" }, .vertical = false, .padding = 4,
        } };

        freq = new Parameter{ {
            .tag = Params::Freq1 + index,
            .editor = editor,
            .size = { 630,  52 + y, 630 + 60,  52 + 95 + y },
            .type = Parameter::KNOB, .dark = true, .name = "Freq",
            .min = 30, .max = 22000, .reset = 22000, .decimals = 1,
            .unit = " Hz",
        } };

        reso = new Parameter{ {
            .tag = Params::Reso1 + index,
            .editor = editor,
            .size = { 630, 152 + y, 630 + 65, 152 + 53 + y },
            .type = Parameter::NUMBER, .dark = true, .name = "Reso",
            .min = 0, .max = 125, .reset = 0, .decimals = 1,
            .unit = " %",
        } };

        addView(phse); addView(rphs); addView(sync); addView(plsw); addView(wtps); addView(bend); 
        addView(dcof); addView(shpl); addView(enbs); addView(shpr); addView(shpm); addView(shp2); 
        addView(sh2m); addView(shmr); addView(nsel); addView(enbn); addView(nois); addView(nscl); 
        addView(fldl); addView(enbf); addView(flda); addView(bias); addView(drvl); addView(enbd); 
        addView(drvg); addView(drve); addView(ftrl); addView(enbr); addView(fltr); addView(freq); 
        addView(reso); addView(dest); addView(outl); addView(panel);addView(frez); addView(frzl);
    }

    OscillatorToggle::~OscillatorToggle()
    {
        phse->forget(); sync->forget(); plsw->forget(); wtps->forget(); bend->forget(); dcof->forget(); 
        enbs->forget(); shpr->forget(); shpm->forget(); shp2->forget(); sh2m->forget(); shmr->forget(); 
        enbn->forget(); nois->forget(); nscl->forget(); enbf->forget(); flda->forget(); bias->forget(); 
        enbd->forget(); drvg->forget(); drve->forget(); enbr->forget(); fltr->forget(); freq->forget(); 
        reso->forget(); dest->forget(); frez->forget();
    }

    void OscillatorView::Select()
    {
        if (!selected)
            toggle->setVisible(true);

        selected = true;
    }

    void OscillatorView::Unselect()
    {
        if (selected)
            toggle->setVisible(false);

        selected = false;
    }

    void OscillatorView::onIdle()
    {
        if (!wtloaded)
        {
            wfrm->setDirty(true);
            
            if (Wavetables::basicLoaded())
                wtloaded = true;
        }

        if (modulateChange)
        {
            modulateChange = false;

            wfrm->phase = toggle->phse->getModValue();
            wfrm->osc.settings.wtpos = toggle->wtps->getModValue();
            wfrm->osc.settings.sync = toggle->sync->getModValue();
            wfrm->osc.settings.bend = toggle->bend->getModValue();
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

    CMouseEventResult OscillatorView::onMouseDown(CPoint& where, const CButtonState& buttons)
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
                if (viewWave) 
                    wfrm->setViewSize({ 5, 5, getWidth() - 5, getHeight() - 5 }), 
                    enbl->setVisible(false), 
                    pann->setVisible(false), 
                    volm->setVisible(false), 
                    tune->setVisible(false), 
                    detn->setVisible(false);
                else 
                    wfrm->setViewSize({ 140, 35, 140 + 87, 35 + 75 }), 
                    enbl->setVisible(true), 
                    pann->setVisible(true), 
                    volm->setVisible(true), 
                    tune->setVisible(true), 
                    detn->setVisible(true);
            }
        }

        return _r;
    }

    void OscillatorView::valueChanged(CControl* pControl)
    {
        modulateChange = true;
        if (pControl == enbl || pControl == toggle->enbd || pControl == toggle->enbf || pControl == toggle->enbn
            || pControl == toggle->enbr || pControl == toggle->enbs)
        {
            titl->settings.enabled = enbl->getValue() > 0.5;

            bool _clr = enbl->getValue() > 0.5;
            wfrm->enabled = _clr;
            tune->settings.enabled = _clr;
            volm->settings.enabled = _clr;
            detn->settings.enabled = _clr;
            pann->settings.enabled = _clr;

            toggle->dest->settings.enabled = _clr;
            toggle->phse->settings.enabled = _clr;
            toggle->sync->settings.enabled = _clr;
            toggle->plsw->settings.enabled = _clr;
            toggle->wtps->settings.enabled = _clr;
            toggle->bend->settings.enabled = _clr;
            toggle->dcof->settings.enabled = _clr;

            toggle->enbs->settings.enabled = _clr;
            bool _c1 = toggle->enbs->getValue() > 0.5 && _clr;
            toggle->frzl->settings.enabled = _c1;
            toggle->frez->settings.enabled = _c1;
            toggle->shpl->settings.enabled = _c1;
            toggle->shpr->settings.enabled = _c1;
            toggle->shpm->settings.enabled = _c1;
            toggle->shp2->settings.enabled = _c1;
            toggle->sh2m->settings.enabled = _c1;
            toggle->shmr->settings.enabled = _c1;
            toggle->panel->enabled = _c1;

            toggle->enbn->settings.enabled = _clr;
            bool _c2 = toggle->enbn->getValue() > 0.5 && _clr;
            toggle->nsel->settings.enabled = _c2;
            toggle->nois->settings.enabled = _c2;
            toggle->nscl->settings.enabled = _c2;

            toggle->enbf->settings.enabled = _clr;
            bool _c3 = toggle->enbf->getValue() > 0.5 && _clr;
            toggle->fldl->settings.enabled = _c3;
            toggle->flda->settings.enabled = _c3;
            toggle->bias->settings.enabled = _c3;

            toggle->enbd->settings.enabled = _clr;
            bool _c4 = toggle->enbd->getValue() > 0.5 && _clr;
            toggle->drvl->settings.enabled = _c4;
            toggle->drvg->settings.enabled = _c4;
            toggle->drve->settings.enabled = _c4;

            toggle->enbr->settings.enabled = _clr;
            bool _c5 = toggle->enbr->getValue() > 0.5 && _clr;
            toggle->ftrl->settings.enabled = _c5;
            toggle->fltr->settings.enabled = _c5;
            toggle->freq->settings.enabled = _c5;
            toggle->reso->settings.enabled = _c5;

            toggle->outl->settings.enabled = _clr;

            tune->setDirty(true); volm->setDirty(true); detn->setDirty(true); pann->setDirty(true);
            toggle->dest->setDirty(true); toggle->phse->setDirty(true); toggle->sync->setDirty(true); toggle->plsw->setDirty(true); 
            toggle->wtps->setDirty(true); toggle->bend->setDirty(true); toggle->dcof->setDirty(true); toggle->enbs->setDirty(true); 
            toggle->shpl->setDirty(true); toggle->shpr->setDirty(true); toggle->shpm->setDirty(true); toggle->shp2->setDirty(true); 
            toggle->sh2m->setDirty(true); toggle->shmr->setDirty(true); toggle->panel->setDirty(true); toggle->enbn->setDirty(true); 
            toggle->nsel->setDirty(true); toggle->nois->setDirty(true); toggle->nscl->setDirty(true); toggle->fldl->setDirty(true); 
            toggle->enbf->setDirty(true); toggle->flda->setDirty(true); toggle->bias->setDirty(true); toggle->drvl->setDirty(true); 
            toggle->enbd->setDirty(true); toggle->drvg->setDirty(true); toggle->drve->setDirty(true); toggle->ftrl->setDirty(true); 
            toggle->enbr->setDirty(true); toggle->fltr->setDirty(true); toggle->freq->setDirty(true); toggle->reso->setDirty(true); 
            toggle->outl->setDirty(true); toggle->frez->setDirty(true); toggle->frzl->setDirty(true);

            wfrm->setDirty(true);
        }
        onIdle();
    }

    OscillatorView::OscillatorView(OscillatorToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor)
        : CViewContainer(size), toggle(toggle), index(index)
    {   // Background effect instead of normal background.
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });

        setWantsIdle(true); // We want idle updates!

        wfrm = new WaveformView{ { 140, 35, 140 + 87, 35 + 75 } };

        enbl = new Parameter{ {
            .tag = Params::Enable1 + index,
            .editor = editor,  .listener = this,
            .size{ 169, 3 , 169 + 28, 3 + 28 },
            .type = Parameter::BUTTON, .name = "",
        } };

        tune = new Parameter{ {
            .tag = Params::Pitch1 + index,
            .editor = editor,
            .size{  65,   5 ,  65 + 70,   5 + 96 + 10 },
            .type = Parameter::KNOB, .name = "Pitch",
            .min = -24., .max = 24., .reset = 0., .decimals = 1,
            .unit = " st",
        } };

        volm = new Parameter{ {
            .tag = Params::Volume1 + index,
            .editor = editor,
            .size{ 232,   5 , 232 + 70,   5 + 96 + 10 },
            .type = Parameter::KNOB, .name = "Gain",
            .min = 0., .max = 1., .reset = 1., .decimals = 1,
            .unit = " dB",
        } };

        detn = new Parameter{ {
            .tag = Params::Detune1 + index,
            .editor = editor,
            .size{   5,  16 ,   5 + 60,  21 + 80 + 10 },
            .type = Parameter::KNOB, .name = "Detune",
            .min = -200, .max = 200, .reset = 0, .decimals = 1,
            .unit = " ct",
        } };

        pann = new Parameter{ {
            .tag = Params::Pan1 + index,
            .editor = editor,
            .size{ 302,  16 , 302 + 60,  21 + 80 + 10 },
            .type = Parameter::KNOB, .name = "Pan",
            .min = -50, .max = 50, .reset = 0, .decimals = 1,
            .unit = "pan",
        } };

        char a[2]{ (char)('A' + index), '\0' };
        titl = new Label{ {
            .size = { 151,   3, 151 + 65,   3 + 30 },
            .value = a, .center = true, .fontsize = 24,
        } };
        titl->setMouseEnabled(false);

        addView(enbl); addView(tune); addView(pann); addView(detn); 
        addView(volm); addView(titl); addView(wfrm);
    }

    OscillatorView::~OscillatorView()
    {
        enbl->forget(); tune->forget(); pann->forget(); detn->forget();
        volm->forget();
    }


    CMouseEventResult OscillatorPart::onMouseDown(CPoint& where, const CButtonState& buttons)
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

    OscillatorPart::OscillatorPart(const CRect& size, IControlListener* listener, MyEditor* editor)
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
            i->toggle->phse->registerControlListener(i); i->toggle->wtps->registerControlListener(i); i->toggle->sync->registerControlListener(i); 
            i->toggle->bend->registerControlListener(i); i->toggle->shpr->registerControlListener(i); i->toggle->shpm->registerControlListener(i); 
            i->toggle->shp2->registerControlListener(i); i->toggle->sh2m->registerControlListener(i); i->toggle->plsw->registerControlListener(i); 
            i->toggle->enbs->registerControlListener(i); i->toggle->enbd->registerControlListener(i); i->toggle->enbf->registerControlListener(i); 
            i->toggle->enbn->registerControlListener(i); i->toggle->enbr->registerControlListener(i); i->toggle->shmr->registerControlListener(i); 
            i->toggle->flda->registerControlListener(i); i->toggle->bias->registerControlListener(i); i->toggle->drve->registerControlListener(i); 
            i->toggle->drvg->registerControlListener(i); i->toggle->dcof->registerControlListener(i);

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
}