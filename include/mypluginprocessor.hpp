#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "Knob.hpp"
#include "ModeView.hpp"
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
 * X LFO Sync not synced
 * X LFO global sync
 * X Noise Color
 * X Top bar (save/load preset)
 * X Random phase button
 * X Parameter color when disabled
 * X Alignment of parameters sub oscillator
 * X Enable button of combiner filter
 * X Globalize all colors
 * X Change big waveform display to double click, and add to lfo and envelope
 * X Rename Drive in Combiner section to Shape
 * X Filter Disable Combiner not implemented
 *
 * = Better Drive algorithm
 * = LFO/Envelope modulation redraw
 * = 'Display big waveform' toggle as parameter so it's saved.
 * = Change LFO speed = mouseX, Amount = mouseY
 * 
 */

namespace Kaixo
{
    class Instrument : public AudioEffect
    {
    public:
        Instrument() { setControllerClass(kCMBNEXControllerUID); }
        ~Instrument() override {};

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


            double _samples = data.processContext->projectTimeSamples;

            if (data.symbolicSampleSize == kSample32)
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    for (size_t param = 0; param < Params::Size; param++)
                        params[param] = ParamNames[param].smooth ? params[param] * 0.99 + paramgoals[param] * 0.01 : paramgoals[param];

                    auto [l, r] = Generate(data, _samples + sample);
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

                    auto[l, r] = Generate(data, _samples + sample);
                    if (data.outputs[0].numChannels == 2)
                    {
                        ((Sample64**)out)[0][sample] = l;
                        ((Sample64**)out)[1][sample] = r;
                    }
                }
            }
            return kResultOk;
        }

        String preset = "default";

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            preset = streamer.readStr8();

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

            streamer.writeStr8(preset);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(paramgoals[i]);

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                streamer.writeDouble(modgoals[i]);
                streamer.writeDouble(modamount[i]);
            }

            return kResultOk;
        }

        virtual std::pair<Sample64, Sample64> Generate(ProcessData& data, double samples) = 0;
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
        constexpr static auto Voices = 6;

        static FUnknown* createInstance(void*) { return static_cast<IAudioProcessor*>(new TestProcessor); }

        bool notesPressed[128] { 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        };
        
        double samples = 0;

        void NotePress(Vst::Event& event)
        {
            // Release the longest held note
            if (m_Available.size() == 0)
            {
                int longestheld = m_Pressed.back();
                m_Pressed.pop_back();

                // Set note to -1 and emplace to available.
                m_Notes[longestheld] = -1;
                m_Available.emplace(m_Available.begin(), longestheld);
            }

            // Get an available voice
            if (!m_Available.empty())
            {
                int voice = m_Available.back();
                m_Available.pop_back();

                // Emplace it to pressed voices queue
                m_Pressed.emplace(m_Pressed.begin(), voice);

                // Set voice to note
                m_Notes[voice] = event.noteOn.pitch;
                {
                    voices[voice].velocity = event.noteOn.velocity;
                    voices[voice].pressedOld = voices[voice].frequency;
                    voices[voice].pressed = event.noteOn.pitch + event.noteOn.tuning * 0.01;
                    voices[voice].key = event.noteOn.pitch;

                    if (params[Params::Retrigger] > 0.5 || !voices[voice].env[0].Gate())
                    {
                        for (int i = 0; i < Oscillators; i++)
                            voices[voice].osc[i].phase = params[Params::RandomPhase1 + i] > 0.5 ? (std::rand() % 32767) / 32767. : 0;
                        for (int i = 0; i < LFOs; i++)
                            voices[voice].lfo[i].phase = 0;
                        voices[voice].sub.phase = 0;
                        voices[voice].samplesPress = samples;
                    }

                    for (int i = 0; i < Envelopes; i++)
                    {
                        voices[voice].env[i].settings.legato = params[Params::Retrigger] < 0.5;
                        voices[voice].env[i].Gate(true);
                    }
                }
            }
        }

        void NoteRelease(Vst::Event& event)
        {
            // Find the note in the pressed notes per voice
            while (true)
            {
                auto it = std::find(m_Notes.begin(), m_Notes.end(), event.noteOff.pitch);
                if (it != m_Notes.end())
                {
                    // If it was pressed, get the voice index
                    int voice = std::distance(m_Notes.begin(), it);

                    // Set note to -1 and emplace to available.
                    {
                        for (int i = 0; i < Envelopes; i++)
                            voices[voice].env[i].Gate(false);
                    }
                    m_Notes[voice] = -1;
                    m_Available.emplace(m_Available.begin(), voice);

                    // Erase it from the pressed queue
                    auto it2 = std::find(m_Pressed.begin(), m_Pressed.end(), voice);
                    if (it2 != m_Pressed.end())
                        m_Pressed.erase(it2);
                }
                else break;
            }
        }

        std::array<int, Voices> m_Notes{ -1, -1, -1, -1, -1, -1 };
        std::vector<int> m_Pressed;
        std::vector<int> m_Available{ 0, 1, 2, 3, 4, 5 };

        void Event(Vst::Event& event) override
        {
            switch (event.type)
            {
            case Event::kNoteOnEvent:
                if (paramgoals[Params::PitchBend] > 1 || paramgoals[Params::PitchBend] < 0) paramgoals[Params::PitchBend] = 0.5;

                NotePress(event);

                //velocity = event.noteOn.velocity;
                //pressedOld = frequency;
                //pressed = event.noteOn.pitch + event.noteOn.tuning * 0.01;
                //key = event.noteOn.pitch;
                //notesPressed[event.noteOn.pitch] = true;
                //if (params[Params::Retrigger] > 0.5 || !env[0].Gate())
                //{ 
                //    for (int i = 0; i < Oscillators; i++)
                //        osc[i].phase = params[Params::RandomPhase1 + i] > 0.5 ? (std::rand() % 32767) / 32767. : 0;
                //    for (int i = 0; i < LFOs; i++)
                //        lfo[i].phase = 0;
                //    sub.phase = 0;
                //    samplesPress = samples;
                //}
                
                //for (int i = 0; i < Envelopes; i++)
                //{
                //    env[i].settings.legato = params[Params::Retrigger] < 0.5;
                //    env[i].Gate(true);
                //}
                break;
            case Event::kNoteOffEvent:

                NoteRelease(event);

                //notesPressed[event.noteOff.pitch] = false;

                //if (event.noteOff.pitch != pressed)
                //    break;
                //
                //bool _p = false;
                //for (int i = 127; i >= 0; i--)
                //    if (notesPressed[i])
                //    {
                //        _p = true;
                //        pressedOld = frequency;
                //        pressed = i;
                //        key = i / 127.;
                //
                //        if (params[Params::Retrigger] > 0.5)
                //        {
                //            sub.phase = 0;
                //            for (int i = 0; i < Oscillators; i++)
                //                osc[i].phase = params[Params::RandomPhase1 + i] ? (std::rand() % 32767) / 32767. : 0;
                //            for (int i = 0; i < LFOs; i++)
                //                lfo[i].phase = 0;
                //            samplesPress = samples;
                //        }
                //
                //        for (int i = 0; i < Envelopes; i++)
                //        {
                //            env[i].settings.legato = params[Params::Retrigger] < 0.5;
                //            env[i].Gate(true);
                //        }
                //
                //        break;
                //    }
                //
                //if (!_p) // If no notes pressed, gates off
                //    for (int i = 0; i < Envelopes; i++)
                //        env[i].Gate(false);
                break;
            }
        }

        double GenerateSample(size_t channel, ProcessData& data, double samplerate)
        {
            double bpm = 128;
            if (data.processContext->state & ProcessContext::kTempoValid)
                bpm = data.processContext->tempo;

            double _out = 0;
            for (auto& voice : voices)
            {
                if (voice.env[0].Done()) continue;

                const double _bendOffset = params[Params::PitchBend] * 12 * voice.modulated[Params::Bend] - 6 * voice.modulated[Params::Bend] + voice.modulated[Params::Transpose] * 96 - 48;
                const double _timeMult = voice.modulated[Params::Time] < 0.5 ? (voice.modulated[Params::Time] + 0.5) : ((voice.modulated[Params::Time] - 0.5) * 2 + 1);

                if (channel == 0) // Only do all generating on channel 0
                {

                    for (int i = 0; i < Oscillators; i++)
                    {   // Oscillator generate
                        if (voice.modulated[Params::Volume1 + i] && params[Params::Enable1 + i] > 0.5) // Generate oscillator sound
                            voice.osc[i].sample = voice.osc[i].OffsetOnce(myfmod1(voice.modulated[Params::Phase1 + i] + 5));
                    }

                    { // Sub oscillator
                        int _octave = std::round(params[Params::SubOct] * 4 - 2);
                        voice.sub.SAMPLE_RATE = samplerate;
                        voice.sub.settings.frequency = noteToFreq(voice.frequency + _octave * 12 + +_bendOffset);
                        voice.sub.settings.wtpos = voice.modulated[Params::SubOvertone];
                        voice.sub.sample = voice.sub.OffsetOnce(0);
                    }
                }

                double _cs[Combines * 2 + 1]{ 0, 0, 0, 0, 0, 0, 0 };
                for (int i = 0; i < Oscillators; i++)
                {
                    if (!(voice.modulated[Params::Volume1 + i] && params[Params::Enable1 + i] > 0.5)) continue;

                    voice.oscs[i] = voice.osc[i].sample + voice.modulated[Params::DCOff1 + i] * 2 - 1;

                    // Fold
                    if (params[Params::ENBFold1 + i] > 0.5)
                        voice.oscs[i] = Shapers::fold(voice.oscs[i] * (voice.modulated[Params::Fold1 + i] * 15 + 1), voice.modulated[Params::Bias1 + i] * 2 - 1);

                    // Noise
                    if (params[Params::ENBNoise1 + i] > 0.5)
                        voice.oscs[i] += voice.modulated[Params::Noise1 + i] * voice.noiself[i].Apply(voice.noisehf[i].Apply(random(), channel), channel); // Add noise

                    // Drive
                    if (params[Params::ENBDrive1 + i] > 0.5)
                        voice.oscs[i] = Shapers::drive(voice.oscs[i], voice.modulated[Params::DriveGain1 + i] * 3 + 1, voice.modulated[Params::DriveAmt1 + i]);
                    else
                        voice.oscs[i] = std::max(std::min(voice.oscs[i], 1.), -1.);

                    // Filter
                    if (!voice.filterp[i].off && params[Params::ENBFilter1 + i] > 0.5)
                        voice.oscs[i] = voice.filter[i].Apply(voice.oscs[i], channel); // Apply pre-combine filter

                    // Gain
                    voice.oscs[i] *= voice.modulated[Params::Volume1 + i]; // Adjust for volume
                    double _vs = voice.oscs[i];

                    // Pan
                    if (voice.modulated[Params::Pan1 + i] != 0.5)
                        _vs *= std::min((channel == 1 ? 2 * voice.modulated[Params::Pan1 + i] : 2 - 2 * voice.modulated[Params::Pan1 + i]), 1.); // Panning

                    size_t _dests = params[Params::DestA + i] * 128.;
                    for (int j = 0; j < Combines * 2 + 1; j++)
                        if ((_dests >> j) & 1U) _cs[j] += _vs;
                }

                double _res = voice.modulated[Params::SubGain] * voice.sub.sample; // Start with sub

                for (int i = 0; i < Combines; i++)
                {
                    constexpr static size_t _offsets[]{ 5, 3, 0 };
                    constexpr static size_t _mult[]{ 32, 8, 0 };
                    double _v = Combine(
                        _cs[i * 2] * voice.modulated[Params::PreGainX + i],
                        _cs[i * 2 + 1] * voice.modulated[Params::PreGainX + i],
                        i, voice);

                    // Fold
                    if (params[Params::ENBFoldX + i] > 0.5)
                        _v = Shapers::fold(_v * (voice.modulated[Params::FoldX + i] * 15 + 1), voice.modulated[Params::BiasX + i] * 2 - 1);

                    // Drive
                    if (params[Params::ENBDriveX + i] > 0.5)
                        _v = Shapers::drive(_v, voice.modulated[Params::DriveGainX + i] * 3 + 1, voice.modulated[Params::DriveAmtX + i]);
                    else
                        _v = std::max(std::min(_v, 1.), -1.);

                    _v = _v * voice.modulated[Params::GainX + i];

                    if (params[Params::ENBFilterX + i] > 0.5)
                        _v = voice.cfilter[i].Apply(_v, channel);

                    _v = params[Params::DCX + i] > 0.5 ? voice.dcoff[i].Apply(_v, channel) : _v;

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

                _res *= std::min((channel == 1 ? 2 * voice.modulated[Params::Panning] : 2 - 2 * voice.modulated[Params::Panning]), 1.); // Panning

                _out += Clip(voice.env[0].Apply(_res, channel), channel); // Envelope 0 is gain
            }

            return _out;
        }

        bool work = true;

        ~TestProcessor()
        {
            work = false;
            modulationThread.join();
        }

        std::thread modulationThread{ [this]() {
            while (work)
            {
                for (int m = 0; m < Params::ModCount; m++)
                {
                    auto message = Steinberg::owned(allocateMessage());

                    if (message)
                    {
                        message->setMessageID(UPDATE_MODULATION);
                        message->getAttributes()->setInt(UPDATE_MODULATION_PARAM, m);
                        message->getAttributes()->setFloat(UPDATE_MODULATION_VALUE, voices[0].modulated[m]);

                        sendMessage(message);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
            }
        } };

        int count = 0;
        std::pair<Sample64, Sample64> Generate(ProcessData& data, double s) override
        {
            
            samples = s;
            //bool _editNow = count >= data.processContext->sampleRate / 60.;
            //count++;

            const size_t _index = std::floor(params[Params::Oversample] * 4);
            const size_t _osa = _index == 0 ? 1 : _index == 1 ? 2 : _index == 2 ? 4 : 8;

            const double samplerate = data.processContext->sampleRate * _osa;

            for (auto& voice : voices)
            {
                // Move params to modulated so we can adjust their values
                std::memcpy(voice.modulated, params, Params::ModCount * sizeof(double));
            }

            for (int m = 0; m < Params::ModCount; m++)
            {
                bool edited = false;
                for (int i = 0; i < ModAmt; i++)
                {
                    int index = m * ModAmt + i;
                    int source = (modgoals[index] * (int)ModSources::Amount + 0.1);
                    if (source == 0) continue;
                    edited = true;
                    double amount = modamount[index] * 2 - 1;

                    if (source == (int)ModSources::Vel)
                    {
                        for (auto& voice : voices)
                        voice.modulated[m] += (voice.velocity * amount);
                    }
                    else if (source == (int)ModSources::Key)
                    {
                        for (auto& voice : voices)
                        {
                            const double _bendOffset = params[Params::PitchBend] * 12 * voice.modulated[Params::Bend] - 6 * voice.modulated[Params::Bend] + voice.modulated[Params::Transpose] * 96 - 48;
                            voice.modulated[m] += (((voice.key + _bendOffset) / 127.0) * amount);
                        }
                    }
                    else if (source >= (int)ModSources::Mac1)
                    {
                        int mindex = (source - (int)ModSources::Mac1);

                        for (auto& voice : voices)
                        voice.modulated[m] += (voice.modulated[Params::Macro1 + mindex] * amount);
                    }
                    else if (source >= (int)ModSources::Env1)
                    {
                        int mindex = (source - (int)ModSources::Env1);
                        for (auto& voice : voices)
                        voice.modulated[m] += (voice.env[mindex].sample * amount);
                    }
                    else
                    {
                        int mindex = (source - (int)ModSources::LFO1);
                        for (auto& voice : voices)
                        voice.modulated[m] += (voice.lfo[mindex].sample * amount);
                    }
                    
                }

                if (edited && ParamNames[m].constrain)
                    for (auto& voice : voices)
                        voice.modulated[m] = constrain(voice.modulated[m], 0., 1.);
            }

            for (auto& voice : voices)
            {
                double _bendOffset = params[Params::PitchBend] * 12 * voice.modulated[Params::Bend] - 6 * voice.modulated[Params::Bend] + voice.modulated[Params::Transpose] * 96 - 48;

                // If envelope is done, no sound so return 0
                if (voice.env[0].Done()) continue;

                double bpm = 128;
                if (data.processContext->state & ProcessContext::kTempoValid)
                    bpm = data.processContext->tempo;

                const double _timeMult = voice.modulated[Params::Time] < 0.5 ? (voice.modulated[Params::Time] + 0.5) : ((voice.modulated[Params::Time] - 0.5) * 2 + 1);

                voice.deltaf = _timeMult * std::abs((voice.pressed - voice.pressedOld) / (0.001 + voice.modulated[Params::Glide] * voice.modulated[Params::Glide] * voice.modulated[Params::Glide] * 10 * data.processContext->sampleRate));
                if (voice.frequency > voice.pressed)
                    if (voice.frequency - voice.deltaf < voice.pressed) voice.frequency = voice.pressed;
                    else voice.frequency -= voice.deltaf;

                if (voice.frequency < voice.pressed)
                    if (voice.frequency + voice.deltaf > voice.pressed) voice.frequency = voice.pressed;
                    else voice.frequency += voice.deltaf;

                for (int i = 0; i < Envelopes; i++)
                {   // Adjust Envelope parameters and generate
                    voice.env[i].SAMPLE_RATE = data.processContext->sampleRate;
                    voice.env[i].settings.attack = voice.modulated[Params::Env1A + i] * voice.modulated[Params::Env1A + i] * voice.modulated[Params::Env1A + i] * 5;
                    voice.env[i].settings.attackCurve = voice.modulated[Params::Env1AC + i] * 2 - 1;
                    voice.env[i].settings.attackLevel = voice.modulated[Params::Env1AL + i];
                    voice.env[i].settings.decay = voice.modulated[Params::Env1D + i] * voice.modulated[Params::Env1D + i] * voice.modulated[Params::Env1D + i] * 5;
                    voice.env[i].settings.decayCurve = voice.modulated[Params::Env1DC + i] * 2 - 1;
                    voice.env[i].settings.decayLevel = voice.modulated[Params::Env1DL + i];
                    voice.env[i].settings.sustain = voice.modulated[Params::Env1S + i];
                    voice.env[i].settings.release = voice.modulated[Params::Env1R + i] * voice.modulated[Params::Env1R + i] * voice.modulated[Params::Env1R + i] * 5;
                    voice.env[i].settings.releaseCurve = voice.modulated[Params::Env1RC + i] * 2 - 1;
                    voice.env[i].settings.timeMult = _timeMult;
                    voice.env[i].Generate(0);
                }

                for (int i = 0; i < LFOs; i++)
                {   // Adjust lfo parameters 
                    voice.lfo[i].SAMPLE_RATE = data.processContext->sampleRate;

                    if (params[Params::LFOSync1 + i] > 0.5) // If bpm synced lfo
                    {
                        size_t _type = std::floor(params[Params::LFORate1 + i] * (TimesAmount - 1));

                        double _ps = (60. / bpm) * TimesValue[_type] * 4; // Seconds per oscillation
                        double _f = 1 / _ps; // Frequency of the oscillations
                        double _spp = _ps * data.processContext->sampleRate; // Samples per oscillation
                        double _phase = std::fmod(params[Params::LFORetr1 + i] > 0.5 ? samples - voice.samplesPress : samples, _spp) / _spp;

                        // If playing, sync phase to samples played
                        if (data.processContext->state & ProcessContext::kPlaying)
                            voice.lfo[i].phase = _phase;
                        voice.lfo[i].settings.frequency = _f;
                    }
                    else
                    {
                        size_t _type = std::floor(params[Params::LFORate1 + i] * (TimesAmount - 1));

                        double _f = _timeMult * voice.modulated[Params::LFORate1 + i] * voice.modulated[Params::LFORate1 + i] * 29.9 + 0.1; // Frequency of the oscillations
                        double _ps = 1 / _f; // Seconds per oscillation
                        double _spp = _ps * data.processContext->sampleRate; // Samples per oscillation
                        double _phase = std::fmod(params[Params::LFORetr1 + i] > 0.5 ? samples - voice.samplesPress : samples, _spp) / _spp;

                        // If playing, sync phase to samples played
                        if (data.processContext->state & ProcessContext::kPlaying)
                            voice.lfo[i].phase = _phase;
                        voice.lfo[i].settings.frequency = _f;
                    }

                    voice.lfo[i].settings.wtpos = voice.modulated[Params::LFOPos1 + i];
                    voice.lfo[i].settings.shaper3 = voice.modulated[Params::LFOShaper1 + i];
                    voice.lfo[i].sample = voice.lfo[i].OffsetOnce(voice.modulated[Params::LFOPhase1 + i]) * (voice.modulated[Params::LFOLvl1 + i] * 2 - 1);
                }

                for (int i = 0; i < Combines; i++)
                {   // Combines parameters
                    voice.dcoffp[i].f0 = 35;
                    voice.dcoffp[i].type = FilterType::HighPass;
                    voice.dcoffp[i].Q = 1;
                    voice.dcoffp[i].sampleRate = samplerate;
                    voice.dcoffp[i].RecalculateParameters();

                    auto _ft = std::floor(params[Params::FilterX + i] * 3); // Filter parameters
                    voice.cfilterp[i].sampleRate = samplerate;
                    voice.cfilterp[i].f0 = voice.modulated[Params::FreqX + i] * voice.modulated[Params::FreqX + i] * (22000 - 30) + 30;
                    voice.cfilterp[i].Q = voice.modulated[Params::ResoX + i] * 16 + 1;
                    voice.cfilterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                    voice.cfilterp[i].RecalculateParameters();
                }

                for (int i = 0; i < Oscillators; i++)
                {
                    voice.osc[i].SAMPLE_RATE = samplerate;
                    voice.osc[i].settings.frequency = noteToFreq(voice.frequency + _bendOffset
                        + voice.modulated[Params::Detune1 + i] * 4 - 2 + voice.modulated[Params::Pitch1 + i] * 48 - 24);
                    voice.osc[i].settings.wtpos = voice.modulated[Params::WTPos1 + i];
                    voice.osc[i].settings.sync = voice.modulated[Params::Sync1 + i];
                    voice.osc[i].settings.pw = voice.modulated[Params::PulseW1 + i];
                    voice.osc[i].settings.bend = voice.modulated[Params::Bend1 + i];
                    voice.osc[i].settings.shaper = voice.modulated[Params::Shaper1 + i];
                    voice.osc[i].settings.shaperMix = voice.modulated[Params::ShaperMix1 + i] * (params[Params::ENBShaper1 + i] > 0.5);
                    voice.osc[i].settings.shaper2 = voice.modulated[Params::Shaper21 + i];
                    voice.osc[i].settings.shaper2Mix = voice.modulated[Params::Shaper2Mix1 + i] * (params[Params::ENBShaper1 + i] > 0.5);
                    voice.osc[i].settings.shaperMorph = voice.modulated[Params::ShaperMorph1 + i];

                    auto _ft = std::floor(params[Params::Filter1 + i] * 3); // Filter parameters
                    voice.filterp[i].sampleRate = samplerate;
                    voice.filterp[i].f0 = voice.modulated[Params::Freq1 + i] * voice.modulated[Params::Freq1 + i] * (22000 - 30) + 30;
                    voice.filterp[i].Q = voice.modulated[Params::Reso1 + i] * 16 + 1;
                    voice.filterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
                    voice.filterp[i].RecalculateParameters();

                    voice.noiselfp[i].sampleRate = samplerate;
                    voice.noiselfp[i].f0 = (std::min(voice.modulated[Params::Color1 + i] * 2, 1.)) * 21000 + 1000;
                    voice.noiselfp[i].Q = 0.6;
                    voice.noiselfp[i].type = FilterType::LowPass;
                    voice.noiselfp[i].RecalculateParameters();

                    voice.noisehfp[i].sampleRate = samplerate;
                    voice.noisehfp[i].f0 = (std::max(voice.modulated[Params::Color1 + i] * 2 - 1, 0.)) * 21000 + 30;
                    voice.noisehfp[i].Q = 0.6;
                    voice.noisehfp[i].type = FilterType::HighPass;
                    voice.noisehfp[i].RecalculateParameters();
                }
            }

            if (_osa == 1)
            {
                const double l = GenerateSample(0, data, samplerate);
                const double r = GenerateSample(1, data, samplerate);
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

            return { _suml / _osa, _sumr / _osa };
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

        inline double Combine(double a, double b, int index, auto& voice)
        {
            const auto mmin = voice.modulated[Params::MinMixX + index] ? voice.modulated[Params::MinMixX + index] * CombineSingle(a, b, MIN) : 0;
            const auto mult = voice.modulated[Params::MultMixX + index] ? voice.modulated[Params::MultMixX + index] * CombineSingle(a, b, MULT) : 0;
            const auto pong = voice.modulated[Params::PongMixX + index] ? voice.modulated[Params::PongMixX + index] * CombineSingle(a, b, PONG) : 0;
            const auto mmax = voice.modulated[Params::MaxMixX + index] ? voice.modulated[Params::MaxMixX + index] * CombineSingle(a, b, MAX) : 0;
            const auto mmod = voice.modulated[Params::ModMixX + index] ? voice.modulated[Params::ModMixX + index] * CombineSingle(a, b, MOD) : 0;
            const auto mand = voice.modulated[Params::AndMixX + index] ? voice.modulated[Params::AndMixX + index] * CombineSingle(a, b, AND) : 0;
            const auto inlv = voice.modulated[Params::InlvMixX + index] ? voice.modulated[Params::InlvMixX + index] * CombineSingle(a, b, INLV) : 0;
            const auto mmor = voice.modulated[Params::OrMixX + index] ? voice.modulated[Params::OrMixX + index] * CombineSingle(a, b, OR) : 0;
            const auto mxor = voice.modulated[Params::XOrMixX + index] ? voice.modulated[Params::XOrMixX + index] * CombineSingle(a, b, XOR) : 0;
            const auto madd = voice.modulated[Params::AddMixX + index] ? voice.modulated[Params::AddMixX + index] * CombineSingle(a, b, ADD) : 0;
            
            const auto multiplier = 1 + voice.modulated[Params::MinMixX + index] + voice.modulated[Params::MultMixX + index]
                + voice.modulated[Params::PongMixX + index] + voice.modulated[Params::MaxMixX + index] + voice.modulated[Params::ModMixX + index]
                + voice.modulated[Params::AndMixX + index] + voice.modulated[Params::InlvMixX + index] + voice.modulated[Params::OrMixX + index]
                + voice.modulated[Params::XOrMixX + index] + voice.modulated[Params::AddMixX + index];

            return voice.modulated[Params::MixX + index] * 2 * (1 / multiplier) * (mmin + mult + pong + mmax + mmod + mand + inlv + mmor + mxor + madd) + (1 - voice.modulated[Params::MixX + index]) * (a + b);
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

        BiquadParameters aafp;
        StereoEqualizer<2, BiquadFilter<>> aaf{ aafp };

        struct Voice
        {
            double samplesPress = 0;
            double modulated[Params::ModCount];

            double envelope = 0;
            double frequency = 40;
            double key = 0;
            double pressed = 0;
            double pressedOld = 0;
            double deltaf = 0;
            double velocity = 1;

            Oscillator sub{ {.wavetable = Wavetables::sub } };

            double oscs[Oscillators];
            Oscillator osc[Oscillators];
            Oscillator lfo[LFOs];

            BiquadParameters filterp[Oscillators];
            StereoEqualizer<2, BiquadFilter<>> filter[Oscillators]{ filterp[0], filterp[1], filterp[2], filterp[3] };

            BiquadParameters noiselfp[Oscillators];
            StereoEqualizer<2, BiquadFilter<>> noiself[Oscillators]{ noiselfp[0], noiselfp[1], noiselfp[2], noiselfp[3] };
            BiquadParameters noisehfp[Oscillators];
            StereoEqualizer<2, BiquadFilter<>> noisehf[Oscillators]{ noisehfp[0], noisehfp[1], noisehfp[2], noisehfp[3] };

            BiquadParameters cfilterp[Combines];
            StereoEqualizer<2, BiquadFilter<>> cfilter[Combines]{ cfilterp[0], cfilterp[1], cfilterp[2] };

            BiquadParameters dcoffp[Combines];
            StereoEqualizer<2, BiquadFilter<>> dcoff[Combines]{ dcoffp[0], dcoffp[1], dcoffp[2] };

            ADSR env[Envelopes];
        };

        Voice voices[6];
    };
}