#pragma once
#include "pch.hpp"
#include "Components/Parameter.hpp"
#include "Components/Label.hpp"

namespace Kaixo
{
    class TopBarView : public CViewContainer
    {
    public:
        CNewFileSelector* selector;
        CNewFileSelector* loader;

        CMessageResult notify(CBaseObject* sender, IdStringPtr message) override
        {
            if (selector == sender)
            {
                if (auto file = selector->getSelectedFile(0))
                {
                    editor->savePreset(file);
                    nameDisplay->setDirty(true);
                }
                return kMessageUnknown;
            } else if (loader == sender)
            {
                if (auto file = loader->getSelectedFile(0))
                {
                    editor->loadPreset(file);
                    nameDisplay->setDirty(true);
                }
                return kMessageUnknown;
            }
            else
                return CViewContainer::notify(sender, message);
        }

        class Button : public CView
        {
        public:
            using CView::CView;

            std::function<void(void)> press;

            BackgroundEffect bgef{ getViewSize() };
            bool p = false;
            String text;

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
            {
                if (p)
                {
                    bgef.pressed = true;
                    setDirty(true);
                }
                press();
                return kMouseEventHandled;
            }

            CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override
            {
                bgef.pressed = false;
                setDirty(true);
                return kMouseEventHandled;
            }

            void draw(CDrawContext* pContext) override
            {
                auto a = getViewSize();
                bgef.draw(pContext);
                pContext->setFont(pContext->getFont(), 14);
                pContext->setFontColor(MainText);
                double off = bgef.pressed ? 1 : 0;
                pContext->drawString(text, { a.left + 8 + off, a.getCenter().y + 3 + off });
            }
        };

        class NameDisplay : public CView
        {
        public:
            NameDisplay(const CRect& c, String& name)
                : CView(c), name(name)
            {}

            std::function<void(void)> press;
            String& name;

            BackgroundEffect bgef{ getViewSize() };

            CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override
            {
                press();
                return kMouseEventHandled;
            }

            void draw(CDrawContext* pContext) override
            {
                bgef.dark = true;
                bgef.draw(pContext);
                auto a = getViewSize();
                pContext->setFont(pContext->getFont(), 16);
                pContext->setFontColor(MainText);
                pContext->drawString(name, { a.left + 5, a.getCenter().y + 4 });
            }
        };

        class Overlay : public CView
        {
        public:
            using CView::CView;

            void draw(CDrawContext* pContext) override
            {
                auto a = getViewSize();

                pContext->setFrameColor(BorderHover);

                for (int i = 0; i < 3; i++)
                {
                    double d = 5;
                    double y = 17 + i * d;
                    double x = 75;
                    double w = 50;

                    pContext->drawLine({ x, y }, { x + w, y });
                    pContext->drawLine({ getWidth() - x, y }, { getWidth() - x - w, y });
                }

            }
        };

        Parameter* ovsm, *voic, * retr, *gain, *thrd;
        Label* ovsl, *vcel, *rtrl, *thdl;

        Overlay* ovrl;

        Button* bSave;
        Button* bInit;

        NameDisplay* nameDisplay;

        MyEditor* editor;

        TopBarView(const CRect& size, IControlListener* listener, MyEditor* editor)
            : CViewContainer(size), editor(editor)
        {
            setBackgroundColor({ 0, 0, 0, 0 });
            addView(new BackgroundEffect{ { 0, 0, getWidth(), getHeight() } });
            bInit = new Button{ { 297, 10, 297 + 37, 10 + 25 } };
            bInit->text = "Init";
            bInit->press = [this]() {
                this->editor->controller->Init();
                nameDisplay->setDirty(true);
            };                  
            bInit->p = true;
            
            bSave = new Button{ { 242, 10, 242 + 50, 10 + 25 } };
            bSave->text = "Save";
            bSave->press = [this]() {
                selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
                selector->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                selector->setDefaultSaveName("Preset.cmbnex");
                selector->setTitle("Choose An Audio File");
                selector->run(this);
                selector->forget();
            };          
            
            nameDisplay = new NameDisplay{ { 5, 5, 5 + 335, 5 + 35 }, editor->controller->preset };
            nameDisplay->press = [this]() {
                loader = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
                loader->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
                loader->setDefaultSaveName("Preset.cmbnex");
                loader->setTitle("Choose An Audio File");
                loader->run(this);
                loader->forget();
            };
            
            ovrl = new Overlay{ { 0, 0, getWidth(), getHeight() } };
            ovrl->setMouseEnabled(false);

            double x = 377;
            double padding = 2;

            gain = new Parameter{ {
                .tag = Params::GlobalGain,
                .editor = editor,
                .size = { x,   4,  x + 65,   4 + 35 },
                .type = Parameter::SLIDER, .modable = false, .name = "Global",
                .min = 0, .max = 1, .reset = 0.5, .decimals = 1,
                .unit = " dB",
            } };

            x += 65 + padding + 5;

            voic = new Parameter{ {
                .tag = Params::Voices,
                .editor = editor,
                .size = { x, 23, x + 88, 23 + 16 },
                .type = Parameter::GROUP, .parts = { "Mono", "Poly" },
                .padding = 4
            } };

            vcel = new Label{ {
                .size = { x + 1, 5, x + 1 + 90, 5 + 25 },
                .value = "Voicing", .center = false, .fontsize = 14,
            } };

            x += 88 + padding + 1;

            retr = new Parameter{ {
                .tag = Params::Retrigger,
                .editor = editor,
                .size = { x, 23, x + 16, 23 + 16 },
                .type = Parameter::BUTTON, .name = "R"
            } };

            rtrl = new Label{ {
                .size = { x - 1, 5, x - 1 + 90, 5 + 25 },
                .value = "Rtr", .center = false, .fontsize = 14,
            } };
            
            x += 16 + padding + 5;

            ovsm = new Parameter{ {
                .tag = Params::Oversample,
                .editor = editor,
                .size = { x, 23, x + 93, 23 + 16 },
                .type = Parameter::GROUP, .parts = { "1x", "2x", "4x", "8x" },
                .padding = 4
            } };

            ovsl = new Label{ {
                .size = { x, 5, x + 90, 5 + 25 },
                .value = "Quality", .center = false, .fontsize = 14,
            } };

            x += 93 + padding;

            thrd = new Parameter{ {
                .tag = Params::Threading,
                .editor = editor,
                .size = { x, 23, x + 73, 23 + 16 },
                .type = Parameter::GROUP, .parts = { "Off", "On" },
                .padding = 4
            } };

            thdl = new Label{ {
                .size = { x + 1, 5, x + 1 + 90, 5 + 25 },
                .value = "Threading", .center = false, .fontsize = 14,
            } };
   

            addView(nameDisplay);
            addView(bSave);
            addView(bInit);
            //addView(ovrl);

            addView(thdl);
            addView(thrd);
            addView(rtrl);
            addView(ovsl);
            addView(vcel);
            addView(ovsm);
            addView(voic);
            addView(retr);
            addView(gain);
        }

        ~TopBarView()
        {
            ovsm->forget();
            voic->forget();
            retr->forget();
            gain->forget();
        }
    };
}
