#include "Views/TopBarView.hpp"

namespace Kaixo
{
    CMessageResult TopBarView::notify(CBaseObject* sender, IdStringPtr message)
    {
        // If message from selector, select save file
        if (selector == sender)
        {
            if (auto file = selector->getSelectedFile(0))
            {
                editor->savePreset(file); // Save preset to file
                nameDisplay->setDirty(true); // Dirty to update preset name display
            }
            return kMessageUnknown;
        }
        else if (loader == sender)
        {
            if (auto file = loader->getSelectedFile(0))
            {
                editor->loadPreset(file); // Load preset from file
                nameDisplay->setDirty(true); // Dirty to update preset name display
            }
            return kMessageUnknown;
        }
        else
            return CViewContainer::notify(sender, message);
    }

    CMouseEventResult TopBarView::Button::onMouseExited(CPoint& where, const CButtonState& buttons)
    {
        really = false;
        setDirty(true);
        return kMouseEventHandled;
    }

    CMouseEventResult TopBarView::Button::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        bgef.settings.pressed = true;
        setDirty(true);
        return kMouseEventHandled;
    }

    CMouseEventResult TopBarView::Button::onMouseUp(CPoint& where, const CButtonState& buttons)
    {
        if (bgef.settings.pressed && getViewSize().pointInside(where))
        {
            if (!p)
            {
                press(); // Call callback
            }
            else if (really && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - pressed).count() > 500)
            {
                really = false;
                press(); // Call callback
            }
            else
            {
                pressed = std::chrono::steady_clock::now();
                really = true;
            }
        }
        bgef.settings.pressed = false;
        setDirty(true);
        return kMouseEventHandled;
    }

    void TopBarView::Button::draw(CDrawContext* pContext)
    {
        auto a = getViewSize();
        bgef.draw(pContext);
        pContext->setFont(pContext->getFont(), 14);
        pContext->setFontColor(Colors::MainText);
        double off = bgef.settings.pressed ? 1 : 0;
        double xoff = bgef.settings.pressed ? 1 : 0;
        String str = text;
        if (really) str = "Sure?", xoff -= 9;
        pContext->drawString(str, {a.left + (!p ? 8 : 13) + xoff, a.getCenter().y + 3 + off});
    }

    CMouseEventResult TopBarView::NameDisplay::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        press();
        return kMouseEventHandled;
    }

    void TopBarView::NameDisplay::draw(CDrawContext* pContext)
    {
        auto a = getViewSize();
        BackgroundEffect::draw(pContext, { .size = a, .dark = true });
        pContext->setFont(pContext->getFont(), 16);
        pContext->setFontColor(Colors::MainText);
        pContext->drawString(name, { a.left + 5, a.getCenter().y + 4 });
    }

    void TopBarView::Overlay::draw(CDrawContext* pContext)
    {
        auto a = getViewSize();

        pContext->setFrameColor(Colors::BorderHover);

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

    TopBarView::TopBarView(const CRect& size, IControlListener* listener, MyEditor* editor, PresetView* prst)
        : CViewContainer(size), editor(editor), prst(prst)
    {
        setBackgroundColor({ 0, 0, 0, 0 });
        addView(new BackgroundEffect{ {.size = { 0, 0, getWidth(), getHeight() } } });
        bInit = new Button{ { 484, 10, 484 + 45, 10 + 25 } };
        bInit->text = "Init";
        bInit->press = [this]() {
            this->editor->controller->Init();
            nameDisplay->setDirty(true);
        };
        bInit->p = true;

        static auto CMBNEX = CFileExtension{ "CMBNEX Preset", "cmbnex", "application/preset", 0, "me.kaixo.cmbnex" };

        bSave = new Button{ { 429, 10, 429 + 50, 10 + 25 } };
        bSave->text = "Save";
        bSave->press = [this, editor]() {
            selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
            if (editor->_presetPath != "") 
                selector->setInitialDirectory(editor->_presetPath.string());
            //selector->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
            selector->setDefaultExtension(CMBNEX);
            if (editor->controller->preset != "default")
            {
                std::string _str = editor->controller->preset.operator const Steinberg::char8 *();
                _str += ".cmbnex";
                selector->setDefaultSaveName(_str.c_str());
            }
            else
                selector->setDefaultSaveName("Preset.cmbnex");
    
            selector->setTitle("Choose An Audio File");
            selector->run(this);
            selector->forget();
        };

        nameDisplay = new NameDisplay{ { 200, 5, 200 + 335, 5 + 35 }, editor->controller->preset };
        nameDisplay->press = [this, editor]() {
            
            loader = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
            if (editor->_presetPath != "")
                loader->setInitialDirectory(editor->_presetPath.string()); 
            //loader->addFileExtension(CFileExtension{ "CMBNEX Preset", "cmbnex" });
            loader->setDefaultExtension(CMBNEX);
            loader->setDefaultSaveName("Preset.cmbnex");
            loader->setTitle("Choose An Audio File");
            loader->run(this);
            loader->forget();
        };

        ovrl = new Overlay{ { 0, 0, getWidth(), getHeight() } };
        ovrl->setMouseEnabled(false);

        addView(nameDisplay);
        addView(bSave);
        addView(bInit);
        addView(ovrl);
    }
}
