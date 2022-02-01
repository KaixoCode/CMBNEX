#pragma once
#include "pch.hpp"
#include "CIDs.hpp"
#include "Modules.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/Utils.hpp"

namespace Kaixo
{
    // Buffer that's filled in separate threads per voice.
    // use the lock to access the buffer to make it threadsafe.
    struct Buffer
    {
        std::mutex lock;
        float* left = nullptr;
        float* right = nullptr;
        size_t size = 0;
        size_t amount = 0;

        ~Buffer()
        {
            delete[] right;
            delete[] left;
        }

        void resize(size_t s)
        {
            if (s > size)
            {
                size = s;
                delete[] left;
                delete[] right;
                left = new float[size];
                right = new float[size];
            }
        }
    };

    // Base for an instrument, automatically puts parameters in array of doubles.
    class Instrument : public AudioEffect
    {
    public:
        Buffer swapBuffer; // Swap buffer is filled with samples concurrently by the voices

        struct ParamList // List of params, uses interpolation from current to goal value.
        {
            double values[Params::Size];
            double goals[Params::Size];

            double operator[](std::pair<size_t, double> i) { return (1 - i.second) * values[i.first] + i.second * goals[i.first]; }
        } params;

        bool hasmod[Params::ModCount]; // Array of bools to tell if param has a modulation (for efficiency)
        double modgoals[Params::ModCount * ModAmt]; // Goals of modulations (Env/LFO/etc.)
        double modamount[Params::ModCount * ModAmt]; // Amount of modulation

        Instrument() { setControllerClass(kCMBNEXControllerUID); }
        ~Instrument() override {};

        tresult PLUGIN_API initialize(FUnknown* context) override;
        tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) override;

        tresult PLUGIN_API process(ProcessData& data) override;

        tresult PLUGIN_API setState(IBStream* state) override;
        tresult PLUGIN_API getState(IBStream* state) override;

        virtual void Generate(ProcessData& data, size_t samples) = 0; // Generate into swapBuffer
        virtual void Event(Vst::Event& e) = 0; // Midi events
    };
}