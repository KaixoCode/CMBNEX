#include "Processing/Modules.hpp"
#include "Controller.hpp"
#include "Processing/Processor.hpp"
#include "CIDs.hpp"
#include "version.hpp"

#define stringPluginName "CMBNEX"

using namespace Steinberg::Vst;
using namespace Kaixo;

//------------------------------------------------------------------------
//  VST Plug-in Entry
//------------------------------------------------------------------------
// Windows: do not forget to include a .def file in your project to export
// GetPluginFactory function!
//------------------------------------------------------------------------

BEGIN_FACTORY_DEF ("Kaixo", 
                   "https://kaixo.me", 
                   "mailto:contact@kaixo.me")

    //---First Plug-in included in this factory-------
    // its kVstAudioEffectClass component
    DEF_CLASS2 (INLINE_UID_FROM_FUID(kCMBNEXProcessorUID),
                PClassInfo::kManyInstances,    // cardinality
                kVstAudioEffectClass,    // the component category (do not changed this)
                stringPluginName,        // here the Plug-in name (to be changed)
                Vst::kDistributable,    // means that component and controller could be distributed on different computers
                CMBNEXVST3Category, // Subcategory for this Plug-in (to be changed)
                FULL_VERSION_STR,        // Plug-in version (to be changed)
                kVstVersionString,        // the VST 3 SDK version (do not changed this, use always this define)
                Processor::createInstance)    // function pointer called when this component should be instantiated

    // its kVstComponentControllerClass component
    DEF_CLASS2 (INLINE_UID_FROM_FUID (kCMBNEXControllerUID),
                PClassInfo::kManyInstances, // cardinality
                kVstComponentControllerClass,// the Controller category (do not changed this)
                stringPluginName "Controller",    // controller name (could be the same than component name)
                0,                        // not used here
                "",                        // not used here
                FULL_VERSION_STR,        // Plug-in version (to be changed)
                kVstVersionString,        // the VST 3 SDK version (do not changed this, use always this define)
                Controller::createInstance)// function pointer called when this component should be instantiated

    //----for others Plug-ins contained in this factory, put like for the first Plug-in different DEF_CLASS2---

END_FACTORY
