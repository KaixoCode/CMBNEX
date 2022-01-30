#pragma once
#include "pch.hpp"
#include "CIDs.hpp"
#include "Modules.hpp"
#include "Components/Parameter.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/Utils.hpp"
#include "Views/MainView.hpp"
#include "Processing/BaseProcessor.hpp"

/**
 * TODO:
 * - Option to toggle use of threads
 * - Set amount of voices.
 * 
 */

namespace Kaixo
{
    class Processor : public Instrument
    {
    public:
        static FUnknown* createInstance(void*) { return static_cast<IAudioProcessor*>(new Processor); }

        constexpr static auto Envelopes = 5;
        constexpr static auto LFOs = 5;
        constexpr static auto Oscillators = 4;
        constexpr static auto Combines = 3;
        constexpr static auto Voices = 6;

        enum CombineMode { ADD, MIN, MULT, PONG, MAX, MOD, AND, INLV, OR, XOR, Size };

        struct Voice
        {
            double samplerate = 44100;

            double samplesPress = 0;
            double modulated[Params::ModCount];

            double envelope = 0;
            double frequency = 40;
            double key = 0;
            double pressed = 0;
            double pressedOld = 0;
            double deltaf = 0;
            double velocity = 1;
            double rand = 0;

            Oscillator sub;

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

            BiquadParameters aafp;
            StereoEqualizer<2, BiquadFilter<>> aaf{ aafp };

            ADSR env[Envelopes];
        };

        Voice voices[Voices];

        double projectTimeSamples = 0;
        int lastPressedVoice = 0;
        bool doModulationSync = true;

        std::list<int> m_MonoNotePresses;
        std::array<int, Voices> m_Notes{ -1, -1, -1, -1, -1, -1 };
        std::vector<int> m_Pressed;
        std::vector<int> m_Available{ 0, 1, 2, 3, 4, 5 };

        ProcessData* processData;

        cxxpool::thread_pool voiceThreadPool{ Voices };

        std::thread modulationThread{ &Processor::ModSync, this };

        ~Processor() override;

        // Midi event handlers
        void Event(Vst::Event& event) override;
        void TriggerVoice(int voice, int pitch, int velocity);
        void ReleaseVoice(int voice, int pitch, int velocity);
        void NotePress(Vst::Event& event);
        void NoteRelease(Vst::Event& event);

        // Sync the modulation back to the controller in worker thread.
        void ModSync();

        // Main generate, gets called by base class.
        void Generate(ProcessData& data, size_t s) override;

        // Generate buffer for a single voice
        void GenerateVoice(Voice& voice);
        double GenerateSample(size_t channel, ProcessData& data, Voice& voice, double ratio);
        void CalculateModulation(Voice& voice, double ratio);
        void UpdateComponentParameters(Voice& voice, double ratio);

        double Clip(double a, int channel);
        double Combine(double a, double b, int index, Voice& voice);
        double CombineSingle(double a, double b, int mode);
    };
}