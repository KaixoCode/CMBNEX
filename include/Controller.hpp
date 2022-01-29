#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class Controller;
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

    class Controller : public EditControllerEx1, public IMidiMapping
    {
    public:
        Controller() = default;

        ~Controller() override = default;

        String preset = "Default";

        std::mutex lock;
        std::vector<std::tuple<int, std::reference_wrapper<double>, std::reference_wrapper<bool>>> wakeupCalls;

        tresult PLUGIN_API notify(IMessage* message) override
        {
            if (FIDStringsEqual(message->getMessageID(), UPDATE_MODULATION))
            {
                int64 param;
                double val;
                message->getAttributes()->getInt(UPDATE_MODULATION_PARAM, param);
                message->getAttributes()->getFloat(UPDATE_MODULATION_VALUE, val);

                std::lock_guard _(lock);
                for (auto& i : wakeupCalls)
                    if (param == std::get<0>(i) && std::get<1>(i) != val)
                    {
                        std::get<1>(i).get() = val;
                        std::get<2>(i).get() = true;
                    }

                return kResultOk;
            }

            return EditControllerEx1::notify(message);
        }

        IPlugView* PLUGIN_API createView(FIDString name) override;

        static FUnknown* createInstance (void*) { return static_cast<IEditController*>(new Controller); }

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
                parameters.addParameter(name, nullptr, ParamNames[i].step, ParamNames[i].reset, ParamNames[i].flags, i);
            }

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                int index = i % ModAmt;
                std::string s1 = "Mod" + std::to_string(index) + ParamNames[i / ModAmt].name;
                std::string s2 = "ModAmt" + std::to_string(index) + ParamNames[i / ModAmt].name;
                wchar_t name1[64];
                int c1 = 0;
                for (;c1 < s1.size(); c1++)
                    name1[c1] = s1[c1];
                name1[c1] = '\0';     
                wchar_t name2[64];
                int c2 = 0;
                for (;c2 < s2.size(); c2++)
                    name2[c2] = s2[c2];
                name2[c2] = '\0';

                parameters.addParameter(name1, nullptr, 0, 0.0, ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly, Params::Size + i * 2 + 0); // source
                parameters.addParameter(name2, nullptr, 0, 0.5, ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly, Params::Size + i * 2 + 1); // amount
            }

            return result;
        }

        void Init()
        {
            preset = "default";

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                int index = i * 2 + Params::Size;
                beginEdit(index);
                setParamNormalized(index, 0);
                performEdit(index, 0);
                endEdit(index);
                beginEdit(index + 1);
                setParamNormalized(index + 1, 0.5);
                performEdit(index + 1, 0.5);
                endEdit(index + 1);
            }

            for (size_t i = 0; i < Params::Size; i++)
            {
                beginEdit(i);
                performEdit(i, ParamNames[i].reset);
                if (auto p = getParameterObject(i))
                {
                    p->setNormalized(ParamNames[i].reset);
                    p->changed();
                }
                endEdit(i);
            }
        }

        tresult PLUGIN_API terminate() override { return EditControllerEx1::terminate(); };

        tresult PLUGIN_API setComponentState(IBStream* state) override
        {
            if (!state) return kResultFalse;

            IBStreamer streamer(state, kLittleEndian);

            preset = streamer.readStr8();

            for (size_t i = 0; i < Params::Size; i++)
            { 
                double value = 0.f;
                if (streamer.readDouble(value) == kResultFalse) return kResultFalse;
                setParamNormalized(i, value);
            }

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                double value = 0.f;
                if (streamer.readDouble(value) == kResultFalse) return kResultFalse;
                setParamNormalized(i * 2 + Params::Size, value);
                double value2 = 0.f;
                if (streamer.readDouble(value2) == kResultFalse) return kResultFalse;
                setParamNormalized(i * 2 + Params::Size + 1, value2);
            }

            return kResultOk;
        }

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            preset = streamer.readStr8();

            for (size_t i = 0; i < Params::Size; i++)
            {
                double val;
                streamer.readDouble(val);
                beginEdit(i);
                setParamNormalized(i, val);
                performEdit(i, val);
                endEdit(i);
            }

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                int index = i * 2 + Params::Size;
                double val;
                streamer.readDouble(val);
                beginEdit(index);
                setParamNormalized(index, val);
                performEdit(index, val);
                endEdit(index);
                streamer.readDouble(val);
                beginEdit(index + 1);
                setParamNormalized(index + 1, val);
                performEdit(index + 1, val);
                endEdit(index + 1);
            }

            return kResultOk;
        }

        tresult PLUGIN_API getState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            streamer.writeStr8(preset);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(getParamNormalized(i));
            
            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                int index = i * 2 + Params::Size;
                streamer.writeDouble(getParamNormalized(index));
                streamer.writeDouble(getParamNormalized(index + 1));
            }

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

        OBJ_METHODS(Controller, EditControllerEx1)
        DEFINE_INTERFACES
            DEF_INTERFACE(IMidiMapping)
        END_DEFINE_INTERFACES (EditController)
        DELEGATE_REFCOUNT (EditController)
    protected:
    };
}
