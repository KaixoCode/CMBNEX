#define RAPIDJSON_PARSE_DEFAULT_FLAGS kParseStopWhenDoneFlag

#include "base/source/fstreamer.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "public.sdk/source/main/pluginfactory.h"
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstgui/vstgui.h"
#include "vstgui/vstgui_uidescription.h"
#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"

#include "resource.h"

using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace VSTGUI;

#include "Constants.hpp"

#include <numbers>
#include <algorithm>
#include <cmath>
#include <format>
#include <vector>
#include <charconv>
#include <string>
#include <iostream>
#include <array>