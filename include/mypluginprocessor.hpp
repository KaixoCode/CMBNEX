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
#include "SubOscView.hpp"
#include "MidiView.hpp"
#include "TopBarView.hpp"
#include "myplugincontroller.hpp"

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
 * Midi:
 *  - Transpose
 *  - Bend
 *  - Glide
 *  - Rtg
 *  - Oversampling
 *  - Osc<Vel
 *  - Freq<Key
 *  - Freq<Vel
 *  - Vel
 *  
 * 
 * LFO:
 *  - Sync
 *  - Retrigger
 * 
 * 
 * 
 * TODO:
 * - LFO Sync not synced
 * 
 * - Noise Color
 * - Waveshaper X-Y Panel Thingy
 * - Optimize new shapers
 * 
 * 
 */

namespace Kaixo
{
    class Instrument : public AudioEffect
    {
    public:
        Instrument() { setControllerClass(kCMBNEXControllerUID); }
        ~Instrument() override {};

        TestController* controller;

        tresult PLUGIN_API initialize(FUnknown* context) override
        {
            tresult result = AudioEffect::initialize(context);

            if (result != kResultOk) return result;
            for (int i = 0; i < Params::Size; i++)
                params[i] = paramgoals[i] = ParamNames[i].reset;

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
                modgoals[i] = 0, modamount[i] = 0.5;

            addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
            addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
            addEventInput(STR16("Event In"), 1);

            return kResultOk;
        }
    
        tresult PLUGIN_API terminate() override { return AudioEffect::terminate(); }
        tresult PLUGIN_API setActive(TBool state) override { return AudioEffect::setActive(state); }
        tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) override 
        {
            controller = dynamic_cast<TestController*>(getPeer());
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
                        else
                        {
                            param -= Params::Size;
                            if (param % 2 == 0)
                            {
                                param /= 2;
                                modgoals[param] = end;
                            }
                            else
                            {
                                (param -= 1) /= 2;
                                modamount[param] = end;
                            }
                        }
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

                    auto [l, r] = Generate(data);
                    if (data.outputs[0].numChannels == 2)
                    {
                        ((Sample32**)out)[0][sample] = l;
                        ((Sample32**)out)[1][sample] = r;
                    }
                }
            }
            else
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    for (size_t param = 0; param < Params::Size; param++)
                        params[param] = ParamNames[param].smooth ? params[param] * 0.99 + paramgoals[param] * 0.01 : paramgoals[param];

                    auto[l, r] = Generate(data);
                    if (data.outputs[0].numChannels == 2)
                    {
                        ((Sample64**)out)[0][sample] = l;
                        ((Sample64**)out)[1][sample] = r;
                    }
                }
            }
            return kResultOk;
        }

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.readDouble(paramgoals[i]);

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                streamer.readDouble(modgoals[i]);
                streamer.readDouble(modamount[i]);
            }

            return kResultOk;
        }

        tresult PLUGIN_API getState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(paramgoals[i]);

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                streamer.writeDouble(modgoals[i]);
                streamer.writeDouble(modamount[i]);
            }

            return kResultOk;
        }

        virtual std::pair<Sample64, Sample64> Generate(ProcessData& data) = 0;
        virtual void Event(Vst::Event& e) = 0;
        
        double params[Params::Size];
        double paramgoals[Params::Size];

        double modgoals[Params::ModCount * ModAmt];
        double modamount[Params::ModCount * ModAmt];
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

        bool notesPressed[128] { 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        };


        void Event(Vst::Event& event) override
        {
            switch (event.type)
            {
            case Event::kNoteOnEvent:
                if (paramgoals[Params::PitchBend] > 1 || paramgoals[Params::PitchBend] < 0) paramgoals[Params::PitchBend] = 0.5;

                velocity = event.noteOn.velocity;
                pressedOld = frequency;
                pressed = event.noteOn.pitch + event.noteOn.tuning * 0.01;
                key = event.noteOn.pitch / 127.;
                notesPressed[event.noteOn.pitch] = true;
                if (params[Params::Retrigger] > 0.5 || !env[0].Gate())
                { 
                    for (int i = 0; i < Oscillators; i++)
                        osc[i].phase = params[Params::RandomPhase1 + i] > 0.5 ? (std::rand() % 32767) / 32767. : 0;
                    for (int i = 0; i < LFOs; i++)
                        lfo[i].phase = 0;
                    sub.phase = 0;
                }
                
                for (int i = 0; i < Envelopes; i++)
                {
                    env[i].settings.legato = params[Params::Retrigger] < 0.5;
                    env[i].Gate(true);
                }
                break;
            case Event::kNoteOffEvent:

                notesPressed[event.noteOff.pitch] = false;

                if (event.noteOff.pitch != pressed)
                    break;

                bool _p = false;
                for (int i = 127; i >= 0; i--)
                    if (notesPressed[i])
                    {
                        _p = true;
                        pressedOld = frequency;
                        pressed = i;
                        key = i / 127.;

                        if (params[Params::Retrigger] > 0.5)
                        {
                            sub.phase = 0;
                            for (int i = 0; i < Oscillators; i++)
                                osc[i].phase = params[Params::RandomPhase1 + i] ? (std::rand() % 32767) / 32767. : 0;
                            for (int i = 0; i < LFOs; i++)
                                lfo[i].phase = 0;
                        }

                        for (int i = 0; i < Envelopes; i++)
                        {
                            env[i].settings.legato = params[Params::Retrigger] < 0.5;
                            env[i].Gate(true);
                        }

                        break;
                    }

                if (!_p) // If no notes pressed, gates off
                    for (int i = 0; i < Envelopes; i++)
                        env[i].Gate(false);
                break;
            }
        }

        double GenerateSample(size_t channel, ProcessData& data, double samplerate)
        {
            double bpm = 128;
            if (data.processContext->state & ProcessContext::kTempoValid)
                bpm = data.processContext->tempo;

            if (channel == 0) // Only do all generating on channel 0
            {
                const double _bendOffset = params[Params::PitchBend] * 12 * modulated[Params::Bend] - 6 * modulated[Params::Bend] + modulated[Params::Transpose] * 96 - 48;
                const double _timeMult = modulated[Params::Time] < 0.5 ? (modulated[Params::Time] + 0.5) : ((modulated[Params::Time] - 0.5) * 2 + 1);

                for (int i = 0; i < Oscillators; i++)
                {   // Oscillator generate
                    if (modulated[Params::Volume1 + i] && params[Params::Enable1 + i] > 0.5) // Generate oscillator sound
                        osc[i].sample = osc[i].OffsetOnce(std::fmod(modulated[Params::Phase1 + i] + 5, 1.));
                }

                { // Sub oscillator
                    int _octave = std::round(params[Params::SubOct] * 4 - 2);
                    sub.SAMPLE_RATE = samplerate;
                    sub.settings.frequency = noteToFreq(frequency + _octave * 12 + +_bendOffset);
                    sub.settings.wtpos = modulated[Params::SubOvertone];
                    sub.sample = sub.OffsetOnce(0);
                }
            }

            double _cs[Combines * 2 + 1]{ 0, 0, 0, 0, 0, 0, 0 };
            for (int i = 0; i < Oscillators; i++)
            {
                if (!(modulated[Params::Volume1 + i] && params[Params::Enable1 + i] > 0.5)) continue;
                 
                double _vs = osc[i].sample + modulated[Params::DCOff1 + i] * 2 - 1;

                // Fold
                if (params[Params::ENBFold1 + i] > 0.5)
                    _vs = Shapers::fold(_vs * (modulated[Params::Fold1 + i] * 15 + 1), modulated[Params::Bias1 + i] * 2 - 1);

                // Noise
                if (modulated[Params::Noise1 + i])
                    _vs += modulated[Params::Noise1 + i] * random(); // Add noise

                // Drive
                if (params[Params::ENBDrive1 + i] > 0.5)
                    _vs = Shapers::drive(_vs, modulated[Params::DriveGain1 + i] * 3 + 1, modulated[Params::DriveAmt1 + i]);
                else
                    _vs = std::max(std::min(_vs, 1.), -1.);

                // Filter
                if (!filterp[i].off && params[Params::ENBFilter1 + i] > 0.5)
                    _vs = filter[i].Apply(_vs, channel); // Apply pre-combine filter

                // Gain
                _vs *= modulated[Params::Volume1 + i]; // Adjust for volume

                // Pan
                if (modulated[Params::Pan1 + i] != 0.5)
                    _vs *= std::min((channel == 1 ? 2 * modulated[Params::Pan1 + i] : 2 - 2 * modulated[Params::Pan1 + i]), 1.); // Panning

                size_t _dests = params[Params::DestA + i] * 128.;
                for (int j = 0; j < Combines * 2 + 1; j++)
                    if ((_dests >> j) & 1U) _cs[j] += _vs;
            }

            double _res = modulated[Params::SubGain] * sub.sample; // Start with sub

            for (int i = 0; i < Combines; i++)
            {
                constexpr static size_t _offsets[]{ 5, 3, 0 };
                constexpr static size_t _mult[]{ 32, 8, 0 };
                double _v = Combine(
                    _cs[i * 2] * modulated[Params::PreGainX + i], 
                    _cs[i * 2 + 1] * modulated[Params::PreGainX + i], 
                    i);

                // Fold
                if (params[Params::ENBFoldX + i] > 0.5)
                    _v = Shapers::fold(_v * (modulated[Params::FoldX + i] * 15 + 1), modulated[Params::BiasX + i] * 2 - 1);

                // Drive
                if (params[Params::ENBDriveX + i] > 0.5)
                    _v = Shapers::drive(_v, modulated[Params::DriveGainX + i] * 3 + 1, modulated[Params::DriveAmtX + i]);
                else
                    _v = std::max(std::min(_v, 1.), -1.);

                _v = _v * modulated[Params::GainX + i];

                if (!cfilterp[i].off) _v = cfilter[i].Apply(_v, channel);
                _v = params[Params::DCX + i] > 0.5 ? dcoff[i].Apply(_v, channel) : _v;

                if (i == Combines - 1)
                {
                    _res += _v;
                    break;
                }

                const size_t _dests = params[Params::DestX + i] * _mult[i];
                for (int j = 0; j < _offsets[i]; j++)
                    if ((_dests >> j) & 1U) _cs[j + (i + 1) * 2] += _v;
            }

            _res += _cs[Combines * 2];

            _res *= std::min((channel == 1 ? 2 * modulated[Params::Panning] : 2 - 2 * modulated[Params::Panning]), 1.); // Panning

            return env[0].Apply(_res, channel); // Envelope 0 is gain
        }

        int count = 0;
        std::pair<Sample64, Sample64> Generate(ProcessData& data) override
        {
            bool _editNow = count >= 512;
            for (int m = 0; m < Params::ModCount; m++)
            {
                if (_editNow && controller) {
                    count = 0;
                    controller->updateModulation((Params)m, modulated[m]);
                }
            }
            count++;

            const size_t _index = std::floor(params[Params::Oversample] * 4);
            const size_t _osa = _index == 0 ? 1 : _index == 1 ? 2 : _index == 2 ? 4 : 8;

            const double samplerate = data.processContext->sampleRate * _osa;

            // Move params to modulated so we can adjust their values
            std::memcpy(modulated, params, Params::ModCount * sizeof(double));
            for (int m = 0; m < Params::ModCount; m++)
            {
                bool edited = false;
                for (int i = 0; i < ModAmt; i++)
                {
                    int index = m * ModAmt + i;
                    int source = (modgoals[index] * (int)ModSources::Amount);
                    if (source == 0) continue;
                    edited = true;
                    double amount = modamount[index] * 2 - 1;

                    if (source == (int)ModSources::Vel)
                    {
                        modulated[m] += (velocity * amount);
                    }
                    else if (source == (int)ModSources::Key)
                    {
                        modulated[m] += (key * amount);
                    }
                    else if (source >= (int)ModSources::Osc1)
                    {
                        int mindex = (source - (int)ModSources::Osc1);
                        modulated[m] += (osc[mindex].sample * amount);
                    }
                    else if (source >= (int)ModSources::Mac1)
                    {
                        int mindex = (source - (int)ModSources::Mac1);
                        modulated[m] += (params[Params::Macro1 + mindex] * amount);
                    }
                    else if (source >= (int)ModSources::Env1)
                    {
                        int mindex = (source - (int)ModSources::Env1);
                        modulated[m] += (env[mindex].sample * amount);
                    }
                    else
                    {
                        int mindex = (source - (int)ModSources::LFO1);
                        modulated[m] += ((params[Params::LFOLvl1 + mindex] * 2 - 1) * lfo[mindex].sample * amount);
                    }
                }

                if (edited && ParamNames[m].constrain)
                    modulated[m] = constrain(modulated[m], 0., 1.);
            }

            // If envelope is done, no sound so return 0
            if (env[0].Done()) return { 0, 0 };

            double bpm = 128;
            if (data.processContext->state & ProcessContext::kTempoValid)
                bpm = data.processContext->tempo;

            const double _bendOffset = params[Params::PitchBend] * 12 * modulated[Params::Bend] - 6 * modulated[Params::Bend] + modulated[Params::Transpose] * 96 - 48;
            const double _timeMult = modulated[Params::Time] < 0.5 ? (modulated[Params::Time] + 0.5) : ((modulated[Params::Time] - 0.5) * 2 + 1);
            
            deltaf = _timeMult * std::abs((pressed - pressedOld) / (0.001 + modulated[Params::Glide] * modulated[Params::Glide] * modulated[Params::Glide] * 10 * data.processContext->sampleRate));
            if (frequency > pressed)
                if (frequency - deltaf < pressed) frequency = pressed;
                else frequency -= deltaf;

            if (frequency < pressed)
                if (frequency + deltaf > pressed) frequency = pressed;
                else frequency += deltaf;

            for (int i = 0; i < Envelopes; i++)
            {   // Adjust Envelope parameters and generate
                env[i].SAMPLE_RATE = data.processContext->sampleRate;
                env[i].settings.attack = modulated[Params::Env1A + i] * modulated[Params::Env1A + i] * modulated[Params::Env1A + i] * 5;
                env[i].settings.attackCurve = modulated[Params::Env1AC + i] * 2 - 1;
                env[i].settings.attackLevel = modulated[Params::Env1AL + i];
                env[i].settings.decay = modulated[Params::Env1D + i] * modulated[Params::Env1D + i] * modulated[Params::Env1D + i] * 5;
                env[i].settings.decayCurve = modulated[Params::Env1DC + i] * 2 - 1;
                env[i].settings.decayLevel = modulated[Params::Env1DL + i];
                env[i].settings.sustain = modulated[Params::Env1S + i];
                env[i].settings.release = modulated[Params::Env1R + i] * modulated[Params::Env1R + i] * modulated[Params::Env1R + i] * 5;
                env[i].settings.releaseCurve = modulated[Params::Env1RC + i] * 2 - 1;
                env[i].settings.timeMult = _timeMult;
                env[i].Generate(0);
            }

            for (int i = 0; i < LFOs; i++)
            {   // Adjust lfo parameters 
                lfo[i].SAMPLE_RATE = data.processContext->sampleRate;

                if (params[Params::LFOSync1 + i] > 0.5) // If bpm synced lfo
                {
                    size_t _type = std::floor(params[Params::LFORate1 + i] * (TimesAmount - 1));
                    double _ps = (bpm / 60.) / (TimesValue[_type] * 4); // (beats per phase / bpm) / 60 = seconds per phase
                    lfo[i].settings.frequency = _ps;
                }
                else
                    lfo[i].settings.frequency = _timeMult * modulated[Params::LFORate1 + i] * modulated[Params::LFORate1 + i] * 29.9 + 0.1;
                lfo[i].settings.wtpos = modulated[Params::LFOPos1 + i];
                lfo[i].settings.shaper3 = modulated[Params::LFOShaper1 + i];
                lfo[i].sample = lfo[i].OffsetOnce(modulated[Params::LFOPhase1 + i]);
            }

            for (int i = 0; i < Combines; i++)
            {   // Combines parameters
                dcoffp[i].f0 = 35;
                dcoffp[i].type = FilterType::HighPass;
                dcoffp[i].Q = 1;
                dcoffp[i].sampleRate = samplerate;
                dcoffp[i].RecalculateParameters();

                auto _ft = std::floor(params[Params::FilterX + i] * 3); // Filter parameters
                cfilterp[i].sampleRate = samplerate;
                cfilterp[i].f0 = modulated[Params::FreqX + i] * modulated[Params::FreqX + i] * (22000 - 20) + 20;
                cfilterp[i].Q = modulated[Params::ResoX + i] * 16 + 1;
                cfilterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                cfilterp[i].RecalculateParameters();
            }

            for (int i = 0; i < Oscillators; i++)
            {
                osc[i].SAMPLE_RATE = samplerate; 
                osc[i].settings.frequency = noteToFreq(frequency + _bendOffset
                    + modulated[Params::Detune1 + i] * 4 - 2 + modulated[Params::Pitch1 + i] * 48 - 24);
                osc[i].settings.wtpos = modulated[Params::WTPos1 + i];
                osc[i].settings.sync = modulated[Params::Sync1 + i];
                osc[i].settings.pw = modulated[Params::PulseW1 + i];
                osc[i].settings.bend = modulated[Params::Bend1 + i];
                osc[i].settings.shaper = modulated[Params::Shaper1 + i];
                osc[i].settings.shaperMix = modulated[Params::ShaperMix1 + i] * (params[Params::ENBShaper1 + i] > 0.5);
                osc[i].settings.shaper2 = modulated[Params::Shaper21 + i];
                osc[i].settings.shaper2Mix = modulated[Params::Shaper2Mix1 + i] * (params[Params::ENBShaper1 + i] > 0.5);
                osc[i].settings.shaperMorph = modulated[Params::ShaperMorph1 + i];

                auto _ft = std::floor(params[Params::Filter1 + i] * 3); // Filter parameters
                filterp[i].sampleRate = samplerate;
                filterp[i].f0 = modulated[Params::Freq1 + i] * modulated[Params::Freq1 + i] * (22000 - 30) + 30;
                filterp[i].Q = modulated[Params::Reso1 + i] * 16 + 1;
                filterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                filterp[i].RecalculateParameters();
            }

            if (_osa == 1)
            {
                const double l = Clip(GenerateSample(0, data, samplerate), 0);
                const double r = Clip(GenerateSample(1, data, samplerate), 1);
                return { l, r };
            }

            aafp.sampleRate = samplerate;
            aafp.f0 = data.processContext->sampleRate * 0.4;
            aafp.Q = 0.6;
            aafp.type = FilterType::LowPass;
            aafp.RecalculateParameters();

            double _suml = 0;
            double _sumr = 0;
            for (int k = 0; k < _osa; k++)
            {
                double _l = GenerateSample(0, data, samplerate);
                double _r = GenerateSample(1, data, samplerate);

                _l = aaf.Apply(_l, 0);
                _r = aaf.Apply(_r, 1);
                
                _suml += _l;
                _sumr += _r;
            }

            return { Clip(_suml / _osa, 0), Clip(_sumr / _osa, 1) };
        }

        enum CombineMode { ADD, MIN, MULT, PONG, MAX, MOD, AND, INLV, OR, XOR, Size };

        inline double Clip(double a, int channel)
        {
            const size_t f = std::floor(params[Params::Clipping] * 3);
            switch (f)
            {
            case 0: a = std::tanh(a) * 1.312; break;
            case 1: a = (1.99 * a) / (1 + std::abs(a)); break;
            }
            return constrain(a, -1., 1.);
        }

        inline double Combine(double a, double b, int index)
        {
            const auto mmin = modulated[Params::MinMixX + index] * CombineSingle(a, b, MIN);
            const auto mult = modulated[Params::MultMixX + index] * CombineSingle(a, b, MULT);
            const auto pong = modulated[Params::PongMixX + index] * CombineSingle(a, b, PONG);
            const auto mmax = modulated[Params::MaxMixX + index] * CombineSingle(a, b, MAX);
            const auto mmod = modulated[Params::ModMixX + index] * CombineSingle(a, b, MOD);
            const auto mand = modulated[Params::AndMixX + index] * CombineSingle(a, b, AND);
            const auto inlv = modulated[Params::InlvMixX + index] * CombineSingle(a, b, INLV);
            const auto mmor = modulated[Params::OrMixX + index] * CombineSingle(a, b, OR);
            const auto mxor = modulated[Params::XOrMixX + index] * CombineSingle(a, b, XOR);
            const auto madd = modulated[Params::AddMixX + index] * CombineSingle(a, b, ADD);
            
            const auto multiplier = 1 + modulated[Params::MinMixX + index] + modulated[Params::MultMixX + index]
                + modulated[Params::PongMixX + index] + modulated[Params::MaxMixX + index] + modulated[Params::ModMixX + index]
                + modulated[Params::AndMixX + index] + modulated[Params::InlvMixX + index] + modulated[Params::OrMixX + index]
                + modulated[Params::XOrMixX + index] + modulated[Params::AddMixX + index];

            return modulated[Params::MixX + index] * 2 * (1 / multiplier) * (mmin + mult + pong + mmax + mmod + mand + inlv + mmor + mxor + madd) + (1 - modulated[Params::MixX + index]) * (a + b);
        }

        inline double CombineSingle(double a, double b, int mode)
        {
            const uint64_t _a = std::bit_cast<uint64_t>(a);
            const uint64_t _b = std::bit_cast<uint64_t>(b);
            const uint64_t _as = a * 4294967296;
            const uint64_t _bs = b * 4294967296;
            double _ab = 0;
            switch (mode) {
            case AND:  _ab = 1.5 * (std::bit_cast<double>(_a & _b)); break;
            case OR:   _ab = 0.3 * (std::bit_cast<double>(_a | _b)); if (std::isnan(_ab)) _ab = 0; break;
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
        double frequency = 40;
        double key = 0;
        double pressed = 0;
        double pressedOld = 0;
        double deltaf = 0;
        double velocity = 1;

        Oscillator sub{ {.wavetable = Wavetables::sub } };

        BiquadParameters aafp;
        StereoEqualizer<2, BiquadFilter<>> aaf{ aafp };

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