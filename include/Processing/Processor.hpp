#pragma once
#include "pch.hpp"
#include "CIDs.hpp"
#include "Modules.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/Utils.hpp"
#include "Processing/BaseProcessor.hpp"
#ifndef CMBNEX_TESTS
#include "Views/MainView.hpp"
#endif

namespace Kaixo
{
    // Main processor for synth
    class Processor : public Instrument
    {
    public:
        static FUnknown* createInstance(void*) { return static_cast<IAudioProcessor*>(new Processor); }

        // Amounts of each
        constexpr static auto Envelopes = 5;
        constexpr static auto LFOs = 5;
        constexpr static auto Oscillators = 4;
        constexpr static auto Combines = 3;
        constexpr static auto Voices = 6;

        // Combiner modes
        enum CombineMode { ADD, MIN, MULT, PONG, MAX, MOD, AND, INLV, OR, XOR, Size };

        // Single voice contains all components.
        struct Voice
        {
            double samplerate = 44100;

            double samplesPress = 0; // Samples passed since start of project when voice activated
            double modulated[Params::ModCount]; // Modulated parameter values.

            double pressed = 0; // Pressed note (0-127)
            double pressedOld = 0; // Previous pressed note (0-127)
            double deltaf = 0; // Calculated delta to glide from pressedOld to pressed
            double frequency = 40; // Current note, taking into account glide (0-127)
            double key = 0; // Pressed key (0-127)
            double velocity = 1; // Pressed velocity
            double rand = 0; // Random value, generated each note press.

            double oscs[Oscillators]; // Oscillator values, stored separately for modulation
            Oscillator osc[Oscillators]; // Main oscillators
            Oscillator sub; // Sub oscillator
            Oscillator lfo[LFOs]; // LFOs
            ADSR env[Envelopes]; // Envelopes

            // Main oscillator filters
            BiquadParameters filterp[Oscillators];
            StereoEqualizer<2, BiquadFilter> filter[Oscillators]{ filterp[0], filterp[1], filterp[2], filterp[3] };

            // Main oscillator noise color filters
            BiquadParameters noiselfp[Oscillators];
            StereoEqualizer<2, BiquadFilter> noiself[Oscillators]{ noiselfp[0], noiselfp[1], noiselfp[2], noiselfp[3] };
            BiquadParameters noisehfp[Oscillators];
            StereoEqualizer<2, BiquadFilter> noisehf[Oscillators]{ noisehfp[0], noisehfp[1], noisehfp[2], noisehfp[3] };

            // Combiner filters
            BiquadParameters cfilterp[Combines];
            StereoEqualizer<2, BiquadFilter> cfilter[Combines]{ cfilterp[0], cfilterp[1], cfilterp[2] };

            // DC offset filter
            BiquadParameters dcoffp[Combines];
            StereoEqualizer<2, BiquadFilter> dcoff[Combines]{ dcoffp[0], dcoffp[1], dcoffp[2] };

            // Anti-aliasing filter
            BiquadParameters aafp;
            StereoEqualizer<2, BiquadFilter> aaf{ aafp };
        };

        Voice voices[Voices]; // All the voices

        double projectTimeSamples = 0; // Amount of samples since start project.
        int lastPressedVoice = 0; // Last voice index that was activated

        bool doModulationSync = true; // Gets set to false at end to stop modulation sync thread.

        std::list<int> m_MonoNotePresses; // Stack to keep track of key pressed in mono mode.
        std::array<int, Voices> m_Notes{ -1, -1, -1, -1, -1, -1 }; // Notes pressed for each voice
        std::vector<int> m_Pressed; // Current voices busy
        std::vector<int> m_Available{ 0, 1, 2, 3, 4, 5 }; // Available voices

        ProcessData* processData; // Process data given by DAW.

        cxxpool::thread_pool voiceThreadPool{ Voices }; // Threadpool, 1 thread for each voice.

        std::thread modulationThread{ &Processor::ModSync, this }; // Modulation sync thread.

        ~Processor() override;

        // Midi event handlers
        void Event(Vst::Event& event) override;
        void TriggerVoice(int voice, int pitch, double velocity);
        void ReleaseVoice(int voice, int pitch, double velocity);
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