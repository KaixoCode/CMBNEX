#pragma once
#define RAPIDJSON_PARSE_DEFAULT_FLAGS kParseStopWhenDoneFlag

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shlobj.h>
#endif

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
#include "pluginterfaces/base/smartpointer.h"

#ifndef CMBNEX_TESTS
#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstgui/vstgui.h"
#include "vstgui/vstgui_uidescription.h"
#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"
using namespace VSTGUI;
#endif 

using namespace Steinberg;
using namespace Steinberg::Vst;

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
#include <mutex>
#include <filesystem>
#include <fstream>
#include <condition_variable>
#include <future>
#include <complex>
#include <cassert>

#if defined(_M_X64) || defined(__amd64__) || defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)

#ifndef USE_SIMD
#define USE_SIMD 1
#endif

#if USE_SIMD
#include <immintrin.h>
#endif

#elif defined (__ARM_NEON__) || defined (__ARM_NEON) || defined (__arm64__) || defined (__aarch64__)

#ifndef USE_SIMD
#define USE_SIMD 1
#endif

#include <arm_neon.h>

#else

// No SIMD Support
#ifndef USE_SIMD
#define USE_SIMD 0
#endif

#endif
