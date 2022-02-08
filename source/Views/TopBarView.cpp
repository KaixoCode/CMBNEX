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

    CMouseEventResult TopBarView::Button::onMouseDown(CPoint& where, const CButtonState& buttons)
    {
        if (p)
        {   // If press enabled for background effect, setdirty
            bgef.settings.pressed = true;
            setDirty(true);
        }
        press(); // Call callback
        return kMouseEventHandled;
    }

    CMouseEventResult TopBarView::Button::onMouseUp(CPoint& where, const CButtonState& buttons)
    {
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
        pContext->drawString(text, { a.left + 8 + off, a.getCenter().y + 3 + off });
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
        bInit = new Button{ { 492, 10, 492 + 37, 10 + 25 } };
        bInit->text = "Init";
        bInit->press = [this]() {
            this->editor->controller->Init();
            nameDisplay->setDirty(true);
            Colors::MainGreen = CColor{ 179, 66, 0, 255 };
        };
        bInit->p = true;

        bSave = new Button{ { 437, 10, 437 + 50, 10 + 25 } };
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

        nameDisplay = new NameDisplay{ { 200, 5, 200 + 335, 5 + 35 }, editor->controller->preset };
        nameDisplay->press = [this]() {
            //this->prst->setVisible(true);
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

        addView(nameDisplay);
        addView(bSave);
        addView(bInit);
        addView(ovrl);
    }
}
