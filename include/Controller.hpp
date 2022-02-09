#pragma once
#include "pch.hpp"

namespace Kaixo
{
    class UserSettings
    {
    public:
        static std::filesystem::path SettingsPath()
        {
            std::filesystem::path _path;
#ifdef WIN32
            TCHAR szPath[MAX_PATH];
            // Get path for each computer, non-user specific and non-roaming data.
            if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
            {
                // Append product-specific path
                _path = szPath;
                _path = _path / "Kaixo" / "CMBNEX" / "settings";

                if (!std::filesystem::exists(_path))
                    std::filesystem::create_directories(_path);
            }
#endif

            return _path;
        }
    };


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

        std::filesystem::path _presetPath = "";

        void savePreset(UTF8StringPtr file);
        void loadPreset(UTF8StringPtr file);
        void Init();

        void loadSettings()
        {
            loadColor();
            loadTheme();
            loadPath();
        }

        void loadPath()
        {
            try
            {
                std::ifstream _file{ UserSettings::SettingsPath() / "presetpath" };
                if (_file.is_open())
                {
                    _file >> _presetPath;
                }
            }
            catch (...)
            {
            }
        }

        void loadTheme()
        {
            try
            {
                std::ifstream _file{ UserSettings::SettingsPath() / "theme" };
                if (_file.is_open())
                {
                    _file >> Colors::lightMode;
                    UpdateTheme(Colors::lightMode);
                }
            }
            catch (...)
            {
            }
        }

        void loadColor()
        {
            try
            {
                std::ifstream _file{ UserSettings::SettingsPath() / "color" };
                if (_file.is_open())
                {
                    _file >> Colors::MainGreen.red;
                    _file >> Colors::MainGreen.green;
                    _file >> Colors::MainGreen.blue;
                }
            } 
            catch(...)
            { 
            }
        }

        void savePath()
        {
            try
            {
                std::ofstream _file{ UserSettings::SettingsPath() / "presetpath" };
                _file << _presetPath;
            }
            catch (...)
            {
            }
        }

        void saveColor()
        {
            try
            {
                std::ofstream _file{ UserSettings::SettingsPath() / "color" };
                _file << Colors::MainGreen.red;
                _file << Colors::MainGreen.green;
                _file << Colors::MainGreen.blue;
            }
            catch (...)
            {
            }
        }

        void UpdateTheme(bool value)
        {
            Colors::lightMode = value;
            if (value)
            {
                Colors::Background = CColor{ 250, 250, 250, 255 };
                Colors::MainBack = CColor{ 235, 235, 235, 255 };
                Colors::MainBackL = CColor{ 245, 240, 245, 255 };
                Colors::MainBackD = CColor{ 215, 215, 215, 255 };
                Colors::DarkBackD = CColor{ 195, 195, 195, 255 };
                Colors::DarkBack = CColor{ 220, 220, 220, 255 };
                Colors::DarkBackH = CColor{ 205, 205, 205, 255 };
                Colors::ItemBack = CColor{ 220, 220, 220, 255 };
                Colors::KnobBack = CColor{ 205, 205, 205, 255 };
                Colors::KnobBackL = CColor{ 220, 220, 220, 255 };
                Colors::KnobBackDark = CColor{ 195, 195, 195, 255 };
                Colors::Border = CColor{ 230, 230, 230, 255 };
                Colors::MainText = CColor{ 55, 55, 55, 255 };
                Colors::OffText = CColor{ 155, 155, 155, 255 };
                Colors::OffTextL = CColor{ 155, 155, 155, 255 };
                Colors::BorderHover = CColor{ 225, 225, 225, 255 };
            }
            else
            {
                Colors::Background = CColor{ 15, 15, 15, 255 };
                Colors::MainBack = CColor{ 25, 25, 25, 255 };
                Colors::MainBackL = CColor{ 40, 40, 40, 255 };
                Colors::MainBackD = CColor{ 5,  5,  5, 255 };
                Colors::DarkBackD = CColor{ 5, 5, 5, 255 };
                Colors::DarkBack = CColor{ 15, 15, 15, 255 };
                Colors::DarkBackH = CColor{ 23, 23, 23, 255 };
                Colors::ItemBack = CColor{ 15, 15, 15, 255 };
                Colors::KnobBack = CColor{ 45, 45, 45, 255 };
                Colors::KnobBackL = CColor{ 55, 55, 55, 255 };
                Colors::KnobBackDark = CColor{ 30, 30, 30, 255 };
                Colors::Border = CColor{ 30, 30, 30, 255 };
                Colors::MainText = CColor{ 200, 200, 200, 255 };
                Colors::OffText = CColor{ 100, 100, 100, 255 };
                Colors::OffTextL = CColor{ 110, 110, 110, 255 };
                Colors::BorderHover = CColor{ 70, 70, 70, 255 };
            }
        }

        void saveTheme()
        {
            try
            {
                std::ofstream _file{ UserSettings::SettingsPath() / "theme" };
                _file << Colors::lightMode;
            }
            catch (...)
            {
            }
        }

    };

    // Plugin controller
    class Controller : public EditControllerEx1, public IMidiMapping
    {
    public:
        static FUnknown* createInstance (void*) { return static_cast<IEditController*>(new Controller); }
        Controller() = default;

        ~Controller() override = default;

        String preset = "default";

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
