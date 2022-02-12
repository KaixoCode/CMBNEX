#include "Controller.hpp"
#include "CIDs.hpp"
#include "version.hpp"

using namespace Steinberg;

namespace Kaixo 
{
    MyEditor::MyEditor(Controller* controller)
        : VST3Editor(new UIDescription{ CResourceDescription{ UIDESC } }, dynamic_cast<EditController*>(controller), "view" ),
//        : VST3Editor(dynamic_cast<EditController*>(controller), "view", "CMBNEX.uidesc"),
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
        std::ofstream _file{ _path, std::ios::out };
        _file << std::fixed << std::setprecision(9);

        std::filesystem::path _p = _path;
        _presetPath = _p.parent_path();
        savePath();
        controller->preset = _p.stem().c_str();

        for (size_t i = 0; i < Params::Size; i++)
        {
            // First write parameter name, to identify.
            _file << ParamNames[i].name;
            _file << "=";
            _file << controller->getParamNormalized(i);

            if (i < Params::ModCount) // If modable
            {   // Add mod amount + goals
                for (int j = 0; j < ModAmt; j++)
                {
                    int param = Params::Size + (i * ModAmt + j) * 2;
                    _file << ",";
                    double v = controller->getParamNormalized(param);
                    _file << v;
                    _file << ",";
                    v = controller->getParamNormalized(param + 1);
                    _file << v;
                }
            }
            _file << ",";
            _file << "\n";
        }

        _file.close();
    }

    void MyEditor::Init()
    {
        controller->Init();
    }

    void MyEditor::loadPreset(UTF8StringPtr file)
    {
        Init();
        std::string _path = file;
        std::ifstream _file{ _path, std::ios::in };
        try
        {
            std::filesystem::path _p = _path;
            _presetPath = _p.parent_path();
            savePath();
            controller->preset = _p.stem().c_str();

            std::string line;
            while (std::getline(_file, line))
            {
                try
                {
                    std::string name = line.substr(0, line.find_first_of("="));
                    int param = -1;
                    for (auto& i : ParamNames)
                    {
                        param++;
                        if (i.name == name)
                            break;
                    }

                    if (param >= Params::Size)
                        continue;

                    std::string remainder = line.substr(line.find_first_of("=") + 1);
                    std::string values = remainder.substr(0, remainder.find_first_of(","));
                    double value = std::stod(values);

                    if (param < Params::ModCount)
                    {
                        try
                        {
                            for (int i = 0; i < ModAmt; i++)
                            {
                                remainder = remainder.substr(remainder.find_first_of(",") + 1);
                                std::string values1 = remainder.substr(0, remainder.find_first_of(","));
                                remainder = remainder.substr(remainder.find_first_of(",") + 1);
                                std::string values2 = remainder.substr(0, remainder.find_first_of(","));
                                double moda = std::stod(values1);
                                double modb = std::stod(values2);

                                int p = Params::Size + (param * ModAmt + i) * 2;

                                controller->beginEdit(p);
                                controller->performEdit(p, moda);
                                controller->setParamNormalized(p, moda);
                                controller->endEdit(p);

                                controller->beginEdit(p + 1);
                                controller->performEdit(p + 1, modb);
                                controller->setParamNormalized(p + 1, modb);
                                controller->endEdit(p + 1);
                            }
                        }
                        catch (...)
                        {

                        }
                    }

                    controller->beginEdit(param);
                    controller->performEdit(param, value);
                    controller->setParamNormalized(param, value);
                    controller->endEdit(param);
                }
                catch (...)
                {

                }
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
            return new MyEditor(this);

        return 0;
    }

    tresult PLUGIN_API Controller::notify(IMessage* message)
    {   // Receive modulation updates
        if (FIDStringsEqual(message->getMessageID(), UPDATE_MODULATION))
        {
            // Get the values from the message
            int64 param; double val;
            message->getAttributes()->getInt(UPDATE_MODULATION_PARAM, param);
            message->getAttributes()->getFloat(UPDATE_MODULATION_VALUE, val);

            // Update the wakeupcalls.
            std::lock_guard _(lock);
            for (auto& i : wakeupCalls)
            {
                if (param == std::get<0>(i) && std::get<1>(i) != val)
                {
                    std::get<1>(i).get() = val;
                    std::get<2>(i).get() = true;
                }
            }

            return kResultOk;
        }

        return EditControllerEx1::notify(message);
    }

    tresult PLUGIN_API Controller::initialize(FUnknown* context)
    {
        tresult result = EditControllerEx1::initialize(context);
        if (result != kResultOk) return result;

        // Initialize all the normal parameters
        for (size_t i = 0; i < Params::Size; i++)
        {
            wchar_t name[32];
            int c = 0;
            for (; c < std::strlen(ParamNames[i].name); c++)
                name[c] = ParamNames[i].name[c];
            name[c] = '\0';
            parameters.addParameter(name, nullptr, ParamNames[i].step, ParamNames[i].reset, ParamNames[i].flags, i);
        }

        // Initialize the modulation parameters
        for (int i = 0; i < Params::ModCount * ModAmt; i++)
        {
            // Construct the modulation parameter names as: Mod + mod index + ParamName
            int index = i % ModAmt;
            std::string s1 = "Mod" + std::to_string(index) + ParamNames[i / ModAmt].name;
            std::string s2 = "ModAmt" + std::to_string(index) + ParamNames[i / ModAmt].name;

            // Copy it over to a wide string for the parameter.
            wchar_t name1[64];
            int c1 = 0;
            for (; c1 < s1.size(); c1++)
                name1[c1] = s1[c1];
            name1[c1] = '\0';
            wchar_t name2[64];
            int c2 = 0;
            for (; c2 < s2.size(); c2++)
                name2[c2] = s2[c2];
            name2[c2] = '\0';

            // Add the parameters as hidden and readonly.
            parameters.addParameter(name1, nullptr, 0, 0.0, ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly, Params::Size + i * 2 + 0); // source
            parameters.addParameter(name2, nullptr, 0, 0.5, ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly, Params::Size + i * 2 + 1); // amount
        }

        return result;
    }

    void Controller::Init()
    {   // Default preset
        preset = "default"; 

        for (int i = 0; i < Params::ModCount * ModAmt; i++)
        {   // First reset all modulations
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
        {   // Reset all normal parameters
            beginEdit(i);
            performEdit(i, ParamNames[i].reset);
            if (auto p = getParameterObject(i))
            {   // Use parameter object so we can call 'changed' to always update the ui.
                p->setNormalized(ParamNames[i].reset);
                p->changed();
            }
            endEdit(i);
        }
    }

    tresult PLUGIN_API Controller::setComponentState(IBStream* state)
    {
        if (!state) return kResultFalse;

        IBStreamer streamer(state, kLittleEndian);

        // While we can read parameter name
        while (char8* name = streamer.readStr8())
        {
            for (size_t i = 0; i < Params::Size; i++)
            {   // Find parameter
                if (std::strcmp(ParamNames[i].name, name) == 0)
                {   // Apply value
                    double value = 0.f;
                    streamer.readDouble(value);
                    setParamNormalized(i, value);
                    if (i < Params::ModCount) // If modable
                    {   // Set mod goals + amount
                        for (int j = 0; j < ModAmt; j++)
                        {
                            double value = 0.f;
                            streamer.readDouble(value);
                            setParamNormalized((i * ModAmt + j) * 2 + Params::Size, value);
                            streamer.readDouble(value);
                            setParamNormalized((i * ModAmt + j) * 2 + Params::Size + 1, value);
                        }
                    }
                    break;
                }
            }
        }

        return kResultOk;
    }

    tresult PLUGIN_API Controller::setState(IBStream* state)
    {
        IBStreamer streamer(state, kLittleEndian);

        preset = streamer.readStr8();

        return kResultOk;
    }

    tresult PLUGIN_API Controller::getState(IBStream* state)
    {
        IBStreamer streamer(state, kLittleEndian);

        streamer.writeStr8(preset);

        return kResultOk;
    }

    tresult PLUGIN_API Controller::getMidiControllerAssignment(int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& id)
    {
        if (busIndex == 0 && midiControllerNumber == ControllerNumbers::kPitchBend)
        {
            id = Params::PitchBend;
            return kResultTrue;
        }
        if (busIndex == 0 && midiControllerNumber == ControllerNumbers::kCtrlModWheel)
        {
            id = Params::ModWheel;
            return kResultTrue;
        }
        return kResultFalse;
    }
}
