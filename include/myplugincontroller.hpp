#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class TestController;
    class MyEditor : public VST3Editor
    {
    public:

        MyEditor(EditController* controller)
            : VST3Editor(new UIDescription{ CResourceDescription{ IDR_DATA1 } }, controller, "view"),
            controller(controller)
        {}
    
        EditController* controller;
    };

    class TestController : public EditControllerEx1, public IMidiMapping
    {
    public:
        TestController() = default;

        ~TestController () override = default;

        IPlugView* PLUGIN_API createView(FIDString name) override
        {
            if (strcmp(name, ViewType::kEditor) == 0)
            {
                return new MyEditor(this);
            }
            return 0;
        }

        static FUnknown* createInstance (void*) { return static_cast<IEditController*>(new TestController); }

        tresult PLUGIN_API initialize(FUnknown* context) override
        {
            tresult result = EditControllerEx1::initialize(context);
            if (result != kResultOk) return result;

            for (size_t i = 0; i < Params::Size; i++)
            {
                wchar_t name[32];
                int c = 0;
                for (;c < std::strlen(ParamNames[i].name); c++)
                    name[c] = ParamNames[i].name[c];
                name[c] = '\0';
                parameters.addParameter(name, nullptr, ParamNames[i].step, ParamNames[i].reset, ParameterInfo::kCanAutomate, i);
            }
            return result;
        }

        tresult PLUGIN_API terminate() override { return EditControllerEx1::terminate(); };

        tresult PLUGIN_API setComponentState(IBStream* state) override
        {
            if (!state) return kResultFalse;

            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
            { 
                double value = 0.f;
                if (streamer.readDouble(value) == kResultFalse) return kResultFalse;
                setParamNormalized(i, value);
            }

            return kResultOk;
        }

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
            {
                double val;
                streamer.readDouble(val);
                beginEdit(i);
                setParamNormalized(i, val);
                performEdit(i, val);
                endEdit(i);
            }
            return kResultOk;
        }

        tresult PLUGIN_API getState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(getParamNormalized(i));

            return kResultOk;
        }

        //tresult PLUGIN_API setState(IBStream* state) override;
        //tresult PLUGIN_API getState(IBStream* state) override;
        //tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string) override;
        //tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized) override;

        tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id) override
        {
            if (busIndex == 0 && midiControllerNumber == ControllerNumbers::kPitchBend)
            {
                id = Params::PitchBend;
                return kResultTrue;
            }
            return kResultFalse;
        }

        OBJ_METHODS(TestController, EditControllerEx1)
        DEFINE_INTERFACES
            DEF_INTERFACE(IMidiMapping)
        END_DEFINE_INTERFACES (EditController)
        DELEGATE_REFCOUNT (EditController)
    protected:
    };
}
