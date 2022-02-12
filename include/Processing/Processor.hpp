#pragma once
#include "pch.hpp"
#include "CIDs.hpp"
#include "Modules.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/Utils.hpp"
#include "Processing/BaseProcessor.hpp"
#include "Utils/SIMD.hpp"
#ifndef CMBNEX_TESTS
#include "Views/MainView.hpp"
#endif

namespace Kaixo
{
    // Main processor for synth
    class Processor : public Instrument
    {
    public:
        static inline auto SIMDPATH = ChoosePath();

        // Fast random number generator, uses xorshift algorithm
        struct NoiseGenerator
        {
            NoiseGenerator()
            {
            }
            constexpr static inline auto max = (double)std::numeric_limits<uint32_t>::max();
            uint32_t x = 0xF123456789ABCDEF;
            inline double operator()() {
                x ^= x << 13;
                x ^= x >> 17;
                x ^= x << 5;
                return 2 * (x / max) - 1.;
            }
        };

        static FUnknown* createInstance(void*) { return static_cast<IAudioProcessor*>(new Processor); }

        // Amounts of each
        constexpr static auto Envelopes = 5;
        constexpr static auto LFOs = 5;
        constexpr static auto Oscillators = 4;
        constexpr static auto Unison = 8;
        constexpr static auto Combines = 3;
        constexpr static auto Voices = 16;

        // Combiner modes
        enum CombineMode { ADD, MIN, MULT, PONG, MAX, MOD, AND, INLV, OR, XOR, Size };

        // Single voice contains all components.
        struct Voice
        {
            NoiseGenerator myrandom;

            Voice() {
                std::fill_n(modulated, Params::ModCount, 0.);
            }

            Buffer buffer;
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

            double bendOffset = 0;
            int sinceLastParamUpdate = 0;

            double oscs[2][Oscillators][8]; // Oscillator values, stored separately for modulation
            double outs[2][8];
            int unison[Oscillators];
            int unisonMode[Oscillators];
            Oscillator osc[Oscillators * Unison]; // Main oscillators
            Oscillator sub; // Sub oscillator
            Oscillator lfo[LFOs]; // LFOs
            ADSR env[Envelopes]; // Envelopes

            // Main oscillator filters
            BiquadParameters filterp[Oscillators];
            StereoEqualizer<2, BiquadFilter> filter[Oscillators]{ filterp[0], filterp[1], filterp[2], filterp[3], };


            // Combiner filters
            BiquadParameters cfilterp[Combines];
            StereoEqualizer<2, BiquadFilter> cfilter[Combines]{ cfilterp[0], cfilterp[1], cfilterp[2] };

            // DC offset filter
            BiquadParameters dcoffp[Combines];
            StereoEqualizer<2, BiquadFilter> dcoff[Combines]{ dcoffp[0], dcoffp[1], dcoffp[2] };

            // Anti-aliasing filter
            // 2x Global, 4x Oscillator, 6x Combiner: 12
            EllipticParameters aafp[12];
            EllipticFilter aaf[12];

            void Reset()
            {
                for (auto& i : env) i.m_Phase = -1, i.m_SustainPhase = false, i.sample = 0, i.m_Gate = false, i.m_Down = i.settings.sustain, i.Generate(0);
                for (auto& i : osc) i.phase = 0, i.sample = 0;
                for (auto& i : lfo) i.phase = 0, i.sample = 0;
                for (auto& i : oscs[0]) for (auto& j : i) j = 0;
                for (auto& i : oscs[1]) for (auto& j : i) j = 0;
                for (auto& i : filterp) i.RecalculateParameters(true);
                for (auto& i : cfilterp) i.RecalculateParameters(true);
                for (auto& i : dcoffp) i.RecalculateParameters(true);
            }


            struct SIMDMemory
            {
                SIMDMemory()
                {
                    std::fill_n(_ovsinA, Oscillators * Unison * 8, 0);
                    std::fill_n(_phasoA, Oscillators * Unison * 8, 0);
                    std::fill_n(_dcoffA, Oscillators * Unison * 8, 0);
                    std::fill_n(_enbflA, Oscillators * Unison * 8, 0);
                    std::fill_n(_fldgaA, Oscillators * Unison * 8, 0);
                    std::fill_n(_fldbiA, Oscillators * Unison * 8, 0);
                    std::fill_n(_enbdrA, Oscillators * Unison * 8, 0);
                    std::fill_n(_drvgaA, Oscillators * Unison * 8, 0);
                    std::fill_n(_drvshA, Oscillators * Unison * 8, 0);
                    std::fill_n(_pulswA, Oscillators * Unison * 8, 0);
                    std::fill_n(_bendaA, Oscillators * Unison * 8, 0);
                    std::fill_n(_syncaA, Oscillators * Unison * 8, 0);
                    std::fill_n(_shap1A, Oscillators * Unison * 8, 0);
                    std::fill_n(_shap2A, Oscillators * Unison * 8, 0);
                    std::fill_n(_morphA, Oscillators * Unison * 8, 0);
                    std::fill_n(_shmx1A, Oscillators * Unison * 8, 0);
                    std::fill_n(_shmx2A, Oscillators * Unison * 8, 0);
                    std::fill_n(_phaseA, Oscillators * Unison * 8, 0);
                    std::fill_n(_wtposA, Oscillators * Unison * 8, 0);
                    std::fill_n(_freqcA, Oscillators * Unison * 8, 0);
                    std::fill_n(_gainsA, Oscillators * Unison * 8, 0);
                    std::fill_n(_panniA, Oscillators * Unison * 8, 0);
                    std::fill_n(_makeuA, Oscillators * Unison * 8, 0);
                    std::fill_n(_destL, Oscillators * Unison * 8, nullptr);
                    std::fill_n(_destR, Oscillators * Unison * 8, nullptr);
                    std::fill_n(_phasR, Oscillators * Unison * 8, nullptr);

                }

                float _ovsinA[Oscillators * Unison * 8];
                float _phasoA[Oscillators * Unison * 8];
                float _dcoffA[Oscillators * Unison * 8];
                float _enbflA[Oscillators * Unison * 8];
                float _fldgaA[Oscillators * Unison * 8];
                float _fldbiA[Oscillators * Unison * 8];
                float _enbdrA[Oscillators * Unison * 8];
                float _drvgaA[Oscillators * Unison * 8];
                float _drvshA[Oscillators * Unison * 8];
                float _pulswA[Oscillators * Unison * 8];
                float _bendaA[Oscillators * Unison * 8];
                float _syncaA[Oscillators * Unison * 8];
                float _shap1A[Oscillators * Unison * 8];
                float _shap2A[Oscillators * Unison * 8];
                float _morphA[Oscillators * Unison * 8];
                float _shmx1A[Oscillators * Unison * 8];
                float _shmx2A[Oscillators * Unison * 8];
                float _phaseA[Oscillators * Unison * 8];
                float _wtposA[Oscillators * Unison * 8];
                float _freqcA[Oscillators * Unison * 8];
                float _gainsA[Oscillators * Unison * 8];
                float _panniA[Oscillators * Unison * 8];
                float _makeuA[Oscillators * Unison * 8];
                double* _destL[Oscillators * Unison * 8];
                double* _destR[Oscillators * Unison * 8];
                float* _phasR[Oscillators * Unison * 8];
            } memory;
        };

        Voice voices[Voices]; // All the voices

        double projectTimeSamples = 0; // Amount of samples since start project.
        int lastPressedVoice = 0; // Last voice index that was activated
        bool monophonic = false;

        bool doModulationSync = true; // Gets set to false at end to stop modulation sync thread.

        std::list<int> m_MonoNotePresses; // Stack to keep track of key pressed in mono mode.
        std::array<int, Voices> m_Notes{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }; // Notes pressed for each voice
        std::vector<int> m_Pressed; // Current voices busy
        std::vector<int> m_Available{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }; // Available voices

        ProcessData* processData; // Process data given by DAW.

        cxxpool::thread_pool voiceThreadPool{ Voices }; // Threadpool, 1 thread for each voice.

        std::thread modulationThread{ &Processor::ModSync, this }; // Modulation sync thread.

        ~Processor() override;

        // Midi event handlers
        void Event(Vst::Event& event) override;
        void TriggerVoice(int voice, int pitch, double velocity, bool legato = false);
        void ReleaseVoice(int voice, int pitch, double velocity);
        void NotePress(Vst::Event& event);
        void NoteRelease(Vst::Event& event);

        // Sync the modulation back to the controller in worker thread.
        void ModSync();

        // Main generate, gets called by base class.
        void Generate(ProcessData& data, size_t s) override;

        // Generate buffer for a single voice
        void GenerateVoice(Voice& voice);

#include "Processing/SIMD_Impl.hpp"

        double GenerateSample(size_t channel, ProcessData& data, Voice& voice, double ratio, int osi);
        void CalculateModulation(Voice& voice, double ratio, int ps);
        void UpdateComponentParameters(Voice& voice, double ratio, int ps);

        double Combine(double a, double b, int index, Voice& voice);
        static double CombineSingle(double a, double b, int mode);
    };
}
