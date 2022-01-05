#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "OscillatorView.hpp"
#include "EnvelopeView.hpp"
#include "CombineView.hpp"
#include "ModeView.hpp"
#include "LFOView.hpp"

/**
 * 
 * Oscillator: 
 *  - Gain                    | Knob
 *  - Panning                 | Slider
 *                            
 *  - Tune                    | Knob
 *  - Detune                  | Number
 *                            
 *  - Hard Sync               | Number
 *  - Pulse Width             | Number
 *  - Shaper                  | Number
 *  - Wavetable Position      | Number
 *  - Phase Position          | Number
 *  - Random Phase            | Button
 *  - Wave Visuals            | 
 *                            
 *  - Filter Type (LP/HP/BP)  | Group
 *  - Filter Cutoff           | 
 *  - Filter Resonance        |
 * 
 * Oscillator:
 *  - Gain Envelope (maybe, if not parameter modulation) 
 *  - Wavetable position visuals
 *  - Phase position visuals + random phase toggle
 *  - Tune + detune
 *  - Sync modes (like hard sync, fold, pulse width, etc.)
 *  - Volume
 *  - Waveshaper
 *  - Pre-combine filter + envelope (if not parameter modulation)
 *  - Panning (either simple panning or with voices).
 * 
 * Combine:
 *  - Oscillator A + B, C + D, AB + CD
 *  - Add, Div, Mult, Mod, Or, And
 *  - Remove DC offset filter.
 *  - Feedback with delay option.
 *  - Post-combine volume envelope (if not parameter modulation).
 *  - Post-combine volume knob.
 * 
 * Modulation:
 *  - 4 x ADSR + curves + visuals
 *  - 4 x LFO + curves + visuals
 * 
 * Filter:
 *  - Global post-everything filter
 *  - Envelope (if not parameter modulation)
 * 
 * Pitch:
 *  - Pitch bend
 *  - Pitch envelope (if not parameter modulation)
 *  - Pitch Glide
 * 
 * Master gain
 * 
 */
namespace Kaixo
{
    class Instrument : public AudioEffect
    {
    public:
        Instrument() { setControllerClass(kTestControllerUID); }
        ~Instrument() override {};

        tresult PLUGIN_API initialize(FUnknown* context) override
        {
            tresult result = AudioEffect::initialize(context);
            if (result != kResultOk) return result;
            
            for (int i = 0; i < Params::Size; i++)
                params[i] = paramgoals[i] = ParamNames[i].reset;

            addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
            addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
            addEventInput(STR16("Event In"), 1);

            return kResultOk;
        }
    
        tresult PLUGIN_API terminate() override { return AudioEffect::terminate(); }
        tresult PLUGIN_API setActive(TBool state) override { return AudioEffect::setActive(state); }
        tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) override 
        { 
            Module::SAMPLE_RATE = newSetup.sampleRate; 
            return AudioEffect::setupProcessing(newSetup); 
        }
    
        tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) override 
        {
            switch (symbolicSampleSize) 
            {
            case kSample32: return kResultTrue;
            case kSample64: return kResultTrue;
            default: return kResultFalse;
            };
        }

        tresult PLUGIN_API process(ProcessData& data) override
        {
            if (data.inputParameterChanges)
            {
                int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
                for (int32 index = 0; index < numParamsChanged; index++)
                { 
                    if (auto* paramQueue = data.inputParameterChanges->getParameterData(index))
                    {
                        ParamValue end;
                        int32 sampleOffset;
                        int32 numPoints = paramQueue->getPointCount();
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, end) != kResultTrue) continue;
                        auto param = paramQueue->getParameterId();
                        if (param < Params::Size) 
                            paramgoals[param] = end;
                    }
                }
            }

            IEventList* eventList = data.inputEvents;
            if (eventList)
            {
                int32 numEvent = eventList->getEventCount();
                for (int32 i = 0; i < numEvent; i++)
                {
                    Vst::Event event;
                    if (eventList->getEvent(i, event) == kResultOk)
                    {
                        Event(event);
                    }
                }
            }

            uint32 numChannels = data.outputs[0].numChannels;

            uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
            void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
            void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

            if (data.symbolicSampleSize == kSample32)
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    for (size_t param = 0; param < Params::Size; param++)
                        params[param] = ParamNames[param].smooth ? params[param] * 0.99 + paramgoals[param] * 0.01 : paramgoals[param];

                    for (size_t channel = 0; channel < data.outputs[0].numChannels; channel++)
                    {
                        ((Sample32**)out)[channel][sample] = static_cast<Sample32>(Generate(channel));
                    }
                }
            }
            else
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    for (size_t param = 0; param < Params::Size; param++)
                        params[param] = ParamNames[param].smooth ? params[param] * 0.99 + paramgoals[param] * 0.01 : paramgoals[param];

                    for (size_t channel = 0; channel < data.outputs[0].numChannels; channel++)
                    {
                        ((Sample64**)out)[channel][sample] = Generate(channel);
                    }
                }
            }
            return kResultOk;
        }

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.readDouble(params[i]);

            return kResultOk;
        }

        tresult PLUGIN_API getState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(params[i]);

            return kResultOk;
        }

        virtual Sample64 Generate(size_t channel) = 0;
        virtual void Event(Vst::Event& e) = 0;
        
        double params[Params::Size];
        double paramgoals[Params::Size];
    };


    class TestProcessor : public Instrument
    {
    public:

        constexpr static auto Envelopes = 5;
        constexpr static auto LFOs = 5;
        constexpr static auto Oscillators = 4;
        constexpr static auto Combines = 3;

        static FUnknown* createInstance(void*) { return static_cast<IAudioProcessor*>(new TestProcessor); }

        double modulated[Params::ModCount];

        void Event(Vst::Event& event) override
        {
            switch (event.type)
            {
            case Event::kNoteOnEvent:
                pressed = event.noteOn.pitch;
                if (!env[0].Gate())
                { // Only reset phase when not gate
                    for (int i = 0; i < Oscillators; i++)
                        osc[i].phase = params[Params::RandomPhase1 + i] ? (std::rand() % 32767) / 32767. : 0;
                    for (int i = 0; i < LFOs; i++)
                        lfo[i].phase = 0;
                }
                for (int i = 0; i < Envelopes; i++)
                    env[i].Gate(true);
                break;
            case Event::kNoteOffEvent:
                if (event.noteOff.pitch == pressed)
                {
                    for (int i = 0; i < Envelopes; i++)
                        env[i].Gate(false);
                }
                break;
            }
        }

        double Generate(size_t channel) override
        {   // If envelope is done, no sound so return 0
            if (env[0].Done()) return 0;

            for (int i = 0; i < Envelopes; i++)
            {   // Adjust Envelope parameters and generate
                env[i].settings.attack = params[Params::Env1A + i] * params[Params::Env1A + i] * 5;
                env[i].settings.attackCurve = params[Params::Env1AC + i] * 2 - 1;
                env[i].settings.attackLevel = params[Params::Env1AL + i];
                env[i].settings.decay = params[Params::Env1D + i] * params[Params::Env1D + i] * 5;
                env[i].settings.decayCurve = params[Params::Env1DC + i] * 2 - 1;
                env[i].settings.decayLevel = params[Params::Env1DL + i];
                env[i].settings.sustain = params[Params::Env1S + i];
                env[i].settings.release = params[Params::Env1R + i] * params[Params::Env1R + i] * 5;
                env[i].settings.releaseCurve = params[Params::Env1RC + i] * 2 - 1;
                env[i].Generate(channel);
            }

            if (channel == 0)
            for (int i = 0; i < LFOs; i++)
            {
                lfo[i].settings.oversample = 1;
                lfo[i].settings.frequency = params[Params::LFORate1 + i] * params[Params::LFORate1 + i] * 29.9 + 0.1;
                lfo[i].settings.wtpos = params[Params::LFOPos1 + i];
                lfo[i].settings.shaper3 = params[Params::LFOShaper1 + i];
                lfo[i].sample = lfo[i].Offset(params[Params::LFOPhase1 + i]);
            }

            for (int i = 0; i < Params::ModCount; i++)
                modulated[i] = params[i]; // Move params to modulated so we can adjust their values

            for (int e = 0; e < Envelopes; e++) // Loop over envelopes
                for (int m = 0; m < 5; m++) // Loop over modulators of envelope
                    if (params[Params::Env1M1 + e + (m * 10)] != 0) // If we're modulating something
                    {   // Adjust index for the 'None' value, so ModCount + 1, then -1 to make None == -1
                        size_t index = (size_t)std::floor(params[Params::Env1M1 + e + (m * 10)] * (Params::ModCount + 1)) - 1;
                        double amount = (2 * params[Params::Env1M1A + e + (m * 10)] - 1); // Modulation amount
                        double val = modulated[index] + (env[e].sample * amount); // Add the envelope * amount to the modulated value
                        modulated[index] = ParamNames[index].constrain ? constrain(val, -1, 1) : val; // Constrain if necessary
                    }

            for (int e = 0; e < LFOs; e++) // Loop over lfos
                for (int m = 0; m < 5; m++) // Loop over modulators of lfos
                    if (params[Params::LFO1M1 + e + (m * 10)] != 0) // If we're modulating something
                    {   // Adjust index for the 'None' value, so ModCount + 1, then -1 to make None == -1
                        size_t index = (size_t)std::floor(params[Params::LFO1M1 + e + (m * 10)] * (Params::ModCount + 1)) - 1;
                        double amount = (2 * params[Params::LFO1M1A + e + (m * 10)] - 1); // Modulation amount
                        double val = modulated[index] + ((params[Params::LFOLvl1 + e + (m * 10)] * 2 - 1) * lfo[e].sample * amount); // Add the lfos * amount to the modulated value
                        modulated[index] = ParamNames[index].constrain ? constrain(val, -1, 1) : val; // Constrain if necessary
                    }

            frequency = frequency * 0.95 + 0.05 * pressed; // Smooth frequency

            for (int i = 0; i < Combines; i++)
            {   // Combines parameters
                dcoffp[i].f0 = 35;
                dcoffp[i].type = FilterType::HighPass;
                dcoffp[i].Q = 1;
                dcoffp[i].sampleRate = Module::SAMPLE_RATE;
                dcoffp[i].RecalculateParameters();

                auto _ft = std::floor(params[Params::FilterX + i] * 3); // Filter parameters
                cfilterp[i].sampleRate = Module::SAMPLE_RATE;
                cfilterp[i].f0 = std::pow(modulated[Params::FreqX + i], 2) * (22000 - 20) + 20;
                cfilterp[i].Q = modulated[Params::ResoX + i] * 16 + 1;
                cfilterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                cfilterp[i].RecalculateParameters();
            }

            double _cs[Combines * 2]{ 0, 0, 0, 0, 0, 0 };
            for (int i = 0; i < Oscillators; i++)
            {   // Oscillator parameters
                osc[i].settings.frequency = noteToFreq(frequency 
                    + modulated[Params::Detune1 + i] * 4 - 2 + modulated[Params::Pitch1 + i] * 48 - 24);
                osc[i].settings.wtpos = modulated[Params::WTPos1 + i];
                osc[i].settings.sync = modulated[Params::Sync1 + i] * 8 + 1;
                osc[i].settings.pw = modulated[Params::PulseW1 + i];
                osc[i].settings.shaper = modulated[Params::Shaper1 + i];
                osc[i].settings.shaper2 = modulated[Params::Shaper21 + i];

                auto _ft = std::floor(params[Params::Filter1 + i] * 3); // Filter parameters
                filterp[i].sampleRate = Module::SAMPLE_RATE;
                filterp[i].f0 = std::pow(modulated[Params::Freq1 + i], 2) * (22000 - 20) + 20;
                filterp[i].Q = modulated[Params::Reso1 + i] * 16 + 1;
                filterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                filterp[i].RecalculateParameters();

                if (channel == 0 && modulated[Params::Volume1 + i]) // Generate oscillator sound
                    osc[i].sample = osc[i].Offset(modulated[Params::Phase1 + i]);

                double _vs = osc[i].sample;
                _vs += 2 * modulated[Params::Noise1 + i] * ((std::rand() % 32767) / 32767. - 0.5); // Add noise
                _vs = filter[i].Apply(_vs, channel); // Apply pre-combine filter
                _vs *= modulated[Params::Volume1 + i]; // Adjust for volume
                _vs *= std::min((channel == 1 ? 2 * modulated[Params::Pan1 + i] : 2 - 2 * modulated[Params::Pan1 + i]), 1.); // Panning

                size_t _dests = params[Params::DestA + i] * std::pow(2, Combines * 2);
                for (int j = 0; j < Combines * 2; j++)
                    if ((_dests >> j) & 1U) _cs[j] += _vs;
            }

            double _res = 0;
            for (int i = 0; i < Combines; i++)
            {
                double _v = Combine(_cs[i * 2], _cs[i * 2 + 1], modulated[Params::ModeX + i]);
                _v = std::max(std::min(_v * modulated[Params::GainX + i], 1.), -1.);
                _v = cfilter[i].Apply(_v, channel);
                _v = params[Params::DCX + i] > 0.5 ? dcoff[i].Apply(_v, channel) : _v;

                if (i == Combines - 1)
                {
                    _res = _v;
                    break;
                }

                const size_t _offset = 2 * (Combines - i - 1);
                const size_t _dests = params[Params::DestX + i] * std::pow(2, _offset);
                for (int j = 0; j < _offset; j++)
                    if ((_dests >> j) & 1U) _cs[j + (i + 1) * 2] += _v;
            }

            return env[0].Apply(_res, channel); // Envelope 0 is gain
        }

        enum CombineMode { ADD, MIN, MULT, PONG, MAX, MOD, AND, INLV, OR, XOR, Size };

        double Combine(double a, double b, double mode)
        {
            auto f = std::floor(mode * (Size - 1));
            auto s = std::ceil(mode * (Size - 1));
            auto p = (mode * (Size - 1)) - f;
            return CombineSingle(a, b, f) * (1 - p) + CombineSingle(a, b, s) * p;
        }

        double CombineSingle(double a, double b, int mode)
        {
            uint64_t _a = std::bit_cast<uint64_t>(a);
            uint64_t _b = std::bit_cast<uint64_t>(b);
            uint64_t _as = a * 4294967296;
            uint64_t _bs = b * 4294967296;
            double _ab = 0;
            switch (mode) {
            case AND:  _ab = 1.5 * (std::bit_cast<double>(_a & _b)); break;
            case OR:   _ab = 0.3 * (std::bit_cast<double>(_a | _b)); break;
            case XOR:  _ab = 0.5 * (((_as ^ _bs) % (2 * 4294967296)) / 4294967296. - 1.0); break;
            case PONG: _ab = 1.2 * (a > 0 ? a : b < 0 ? b : 0); break;
            case INLV: _ab = 0.7 * (std::bit_cast<double>((_a & 0x5555555555555555) | (_b & 0xAAAAAAAAAAAAAAAA))); break;
            case MIN:  _ab = 1.4 * (std::min(a, b)); break;
            case MAX:  _ab = 1.3 * (std::max(a, b)); break;
            case MOD:  _ab = 1.4 * (std::bit_cast<double>(_a % (_b == 0 ? 1 : _b))); break;
            case MULT: _ab = 1.8 * (a * b); break;
            case ADD:  _ab = (a + b); break;
            }
            return _ab;
        }
        
        double envelope = 0;
        double frequency = 440;
        size_t pressed = 0;

        Oscillator osc[Oscillators];
        Oscillator lfo[LFOs];

        BiquadParameters filterp[Oscillators];
        StereoEqualizer<2, BiquadFilter<>> filter[Oscillators]{ filterp[0], filterp[1], filterp[2], filterp[3] };

        BiquadParameters cfilterp[Combines];
        StereoEqualizer<2, BiquadFilter<>> cfilter[Combines]{ cfilterp[0], cfilterp[1], cfilterp[2] };

        BiquadParameters dcoffp[Combines];
        StereoEqualizer<2, BiquadFilter<>> dcoff[Combines]{ dcoffp[0], dcoffp[1], dcoffp[2] };

        ADSR env[Envelopes];
    };
}