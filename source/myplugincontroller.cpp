#include "myplugincontroller.hpp"
#include "myplugincids.hpp"

using namespace Steinberg;

namespace Kaixo 
{
    MyEditor::MyEditor(TestController* controller)
        : VST3Editor(dynamic_cast<EditController*>(controller), "view", "myplugineditor.uidesc"),
        controller(controller)
    {
        CView::idleRate = 60;
    }

    MyEditor::~MyEditor() 
    {
        controller->wakeupCalls.clear();
    }

    ModSources MyEditor::modSource(int32_t param, int index) {
        if (param >= Params::ModCount) return ModSources::None;
        int i = (index + param * ModAmt) * 2 + Params::Size;
        return (ModSources)(controller->getParamNormalized(i) * (int)ModSources::Amount);
    }

    void MyEditor::modSource(int32_t param, int index, ModSources set) {
        if (param >= Params::ModCount) return;
        int i = (index + param * ModAmt) * 2 + Params::Size;
        if (set < ModSources::Amount && set >= ModSources::None)
        {
            controller->beginEdit(i);
            controller->performEdit(i, (double)set / (double)ModSources::Amount);
            controller->setParamNormalized(i, (double)set / (double)ModSources::Amount);
            controller->endEdit(i);
        }
    }

    double MyEditor::modAmount(int32_t param, int index) {
        int i = (index + param * ModAmt) * 2 + Params::Size + 1;
        return controller->getParamNormalized(i);
    }

    void MyEditor::modAmount(int32_t param, int index, double value) {
        int i = (index + param * ModAmt) * 2 + Params::Size + 1;
        controller->beginEdit(i);
        controller->performEdit(i, value);
        controller->setParamNormalized(i, value);
        controller->endEdit(i);
    }
}
