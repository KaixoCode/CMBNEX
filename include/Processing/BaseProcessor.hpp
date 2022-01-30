#pragma once
#include "pch.hpp"
#include "CIDs.hpp"
#include "Modules.hpp"
#include "Components/Parameter.hpp"
#include "Utils/ThreadPool.hpp"
#include "Utils/Utils.hpp"
#include "Views/MainView.hpp"

namespace Kaixo
{
    /**
     * Buffer that's filled in separate threads per voice.
     * use the lock to access the buffer to make it threadsafe.
     */
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

    /**
     * Base for an instrument, automatically puts parameters in array of doubles.
     */
    class Instrument : public AudioEffect
    {
    public:
        Buffer swapBuffer;

        Instrument() { setControllerClass(kCMBNEXControllerUID); }
        ~Instrument() override {};

        tresult PLUGIN_API initialize(FUnknown* context) override
        {
            tresult result = AudioEffect::initialize(context);

            if (result != kResultOk) return result;
            for (int i = 0; i < Params::Size; i++) // initialize to reset value
                params.values[i] = params.goals[i] = ParamNames[i].reset;

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
                modgoals[i] = 0, modamount[i] = 0.5;

            addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
            addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
            addEventInput(STR16("Event In"), 1);

            return kResultOk;
        }

        tresult PLUGIN_API terminate() override { return AudioEffect::terminate(); }
        tresult PLUGIN_API setActive(TBool state) override { return AudioEffect::setActive(state); }
        tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) override { return AudioEffect::setupProcessing(newSetup); }

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
            // Process parameter changes
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

                        // Normal parameter, just assign it to the goal so we can interpolate to this value.
                        if (param < Params::Size)
                            params.goals[param] = end;

                        // Otherwise it's part of a modulation.
                        else
                        {
                            // Adjust value and check if it's a goal or amount.
                            param -= Params::Size;
                            if (param % 2 == 0)
                            {
                                // If it's a goal, set new goal
                                param /= 2;
                                modgoals[param] = end;

                                // Also change hasmod to keep up (done for efficiency reasons)
                                int index = std::floor(param / (double)ModAmt);
                                hasmod[index] = false;
                                for (int i = 0; i < ModAmt; i++)
                                    hasmod[index] |= modgoals[index * ModAmt + i] > 0;
                            }
                            else
                            {   // Otherwise it's an amount
                                (param -= 1) /= 2;
                                modamount[param] = end;
                            }
                        }
                    }
                }
            }

            // Events, mainly midi events
            IEventList* eventList = data.inputEvents;
            if (eventList)
            {
                int32 numEvent = eventList->getEventCount();
                for (int32 i = 0; i < numEvent; i++)
                {
                    Vst::Event event;
                    if (eventList->getEvent(i, event) == kResultOk)
                    {
                        Event(event); // handled by derived class.
                    }
                }
            }

            // Prepare the swap buffer with amount of samples, fill with 0s
            swapBuffer.resize(data.numSamples);
            swapBuffer.amount = data.numSamples;
            for (int i = 0; i < data.numSamples; i++)
                swapBuffer.left[i] = swapBuffer.right[i] = 0;
            Generate(data, data.numSamples); // Call generate to fill buffer

            // Then assign the buffer to the output, depending on process mode
            void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);
            if (data.symbolicSampleSize == kSample32)
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    if (data.outputs[0].numChannels == 2)
                    {
                        ((Sample32**)out)[0][sample] = swapBuffer.left[sample];
                        ((Sample32**)out)[1][sample] = swapBuffer.right[sample];
                    }
                }
            }
            else
            {
                for (size_t sample = 0; sample < data.numSamples; sample++)
                {
                    if (data.outputs[0].numChannels == 2)
                    {
                        ((Sample64**)out)[0][sample] = swapBuffer.left[sample];
                        ((Sample64**)out)[1][sample] = swapBuffer.right[sample];
                    }
                }
            }

            // Copy over the goals to the actual values for the params
            std::memcpy(params.values, params.goals, Params::Size * sizeof(double));

            return kResultOk;
        }

        tresult PLUGIN_API setState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.readDouble(params.goals[i]);

            for (int i = 0; i < Params::ModCount; i++)
                hasmod[i] = false;

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                int index = std::floor(i / ModAmt);
                hasmod[index] |= modgoals[i] > 0; // Keep track of if mod, for efficiency

                streamer.readDouble(modgoals[i]);
                streamer.readDouble(modamount[i]);
            }

            return kResultOk;
        }

        tresult PLUGIN_API getState(IBStream* state) override
        {
            IBStreamer streamer(state, kLittleEndian);

            for (size_t i = 0; i < Params::Size; i++)
                streamer.writeDouble(params.goals[i]);

            for (int i = 0; i < Params::ModCount * ModAmt; i++)
            {
                streamer.writeDouble(modgoals[i]);
                streamer.writeDouble(modamount[i]);
            }

            return kResultOk;
        }

        virtual void Generate(ProcessData& data, size_t samples) = 0;
        virtual void Event(Vst::Event& e) = 0;

        struct ParamList // List of params, uses interpolation from current to goal value.
        {
            double values[Params::Size];
            double goals[Params::Size];

            double operator[](std::pair<size_t, double> i) { return (1 - i.second) * values[i.first] + i.second * goals[i.first]; }
        } params;

        bool hasmod[Params::ModCount]; // Array of bools to tell if param has a modulation (for efficiency)
        double modgoals[Params::ModCount * ModAmt]; // Goals of modulations (Env/LFO/etc.)
        double modamount[Params::ModCount * ModAmt]; // Amount of modulation
    };
}