#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"
#include "Components/BackgroundEffect.hpp"

namespace Kaixo
{
    class CombineToggle : public CViewContainer
    {
    public:
        Label* ftrl = nullptr; // Filter title
        Parameter* enbr = nullptr; // Enable Filter
        Parameter* fltr = nullptr; // Filter type
        Parameter* freq = nullptr; // Filter Frequency  
        Parameter* reso = nullptr; // Filter Resonance  

        Parameter* addm = nullptr;
        Parameter* minm = nullptr;
        Parameter* mulm = nullptr;
        Parameter* ponm = nullptr;
        Parameter* maxm = nullptr;
        Parameter* modm = nullptr;
        Parameter* andm = nullptr;
        Parameter* inlm = nullptr;
        Parameter* orrm = nullptr;
        Parameter* xorm = nullptr;

        Label* fldl = nullptr;
        Parameter* enbf = nullptr;
        Parameter* fold = nullptr;
        Parameter* bias = nullptr;

        Label* drvl = nullptr;
        Parameter* enbd = nullptr;
        Parameter* driv = nullptr;
        Parameter* dgai = nullptr;

        Parameter* dest = nullptr;

        Parameter* dcof = nullptr;

        Label* dcol = nullptr;
        Label* outl = nullptr;

        int index = 0;

        CombineToggle(const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), index(index)
        {
            setBackgroundColor({ 0, 0, 0, 0 });

            double x = 2;

            auto* v = new BackgroundEffect{ { 5 + x, 0, getWidth() - 5 - x, getHeight() } };
            addView(v);

            CColor brdclr = BorderHover;
            if (index == 0)
            {
                auto* v = new BackgroundEffect{ { 0,   0, 8,   0 + 67 } };
                v->sides = 1;
                addView(v);

                outl = new Label{ {
                    .size = { 379 + x,  60, 379 + 30 + x,  60 + 20 },
                    .value = "Out", .center = true, .fontsize = 14,
                } };

                dest = new Parameter{ {
                    .editor = editor,
                    .size = { 384 + x,  83, 384 + 20 + x,  83 + 97 + 24 },
                    .type = Parameter::MULTIGROUP,
                    .parts = { "III", "IV", "V", "VI", "O" }, .vertical = true, .padding = 4,
                } };

                dest->setListener(listener); dest->setTag(Params::DestX);
            }
            else if (index == 1)
            {
                auto* v = new BackgroundEffect{ { 0,  72, 8,  72 + 67 } };
                v->sides = 1;
                addView(v);

                outl = new Label{ {
                    .size = { 379 + x,  60, 379 + 30 + x,  60 + 20 },
                    .value = "Out", .center = true, .fontsize = 14,
                } };

                dest = new Parameter{ {
                    .editor = editor,
                    .size = { 384 + x, 131, 384 + 20 + x, 131 + 49 + 24 },
                    .type = Parameter::MULTIGROUP,
                    .parts = { "V", "VI", "O" }, .vertical = true, .padding = 4,
                } };

                dest->setListener(listener); dest->setTag(Params::DestY);
            }
            else if (index == 2)
            {
                auto* v = new BackgroundEffect{ { 0, 144, 8, 144 + 67 } };
                v->sides = 1;
                addView(v);

                outl = nullptr;
                dest = nullptr;
            }

            auto* v1 = new BackgroundEffect{ {  10 + x, 5,  10 + 140 + x, 5 + 201 }, true };
            auto* v2 = new BackgroundEffect{ { 155 + x, 5, 155 + 70 + x, 5 + 201 }, true };
            auto* v3 = new BackgroundEffect{ { 230 + x, 5, 230 + 70 + x, 5 + 201 }, true };
            auto* v4 = new BackgroundEffect{ { 305 + x, 5, 305 + 70 + x, 5 + 201 }, true };
            auto* v5 = new BackgroundEffect{ { 380 + x, 5, 380 +  30 + x, 5 + 201 }, true };
            addView(v1); addView(v2); addView(v3); addView(v4); addView(v5);

            dcol = new Label{ {
                .size = { 379 + x,  10, 379 + 30 + x,  10 + 20 },
                .value = "DC", .center = true, .fontsize = 14,
            } };

            dcof = new Parameter{ {
                .tag = Params::DCX + index,
                .editor = editor,
                .size = { 384 + x,  30, 384 + 20 + x,  30 + 20 },
                .type = Parameter::BUTTON, .dark = true,
            } };

            ftrl = new Label{ {
                .size = { 325 + x,  8, 325 + 40 + x,   8 + 18 },
                .value = "Filter", .center = false, .fontsize = 14,
            } };

            enbr = new Parameter{ { 
                .tag = Params::ENBFilterX + index,
                .editor = editor,
                .size = { 309 + x,   9, 309 + 14 + x,   9 + 14 },
                .type = Parameter::BUTTON, .dark = true,
            } };

            fltr = new Parameter{ {
                .tag = Params::FilterX + index,
                .editor = editor,
                .size = { 309 + x,  27, 309 + 63 + x,  27 + 17 },
                .type = Parameter::GROUP, .dark = true,
                .parts = { "L", "H", "B" }, .padding = 4,
            } };

            freq = new Parameter{ {
                .tag = Params::FreqX + index,
                .editor = editor,
                .size = { 310 + x,  47, 310 + 60 + x,  47 + 95 },
                .type = Parameter::KNOB, .dark = true, .name = "Freq",
                .min = 30, .max = 22000, .reset = 22000, .decimals = 1,
                .unit = " Hz",
            } };

            reso = new Parameter{ {
                .tag = Params::ResoX + index,
                .editor = editor,
                .size = { 310 + x, 148, 310 + 65 + x, 148 + 53 }, 
                .type = Parameter::NUMBER, .dark = true, .name = "Reso",
                .min = 0, .max = 125, .reset = 0, .decimals = 1,
                .unit = " %",
            } };

            drvl = new Label{ {
                .size = { 250 + x,  8, 250 + 40 + x,   8 + 18 },
                .value = "Drive", .center = false, .fontsize = 14,
            } };

            enbd = new Parameter{ { 
                .tag = Params::ENBDriveX + index,
                .editor = editor,
                .size = { 234 + x,   9, 234 + 14 + x,   9 + 14 },
                .type = Parameter::BUTTON, .dark = true
            } };

            dgai = new Parameter{ {
                .tag = Params::DriveGainX + index,
                .editor = editor,
                .size = { 235 + x,  47, 235 + 60 + x,  47 + 95 },
                .type = Parameter::KNOB, .dark = true, .name = "Gain",
                .min = 1, .max = 4, .reset = 1, .decimals = 1,
                .unit = " dB",
            } };

            driv = new Parameter{ {
                .tag = Params::DriveAmtX + index,
                .editor = editor,
                .size = { 235 + x, 148, 235 + 65 + x, 148 + 53 },
                .type = Parameter::NUMBER, .dark = true, .name = "Shape",
                .min = 0, .max = 100, .reset = 0, .decimals = 1,
                .unit = " %",
            } };

            fldl = new Label{ {
                .size = { 175 + x,  8, 175 + 40 + x,   8 + 18 },
                .value = "Fold", .center = false, .fontsize = 14,
            } };

            enbf = new Parameter{ { 
                .tag = Params::ENBFoldX + index,
                .editor = editor,
                .size = { 159 + x,   9, 159 + 14 + x,   9 + 14 }, 
                .type = Parameter::BUTTON, .dark = true
            } };

            fold = new Parameter{ {
                .tag = Params::FoldX + index,
                .editor = editor,
                .size = { 160 + x,  47, 160 + 60 + x,  47 + 95 },
                .type = Parameter::KNOB, .dark = true, .name = "Gain",
                .min = 1, .max = 15.9, .reset = 1, .decimals = 1,
                .unit = " dB",
            } };

            bias = new Parameter{ {
                .tag = Params::BiasX + index,
                .editor = editor,
                .size = { 160 + x, 148, 160 + 65 + x, 148 + 53 },
                .type = Parameter::NUMBER, .dark = true, .name = "Bias",
                .min = -1, .max = 1, .reset = 0, .decimals = 2,
                .unit = "",
            } };
            
            minm = new Parameter{ { 
                .tag = Params::MinMixX + index,
                .editor = editor,
                .size = { 15 + x,  10 - 0 * 1, 15 + 65 + x,  10 + 35 - 0 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Min",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            mulm = new Parameter{ {
                .tag = Params::MultMixX + index,
                .editor = editor,
                .size = { 15 + x,  50 - 1 * 1, 15 + 65 + x,  50 + 35 - 1 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Mult",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            ponm = new Parameter{ {
                .tag = Params::PongMixX + index,
                .editor = editor,
                .size = { 15 + x,  90 - 2 * 1, 15 + 65 + x,  90 + 35 - 2 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Pong",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            maxm = new Parameter{ {
                .tag = Params::MaxMixX + index,
                .editor = editor,
                .size = { 15 + x, 130 - 3 * 1, 15 + 65 + x, 130 + 35 - 3 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Max",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            modm = new Parameter{ {
                .tag = Params::ModMixX + index,
                .editor = editor,
                .size = { 15 + x, 170 - 4 * 1, 15 + 65 + x, 170 + 35 - 4 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Mod",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            andm = new Parameter{ {
                .tag = Params::AndMixX + index,
                .editor = editor,
                .size = { 80 + x,  10 - 0 * 1, 80 + 65 + x,  10 + 35 - 0 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "And",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            inlm = new Parameter{ {
                .tag = Params::InlvMixX + index,
                .editor = editor,
                .size = { 80 + x,  50 - 1 * 1, 80 + 65 + x,  50 + 35 - 1 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Inlv",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            orrm = new Parameter{ {
                .tag = Params::OrMixX + index,
                .editor = editor,
                .size = { 80 + x,  90 - 2 * 1, 80 + 65 + x,  90 + 35 - 2 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "Or",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            xorm = new Parameter{ {
                .tag = Params::XOrMixX + index,
                .editor = editor,
                .size = { 80 + x, 130 - 3 * 1, 80 + 65 + x, 130 + 35 - 3 * 1 },
                .type = Parameter::SMALLSLIDER, .dark = true, .name = "XOr",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            addm = new Parameter{ {
                .tag = Params::AddMixX + index,
                .editor = editor,
                .size = { 80 + x, 170 - 4 * 1, 80 + 65 + x, 170 + 35 - 4 * 1 }, 
                .type = Parameter::SMALLSLIDER, .dark = true , .name = "Add",
                .min = 0, .max = 1, .reset = 0, .decimals = 1,
                .unit = "",
            } };

            addView(ftrl);
            addView(enbr);
            addView(fltr);
            addView(freq);
            addView(reso);

            addView(minm);
            addView(mulm);
            addView(ponm);
            addView(maxm);
            addView(modm);
            addView(andm);
            addView(inlm);
            addView(orrm);
            addView(xorm);
            addView(addm);

            addView(drvl);
            addView(enbd);
            addView(dgai);
            addView(driv);
            addView(fldl);
            addView(enbf);
            addView(fold);
            addView(bias);

            if (dest) addView(dest);
            if (outl) addView(outl);

            addView(dcof);
            addView(dcol);
        }

        ~CombineToggle()
        {
            enbr->forget();
            fltr->forget();
            freq->forget();
            reso->forget();
            minm->forget();
            mulm->forget();
            ponm->forget();
            maxm->forget();
            modm->forget();
            andm->forget();
            inlm->forget();
            orrm->forget();
            xorm->forget();
            addm->forget();
            enbd->forget();
            dgai->forget();
            driv->forget();
            enbf->forget();
            fold->forget();
            bias->forget();
            if (dest) dest->forget();
            dcof->forget();
        }
    };

    class CombineView : public CViewContainer, public IControlListener
    {
    public:
        int index = 0;
        Parameter* gmix;
        Parameter* gain;
        Parameter* pgai;
        Label* titl;
        Label* inp1;
        Label* inp2;

        CombineToggle* toggle;

        bool selected = false;

        void Select()
        {
            if (!selected)
                toggle->setVisible(true);

            selected = true;
        }

        void Unselect()
        {
            if (selected)
                toggle->setVisible(false);

            selected = false;
        }

        void valueChanged(CControl* pControl) override
        {
            bool _c1 = toggle->enbf->getValue() > 0.5;
            toggle->fldl->settings.enabled = _c1;
            toggle->fold->settings.enabled = _c1;
            toggle->bias->settings.enabled = _c1;

            bool _c2 = toggle->enbd->getValue() > 0.5;
            toggle->driv->settings.enabled = _c2;
            toggle->drvl->settings.enabled = _c2;
            toggle->dgai->settings.enabled = _c2;

            bool _c3 = toggle->enbr->getValue() > 0.5;
            toggle->ftrl->settings.enabled = _c3;
            toggle->fltr->settings.enabled = _c3;
            toggle->freq->settings.enabled = _c3;
            toggle->reso->settings.enabled = _c3;

            toggle->fldl->setDirty(true);
            toggle->fold->setDirty(true);
            toggle->bias->setDirty(true);
            toggle->driv->setDirty(true);
            toggle->drvl->setDirty(true);
            toggle->dgai->setDirty(true);
            toggle->ftrl->setDirty(true);
            toggle->fltr->setDirty(true);
            toggle->freq->setDirty(true);
            toggle->reso->setDirty(true);
        }

        BackgroundEffect* bgef;

        CombineView(CombineToggle* toggle, const CRect& size, int index, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), toggle(toggle), index(index)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            bgef = new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } };
            addView(bgef);

            toggle->enbd->registerControlListener(this);
            toggle->enbf->registerControlListener(this);
            toggle->enbr->registerControlListener(this);
            
            pgai = new Parameter{ { 
                .tag = Params::PreGainX + index,
                .editor = editor,
                .size = {  35,  7,  35 + 65,  7 + 53 },
                .type = Parameter::NUMBER, .name = "Pre-Gain",
                .min = 0, .max = 1, .reset = 1, .decimals = 1,
                .unit = " dB",
            } };

            gmix = new Parameter{ {
                .tag = Params::MixX + index,
                .editor = editor,
                .size = { 100,  7, 100 + 65,  7 + 53 },
                .type = Parameter::NUMBER, .name = "Mix",
                .min = 0, .max = 100, .reset = 100, .decimals = 1,
                .unit = " %",
            } };

            gain = new Parameter{ {
                .tag = Params::GainX + index,
                .editor = editor,
                .size = { 165,  7, 165 + 65,  7 + 53 },
                .type = Parameter::NUMBER, .name = "Gain",
                .min = 0, .max = 1, .reset = 1, .decimals = 1,
                .unit = " dB",
            } };

            inp1 = new Label{ {
                .size = { 5, 1, 5 + 20, 1 + 14 },
                .value = index == 0 ? "I" : index == 1 ? "III" : "V",
                .center = true, .fontsize = 14, .enabled = false,
            } };
            
            inp2 = new Label{ {
                .size = { 5, 48, 5 + 20, 48 + 14 },
                .value = index == 0 ? "II" : index == 1 ? "IV" : "VI",
                .center = true, .fontsize = 14, .enabled = false,
            } };

            titl = new Label{ {
                .size = { 5, 19, 5 + 20, 19 + 24 },
                .value = index == 0 ? "X" : index == 1 ? "Y" : "Z",
                .center = true, .fontsize = 24, .enabled = true,
            } };

            addView(gain);
            addView(pgai);
            addView(gmix);

            addView(titl);
            addView(inp1);
            addView(inp2);
        }

        ~CombineView()
        {
            gain->forget();
            pgai->forget();
            gmix->forget();
        }
    };

    class CombinePart : public CViewContainer
    {
    public:
        CombineView* views[3];
        CombineToggle* toggles[3];

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

        CombinePart(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size)
        {
            setBackgroundColor({ 0, 0, 0, 0 });

            toggles[0] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 0, listener, editor };
            toggles[1] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 1, listener, editor };
            toggles[2] = new CombineToggle{ { 228, 0, 228 + 424, 0 + 211 }, 2, listener, editor };

            toggles[0]->setVisible(false);
            toggles[1]->setVisible(false);
            toggles[2]->setVisible(false);

            views[0] = new CombineView{ toggles[0], { 0,   0, 0 + 230,   0 + 67 }, 0, listener, editor };
            views[1] = new CombineView{ toggles[1], { 0,  72, 0 + 230,  72 + 67 }, 1, listener, editor };
            views[2] = new CombineView{ toggles[2], { 0, 144, 0 + 230, 144 + 67 }, 2, listener, editor };

            views[0]->Select();

            addView(views[0]);
            addView(views[1]);
            addView(views[2]);

            addView(toggles[0]);
            addView(toggles[1]);
            addView(toggles[2]);
        }
    };
}