#include "Controller.hpp"
#include "CIDs.hpp"

using namespace Steinberg;

namespace Kaixo 
{
    MyEditor::MyEditor(Controller* controller)
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
        return (ModSources)(controller->getParamNormalized(i) * (int)ModSources::Amount + 0.1);
    }

    void MyEditor::modSource(int32_t param, int index, ModSources set) {
        if (param >= Params::ModCount) return;
        int i = (index + param * ModAmt) * 2 + Params::Size;
        if (set <= ModSources::Amount && set >= ModSources::None)
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

    void MyEditor::savePreset(UTF8StringPtr file)
    {
        std::string _path = file;
        if (!_path.ends_with(".cmbnex"))
            _path += ".cmbnex";
        std::ofstream _file{ _path, std::ios::binary | std::ios::out };

        std::filesystem::path _p = _path;
        controller->preset = _p.stem().c_str();
        for (int i = 0; i < Params::ModCount * ModAmt; i++)
        {
            int index = i * 2 + Params::Size;
            float v = controller->getParamNormalized(index);
            _file.write((char*)&v, sizeof(float));
            v = controller->getParamNormalized(index + 1);
            _file.write((char*)&v, sizeof(float));
        }

        for (size_t i = 0; i < Params::Size; i++)
        {
            float v = controller->getParamNormalized(i);
            _file.write((char*)&v, sizeof(float));
        }

        _file.close();
    }

    void MyEditor::Init()
    {
        controller->Init();
    }

    void MyEditor::loadPreset(UTF8StringPtr file)
    {
        std::string _path = file;
        std::ifstream _file{ _path, std::ios::binary | std::ios::in };
        try
        {
            std::filesystem::path _p = _path;
            controller->preset = _p.stem().c_str();
            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                float v;
                _file.read((char*)&v, sizeof(float));
                int index = i * 2 + Params::Size;
                controller->beginEdit(index);
                controller->performEdit(index, v); 
                controller->setParamNormalized(index, v);
                controller->endEdit(index);
                _file.read((char*)&v, sizeof(float));
                controller->beginEdit(index + 1);
                controller->performEdit(index + 1, v);
                controller->setParamNormalized(index + 1, v);
                controller->endEdit(index + 1); 
            }

            for (size_t i = 0; i < Params::Size; i++)
            {
                float v;
                _file.read((char*)&v, sizeof(float));
                controller->beginEdit(i);
                controller->performEdit(i, v);
                if (auto p = controller->getParameterObject(i))
                {
                    p->setNormalized(v);
                    p->changed();
                }
                controller->endEdit(i);
            }

            _file.close();
        }
        catch (...)
        {

        }
    }

    IPlugView* PLUGIN_API Controller::createView(FIDString name)
    {
        if (std::strcmp(name, ViewType::kEditor) == 0)
        {
            return new MyEditor(this);
        }
        return 0;
    }
}
