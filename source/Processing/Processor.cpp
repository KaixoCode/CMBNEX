#include "Processing/Processor.hpp"

using namespace Steinberg;

namespace Kaixo
{
    Processor::~Processor()
    {
        doModulationSync = false;
        modulationThread.join();
    }

    void Processor::TriggerVoice(int voice, int pitch, double velocity)
    {
        voices[voice].rand = voices[voice].myrandom() * 0.5 + 0.5;
        voices[voice].velocity = velocity;
        voices[voice].frequency = voices[lastPressedVoice].frequency;
        voices[voice].pressedOld = voices[lastPressedVoice].frequency;
        voices[voice].pressed = pitch;
        lastPressedVoice = voice;

        if (params.goals[Params::Retrigger] > 0.5 || !voices[voice].env[0].Gate())
        {
            for (int i = 0; i < Oscillators; i++)
                voices[voice].osc[i].phase = params.goals[Params::RandomPhase1 + i] > 0.5 ? (std::rand() % 32767) / 32767. : 0;
            for (int i = 0; i < LFOs; i++)
                voices[voice].lfo[i].phase = 0;
            voices[voice].sub.phase = 0;
            voices[voice].samplesPress = projectTimeSamples;
        }

        for (int i = 0; i < Envelopes; i++)
        {
            voices[voice].env[i].settings.legato = params.goals[Params::Retrigger] < 0.5;            
            voices[voice].env[i].Gate(true);
        }
    }

    void Processor::NotePress(Vst::Event& event)
    {
        if (params.goals[Params::Voices] < 0.5)
        {
            m_MonoNotePresses.push_back(event.noteOn.pitch);
            TriggerVoice(0, event.noteOn.pitch, event.noteOn.velocity);
            return;
        }

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
                TriggerVoice(voice, event.noteOn.pitch, event.noteOn.velocity);
            }
        }
    }

    void Processor::ReleaseVoice(int voice, int pitch, double velocity)
    {
        for (int i = 0; i < Envelopes; i++)
            voices[voice].env[i].Gate(false);
    }

    void Processor::NoteRelease(Vst::Event& event)
    {
        // If monophonic
        if (params.goals[Params::Voices] < 0.5)
        {
            // If we released a note that isn't currently producing sound (because monophonic)
            // we will remove it from the notepresses
            if (voices[0].pressed != event.noteOff.pitch)
            {
                m_MonoNotePresses.remove(event.noteOff.pitch);
            }
            else
            {
                // Otherwise we'll remove the released note from the stack
                if (!m_MonoNotePresses.empty())
                    m_MonoNotePresses.pop_back();

                // And if notes remain on the stack, we'll trigger that one again.
                if (!m_MonoNotePresses.empty())
                    TriggerVoice(0, m_MonoNotePresses.back(), voices[0].velocity);

                // Otherwise release the voice
                else
                    ReleaseVoice(0, event.noteOff.pitch, event.noteOff.velocity);
            }
        }
        else
        {
            // If not monophonic, empty note stack
            m_MonoNotePresses.clear();
        }

        // Even if monophonic, we'll check the other notes in case
        // it was switched to monophonic while notes were being held,
        // this will then release those voices once the note is released.

        // Find the note in the pressed notes per voice
        while (true)
        {
            auto it = std::find(m_Notes.begin(), m_Notes.end(), event.noteOff.pitch);
            if (it != m_Notes.end())
            {
                // If it was pressed, get the voice index
                int voice = std::distance(m_Notes.begin(), it);

                // Set note to -1 and emplace to available.
                ReleaseVoice(voice, event.noteOff.pitch, event.noteOff.velocity);
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

    void Processor::Event(Vst::Event& event)
    {
        switch (event.type)
        {
        case Event::kNoteOnEvent:
            if (params.goals[Params::PitchBend] > 1 || params.goals[Params::PitchBend] < 0) params.goals[Params::PitchBend] = 0.5;

            NotePress(event);
            break;
        case Event::kNoteOffEvent:

            NoteRelease(event);
            break;
        }
    }

    void Processor::ModSync()
    {   // Gets ran in thread, sends messages for modulation to controller.
        while (doModulationSync)
        {   // Loop over mod parameters
            for (int m = 0; m < Params::ModCount; m++)
            {   // Allocate a message
                if (auto message = Steinberg::owned(allocateMessage()))
                {   // Set id and correct content, then send
                    message->setMessageID(UPDATE_MODULATION);
                    message->getAttributes()->setInt(UPDATE_MODULATION_PARAM, m);
                    message->getAttributes()->setFloat(UPDATE_MODULATION_VALUE, voices[lastPressedVoice].modulated[m]);
                    sendMessage(message);
                }
            }

            // Approximately 60 times a second.
            std::this_thread::sleep_for(std::chrono::milliseconds{ 30 });
        }
    }

    double Processor::GenerateSample(size_t channel, ProcessData& data, Voice& voice, double ratio)
    {   // If the gain envelope is done, no more sound, so return 0.
        if (voice.env[0].Done()) return 0;

        // Calculate bend offset and time mult.
        const double _bendOffset = params[{ Params::PitchBend, ratio }] * 12 * voice.modulated[Params::Bend] - 6 * voice.modulated[Params::Bend] + voice.modulated[Params::Transpose] * 96 - 48;
        const double _timeMult = voice.modulated[Params::Time] < 0.5 ? (voice.modulated[Params::Time] + 0.5) : ((voice.modulated[Params::Time] - 0.5) * 2 + 1);

        if (channel == 0) // Only do all generating on channel 0, to prevent double calculating
        {
            for (int i = 0; i < Oscillators; i++)
            {   // Oscillator generate
                if (params.goals[Params::Enable1 + i] > 0.5) // Generate oscillator sound
                    voice.osc[i].sample = voice.osc[i].Offset(myfmod1(voice.modulated[Params::Phase1 + i] + 5), params.goals[Params::ENBShaper1 + i] > 0.5, params.goals[Params::ShaperFreez1 + i] > 0.5);
                else
                {
                    voice.osc[i].sample = 0;
                    voice.oscs[i] = 0;
                    continue;
                }

                voice.oscs[i] = voice.osc[i].sample + voice.modulated[Params::DCOff1 + i] * 2 - 1;

                // Fold
                if (params.goals[Params::ENBFold1 + i] > 0.5)
                    voice.oscs[i] = Shapers::fold(voice.oscs[i] * (voice.modulated[Params::Fold1 + i] * 15 + 1), voice.modulated[Params::Bias1 + i] * 2 - 1);

                // Noise
                if (params.goals[Params::ENBNoise1 + i] > 0.5)
                    voice.oscs[i] += voice.modulated[Params::Noise1 + i] * voice.noiself[i].Apply(voice.noisehf[i].Apply(voice.myrandom(), channel), channel); // Add noise
                    
                // Drive
                if (params.goals[Params::ENBDrive1 + i] > 0.5)
                    voice.oscs[i] = Shapers::drive(voice.oscs[i], voice.modulated[Params::DriveGain1 + i] * 3 + 1, voice.modulated[Params::DriveAmt1 + i]);
                else
                    voice.oscs[i] = constrain(voice.oscs[i], -1., 1.);

                // Gain
                voice.oscs[i] *= voice.modulated[Params::Volume1 + i]; // Adjust for volume
                
                // Filter
                if (params.goals[Params::ENBFilter1 + i] > 0.5)
                    voice.oscs[i] = voice.filter[i].Apply(voice.oscs[i], channel); // Apply pre-combine filter

                // Apply AAF if oversampling
                //if (params.goals[Params::Oversample] > 0)
                //    voice.oscs[i] = voice.aaf[2 + i].Apply(voice.oscs[i], voice.aafp[2 + i]);
            }

            { // Sub oscillator
                int _octave = std::round(params.goals[Params::SubOct] * 4 - 2);
                voice.sub.SAMPLE_RATE = voice.samplerate;
                voice.sub.settings.frequency = noteToFreq(voice.frequency + _octave * 12 + _bendOffset);
                voice.sub.settings.wtpos = voice.modulated[Params::SubOvertone];
                voice.sub.sample = voice.sub.OffsetClean(0);
            }
        }

        // Oscillator panning and sending to destination
        double _cs[Combines * 2 + 1]{ 0, 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < Oscillators; i++)
        {
            double _vs = voice.oscs[i];

            // Pan
            if (voice.modulated[Params::Pan1 + i] != 0.5)
                _vs *= std::min((channel == 1 ? 2 * voice.modulated[Params::Pan1 + i] : 2 - 2 * voice.modulated[Params::Pan1 + i]), 1.); // Panning

            size_t _dests = params.goals[Params::DestA + i] * 128.;
            for (int j = 0; j < Combines * 2 + 1; j++)
                if ((_dests >> j) & 1U) _cs[j] += _vs;
        }

        // Result, start with sub, rest will be added later.
        double _res = voice.modulated[Params::SubGain] * voice.sub.sample; // Start with sub

        // All the combiners
        for (int i = 0; i < Combines; i++)
        {
            constexpr static size_t _offsets[]{ 5, 3, 0 };
            constexpr static size_t _mult[]{ 32, 8, 0 };

            // Do combine, apply pre-gain
            double _v = Combine(
                _cs[i * 2] * voice.modulated[Params::PreGainX + i] * 2,
                _cs[i * 2 + 1] * voice.modulated[Params::PreGainX + i] * 2,
                i, voice);

            // Fold
            if (params.goals[Params::ENBFoldX + i] > 0.5)
                _v = Shapers::fold(_v * (voice.modulated[Params::FoldX + i] * 15 + 1), voice.modulated[Params::BiasX + i] * 2 - 1);

            // Drive
            if (params.goals[Params::ENBDriveX + i] > 0.5)
                _v = Shapers::drive(_v, voice.modulated[Params::DriveGainX + i] * 3 + 1, voice.modulated[Params::DriveAmtX + i]);
            else // Always clip, even if drive disabled, to prevent unstable filter
                _v = constrain(_v, -1., 1.);

            // Volume
            _v = _v * voice.modulated[Params::GainX + i] * 2;

            // Filter
            if (params.goals[Params::ENBFilterX + i] > 0.5)
                _v = voice.cfilter[i].Apply(_v, channel);

            // DCoffset
            _v = params.goals[Params::DCX + i] > 0.5 ? voice.dcoff[i].Apply(_v, channel) : _v;

            // Apply AAF if oversampling
            //if (params.goals[Params::Oversample] > 0)
            //    _v = voice.aaf[6 + i + channel * Combines].Apply(_v, voice.aafp[6 + i + channel * Combines]);

            if (i == Combines - 1) // if combiner 'Z', add to output and break.
            {
                _res += _v;
                break;
            }

            // Send combiner output to destinations
            const size_t _dests = params.goals[Params::DestX + i] * _mult[i];
            for (int j = 0; j < _offsets[i]; j++)
                if ((_dests >> j) & 1U) _cs[j + (i + 1) * 2] += _v;
        }

        // Add the signals that were sent directly to output.
        _res += _cs[Combines * 2];

        // Clip sample, apply gain envelope and return.
        return voice.env[0].Apply(_res, channel) * voice.modulated[Params::GlobalGain] * 2;
    }

    void Processor::Generate(ProcessData& data, size_t s)
    {
        processData = &data; // Store the process data, so it's accessible everywhere.
        projectTimeSamples = data.processContext->projectTimeSamples; // Store project time samples

        // Prepare buffers
        for (auto& i : voices)
            i.buffer.prepare(s);

        // If monophonic, only generate 1st voice.
        if (params.goals[Params::Voices] < 0.5)
        {
            GenerateVoice(voices[0]);
        }

        // If no threading, do everything on main thread
        else if (params.goals[Params::Threading] < 0.5)
        {
            for (int i = 0; i < Voices; i++)
                GenerateVoice(voices[i]);
        }
        else
        {
            // Divide voices among worker threads and main thread
            std::future<void> futures[Voices];
            bool onMain[Voices]{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
            int forDone = 0;
            for (int i = 0; i < Voices; i++)
            {
                if (swapBuffer.amount < 52) onMain[i] = true; // If buffer < 52, do work on main thread always
                else if (!voices[i].env[0].Done() && forDone < 1) onMain[i] = true, forDone++; // Always do 1 voice on main thread
                else if (voices[i].env[0].Done()) onMain[i] = true; // If no generating, do on main thread
                else futures[i] = voiceThreadPool.push([this, i]() { GenerateVoice(voices[i]); }); // Otherwise send work to threadpool
            }

            // Generate everything that was selected for main thread right here.
            for (int i = 0; i < Voices; i++)
                if (onMain[i])
                    GenerateVoice(voices[i]);

            // Wait on threadpool.
            for (int i = 0; i < Voices; i++)
                if (!onMain[i])
                    futures[i].wait();
        }

        // Finally, sum all voices
        for (auto& i : voices)
            for (int j = 0; j < s; j++)
                swapBuffer.left[j] += i.buffer.left[j],
                swapBuffer.right[j] += i.buffer.right[j];
    }


    inline double Processor::Combine(double a, double b, int index, Voice& voice)
    {   
        const uint64_t _a = std::bit_cast<uint64_t>(a);
        const uint64_t _b = std::bit_cast<uint64_t>(b);
        const uint64_t _as = a * 4294967296;
        const uint64_t _bs = b * 4294967296;

        // Calculate all the combines
        const double sum =
            voice.modulated[Params::MinMixX + index] * 1.4 * (std::min(a, b))
          + voice.modulated[Params::MaxMixX + index] * 1.3 * (std::max(a, b))
          + voice.modulated[Params::AndMixX + index] * 1.5 * (std::bit_cast<double>(_a & _b))
          + voice.modulated[Params::XOrMixX + index] * 1.7 * ((((uint64_t)(a * 4294967296) ^ (uint64_t)(b * 4294967296)) % 4294967296) / 4294967296. - 0.5)
          + voice.modulated[Params::OrMixX + index] * 0.3 * std::bit_cast<double>(_a | _b)
          + voice.modulated[Params::MultMixX + index] * a * b * 1.8
          + voice.modulated[Params::PongMixX + index] * (1.2 * (a > 0 ? a : b < 0 ? b : 0))
          + voice.modulated[Params::ModMixX + index] * 1.4 * (std::bit_cast<double>(_a % (_b == 0 ? 1 : _b)))
          + voice.modulated[Params::InlvMixX + index] * 0.7 * (std::bit_cast<double>((_a & 0x5555555555555555) | (_b & 0xAAAAAAAAAAAAAAAA)))
          + voice.modulated[Params::AddMixX + index] * (a + b);

        // Return the combined signals, taking into account the mix value for this combiner.
        return voice.modulated[Params::MixX + index] * sum + (1 - voice.modulated[Params::MixX + index]) * (a + b);
    }

    double Processor::CombineSingle(double a, double b, int mode)
    {
        // Calculate a couple helper values
        const uint64_t _a = std::bit_cast<uint64_t>(a);
        const uint64_t _b = std::bit_cast<uint64_t>(b);
        const uint64_t _as = a * 4294967296;
        const uint64_t _bs = b * 4294967296;
        switch (mode) { // Switch to the combine mode and calculate result
        case AND:  return 1.5 * (std::bit_cast<double>(_a & _b));
        case OR: { double _ab = 0.3 * (std::bit_cast<double>(_a | _b)); if (std::isnan(_ab)) _ab = 0; return _ab; } // Special case for 'OR' because it can potentially generate NANs
        case XOR:  return 1.7 * (((_as ^ _bs) % 4294967296) / 4294967296. - 0.5);
        case PONG: return 1.2 * (a > 0 ? a : b < 0 ? b : 0); break;
        case INLV: return 0.7 * (std::bit_cast<double>((_a & 0x5555555555555555) | (_b & 0xAAAAAAAAAAAAAAAA)));
        case MIN:  return 1.4 * (std::min(a, b));
        case MAX:  return 1.3 * (std::max(a, b));
        case MOD:  return 1.4 * (std::bit_cast<double>(_a % (_b == 0 ? 1 : _b)));
        case MULT: return 1.8 * (a * b);
        case ADD:  return (a + b);
        }
    }

    void Processor::CalculateModulation(Voice& voice, double ratio)
    {   // If ratio == 1, move goals, because it's more efficient.
        if (ratio == 1) std::memcpy(voice.modulated, params.goals, Params::ModCount * sizeof(double));

        for (int m = 0; m < Params::ModCount; m++)
        {
            // only copy value if ration != 1, because we already did that.
            if (ratio != 1) voice.modulated[m] = params[{ m, ratio }];

            // If this param doesn't have modulation, continue. (Extra state, but makes it more efficient)
            if (!hasmod[m]) continue;

            // Go through all mods of this parameter
            bool edited = false; // Keep track if it was edited
            for (int i = 0; i < ModAmt; i++)
            {   // Calculate index
                int index = m * ModAmt + i;
                if (!modgoals[index]) continue; // If no goal, continue

                // Get the source
                int source = (modgoals[index] * (int)ModSources::Amount + 0.1);
                edited = true; // Since we had a goal, we're gonna edit.

                double amount = modamount[index] * 2 - 1; // Calculate mod amount
                if (source == (int)ModSources::Ran)
                {   // Random * amount
                    voice.modulated[m] += (voice.rand * amount);
                }
                else if (source == (int)ModSources::Mod)
                {   // Modwheel * amount
                    voice.modulated[m] += (params[{ Params::ModWheel, ratio }] * amount);
                }
                else if (source == (int)ModSources::Vel)
                {   // Velocity * amount
                    voice.modulated[m] += (voice.velocity * amount);
                }
                else if (source == (int)ModSources::Key)
                {   // Key also takes into account bend and transpose
                    voice.modulated[m] += (voice.key * amount);
                }
                else if (source >= (int)ModSources::Mac1)
                {   // Macro, first calculate macro index, then multiply amount
                    int mindex = (source - (int)ModSources::Mac1);
                    voice.modulated[m] += (voice.modulated[Params::Macro1 + mindex] * amount);
                }
                else if (source >= (int)ModSources::Env1)
                {   // Envelopes, calculate index, then multiply amount
                    int mindex = (source - (int)ModSources::Env1);
                    voice.modulated[m] += (voice.env[mindex].sample * amount);
                }
                else if (source >= (int)ModSources::LFO1)
                {   // LFO, calculate index, multiply amount
                    int mindex = (source - (int)ModSources::LFO1);
                    voice.modulated[m] += (voice.lfo[mindex].sample * amount);
                }
            }

            // If we need to constrain, and have edited, constrain the value.
            if (edited && ParamNames[m].constrain)
                voice.modulated[m] = constrain(voice.modulated[m], 0., 1.);
        }
    }

    void Processor::UpdateComponentParameters(Voice& voice, double ratio)
    {   // Get BPM from process context if valid, otherwise just use 128
        double bpm = (processData->processContext->state & ProcessContext::kTempoValid) ? processData->processContext->tempo : 128;
        
        // Calculate bend offset and time mult.
        const double _bendOffset = params[{ Params::PitchBend, ratio }] * 12 * voice.modulated[Params::Bend] - 6 * voice.modulated[Params::Bend] + voice.modulated[Params::Transpose] * 96 - 48;
        const double _timeMult = voice.modulated[Params::Time] < 0.5 ? (voice.modulated[Params::Time] + 0.5) : ((voice.modulated[Params::Time] - 0.5) * 2 + 1);

        // Frequency glide
        voice.deltaf = _timeMult * std::abs((voice.pressed - voice.pressedOld) / (0.001 + voice.modulated[Params::Glide] * 
            voice.modulated[Params::Glide] * voice.modulated[Params::Glide] * 10 * processData->processContext->sampleRate));
        if (voice.frequency > voice.pressed)
            if (voice.frequency - voice.deltaf < voice.pressed) voice.frequency = voice.pressed;
            else voice.frequency -= voice.deltaf;

        if (voice.frequency < voice.pressed)
            if (voice.frequency + voice.deltaf > voice.pressed) voice.frequency = voice.pressed;
            else voice.frequency += voice.deltaf;

        voice.key = (voice.frequency + _bendOffset) / 127.;

        for (int i = 0; i < Combines; i++)
        {   // Combines parameters
            voice.dcoffp[i].f0 = 35;
            voice.dcoffp[i].type = FilterType::HighPass;
            voice.dcoffp[i].Q = 1;
            voice.dcoffp[i].sampleRate = voice.samplerate;
            voice.dcoffp[i].RecalculateParameters(ratio == 1);

            auto _ft = std::floor(params.goals[Params::FilterX + i] * 3); // Filter parameters
            voice.cfilterp[i].sampleRate = voice.samplerate;
            voice.cfilterp[i].f0 = voice.modulated[Params::FreqX + i] * voice.modulated[Params::FreqX + i] * (22000 - 30) + 30;
            voice.cfilterp[i].Q = voice.modulated[Params::ResoX + i] * 16 + 1;
            voice.cfilterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
            voice.cfilterp[i].RecalculateParameters(ratio == 1);
        }

        for (int i = 0; i < Envelopes; i++)
        {   // Adjust Envelope parameters and generate
            voice.env[i].SAMPLE_RATE = processData->processContext->sampleRate;
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
            voice.lfo[i].SAMPLE_RATE = processData->processContext->sampleRate;

            if (params.goals[Params::LFOSync1 + i] > 0.5) // If bpm synced lfo
            {
                size_t _type = std::floor(voice.modulated[Params::LFORate1 + i] * (TimesAmount - 1));

                double _ps = (60. / bpm) * TimesValue[_type] * 4; // Seconds per oscillation
                double _f = 1 / _ps; // Frequency of the oscillations
                double _spp = _ps * processData->processContext->sampleRate; // Samples per oscillation
                double _phase = std::fmod((params.goals[Params::LFORetr1 + i] > 0.5 ? projectTimeSamples - voice.samplesPress : projectTimeSamples) + swapBuffer.amount * ratio, _spp) / _spp;

                // If playing, sync phase to samples played
                if (processData->processContext->state & ProcessContext::kPlaying)
                    voice.lfo[i].phase = _phase;
                voice.lfo[i].settings.frequency = _f;
            }
            else
            {
                double _f = _timeMult * voice.modulated[Params::LFORate1 + i] * voice.modulated[Params::LFORate1 + i] * 29.9 + 0.1; // Frequency of the oscillations
                double _ps = 1 / _f; // Seconds per oscillation
                double _spp = _ps * processData->processContext->sampleRate; // Samples per oscillation
                double _phase = std::fmod((params.goals[Params::LFORetr1 + i] > 0.5 ? projectTimeSamples - voice.samplesPress : projectTimeSamples) + swapBuffer.amount * ratio, _spp) / _spp;

                // If playing, sync phase to samples played
                if (processData->processContext->state & ProcessContext::kPlaying)
                    voice.lfo[i].phase = _phase;
                voice.lfo[i].settings.frequency = _f;
            }

            voice.lfo[i].settings.wtpos = voice.modulated[Params::LFOPos1 + i];
            voice.lfo[i].settings.shaper3 = voice.modulated[Params::LFOShaper1 + i];
            voice.lfo[i].sample = voice.lfo[i].OffsetSimple(voice.modulated[Params::LFOPhase1 + i]) * (voice.modulated[Params::LFOLvl1 + i] * 2 - 1);
        }

        for (int i = 0; i < Oscillators; i++)
        {   // Oscillator parameters
            auto _ft = std::floor(params.goals[Params::Filter1 + i] * 3); // Filter parameters
            voice.filterp[i].sampleRate = voice.samplerate;
            voice.filterp[i].f0 = voice.modulated[Params::Freq1 + i] * voice.modulated[Params::Freq1 + i] * (22000 - 30) + 30;
            voice.filterp[i].Q = voice.modulated[Params::Reso1 + i] * 16 + 1;
            voice.filterp[i].type = _ft == 0 ? FilterType::LowPass : _ft == 1 ? FilterType::HighPass : FilterType::BandPass;
            voice.filterp[i].RecalculateParameters(ratio == 1);

            voice.noiselfp[i].sampleRate = voice.samplerate;
            voice.noiselfp[i].f0 = (std::min(voice.modulated[Params::Color1 + i] * 2, 1.)) * 21000 + 1000;
            voice.noiselfp[i].Q = 0.6;
            voice.noiselfp[i].type = FilterType::LowPass;
            voice.noiselfp[i].RecalculateParameters(ratio == 1);

            voice.noisehfp[i].sampleRate = voice.samplerate;
            voice.noisehfp[i].f0 = (std::max(voice.modulated[Params::Color1 + i] * 2 - 1, 0.)) * 21000 + 100;
            voice.noisehfp[i].Q = 0.6;
            voice.noisehfp[i].type = FilterType::HighPass;
            voice.noisehfp[i].RecalculateParameters(ratio == 1);

            voice.osc[i].SAMPLE_RATE = voice.samplerate;
            voice.osc[i].settings.frequency = noteToFreq(voice.frequency + _bendOffset
                + voice.modulated[Params::Detune1 + i] * 4 - 2 + voice.modulated[Params::Pitch1 + i] * 48 - 24);
            voice.osc[i].settings.wtpos = voice.modulated[Params::WTPos1 + i];
            voice.osc[i].settings.sync = voice.modulated[Params::Sync1 + i];
            voice.osc[i].settings.pw = voice.modulated[Params::PulseW1 + i];
            voice.osc[i].settings.bend = voice.modulated[Params::Bend1 + i];
            voice.osc[i].settings.shaper = voice.modulated[Params::Shaper1 + i];
            voice.osc[i].settings.shaperMix = voice.modulated[Params::ShaperMix1 + i] * (params.goals[Params::ENBShaper1 + i] > 0.5);
            voice.osc[i].settings.shaper2 = voice.modulated[Params::Shaper21 + i];
            voice.osc[i].settings.shaper2Mix = voice.modulated[Params::Shaper2Mix1 + i] * (params.goals[Params::ENBShaper1 + i] > 0.5);
            voice.osc[i].settings.shaperMorph = voice.modulated[Params::ShaperMorph1 + i];
        }
    }

    inline void Processor::GenerateVoice(Voice& voice)
    {   
        // Calculate oversample amount
        size_t _index = std::floor(params.goals[Params::Oversample] * 4);
        size_t _osa = _index == 0 ? 1 : _index == 1 ? 2 : _index == 2 ? 4 : 8;
        if (processData->processMode & ProcessModes::kOffline) _osa = 16;

        // Set oversampled sample rate in voice
        voice.samplerate = processData->processContext->sampleRate * _osa;

        // If envelope is done, no sound so return, but do calculate modulations
        if (voice.env[0].Done())
        {
            CalculateModulation(voice, 1);
            UpdateComponentParameters(voice, 1);
            return;
        }

        // Generate the necessary sample amount
        for (int index = 0; index < swapBuffer.amount; index++)
        {
            double ratio = index / (double)swapBuffer.amount;

            CalculateModulation(voice, ratio);
            UpdateComponentParameters(voice, ratio);

            if (_osa == 1) // No oversampling
            {
                const double l = GenerateSample(0, *processData, voice, ratio);
                const double r = GenerateSample(1, *processData, voice, ratio);
                voice.buffer.left[index] += l;
                voice.buffer.right[index] += r;
            }
            else // Oversampling
            {
                for (auto& i : voice.aafp)
                {
                    i.sampleRate = voice.samplerate;
                    i.f0 = std::min(22000., voice.samplerate * 0.4);
                    i.RecalculateParameters();
                }

                double _suml = 0;
                double _sumr = 0;
                for (int k = 0; k < _osa; k++)
                {
                    double _l = GenerateSample(0, *processData, voice, ratio);
                    double _r = GenerateSample(1, *processData, voice, ratio);

                    _l = voice.aaf[0].Apply(_l, voice.aafp[0]);
                    _r = voice.aaf[1].Apply(_r, voice.aafp[1]);

                    _suml += _l;
                    _sumr += _r;
                }
                voice.buffer.left[index] += _suml / _osa;
                voice.buffer.right[index] += _sumr / _osa;
            }
        }
    }
}