#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class Controller;

    // Plugin view
    class MyEditor : public VST3Editor
    {
    public:
        MyEditor(Controller* controller);
        ~MyEditor();

        Controller* controller;

        ModSources modSource(int32_t param, int index);
        void modSource(int32_t param, int index, ModSources set);
        double modAmount(int32_t param, int index);
        void modAmount(int32_t param, int index, double val);

        void savePreset(UTF8StringPtr file);
        void loadPreset(UTF8StringPtr file);
        void Init();
    };

    // Plugin controller
    class Controller : public EditControllerEx1, public IMidiMapping
    {
    public:
        static FUnknown* createInstance (void*) { return static_cast<IEditController*>(new Controller); }
        Controller() = default;

        ~Controller() override = default;

        String preset = "Default";

        // WakeupCalls are used to set modulation values in the ui.
        std::mutex lock;
        std::vector<std::tuple<int, std::reference_wrapper<double>, std::reference_wrapper<bool>>> wakeupCalls;

        // Set to default preset
        void Init();

        IPlugView* PLUGIN_API createView(FIDString name) override;
        tresult PLUGIN_API notify(IMessage* message) override;
        tresult PLUGIN_API initialize(FUnknown* context) override;
        tresult PLUGIN_API setComponentState(IBStream* state) override;
        tresult PLUGIN_API setState(IBStream* state) override;
        tresult PLUGIN_API getState(IBStream* state) override;
        tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id) override;

        OBJ_METHODS(Controller, EditControllerEx1)
        DEFINE_INTERFACES
            DEF_INTERFACE(IMidiMapping)
        END_DEFINE_INTERFACES (EditController)
        DELEGATE_REFCOUNT (EditController)
    protected:
    };
}
