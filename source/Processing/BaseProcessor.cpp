#include "Processing/BaseProcessor.hpp"

namespace Kaixo
{
    tresult PLUGIN_API Instrument::initialize(FUnknown* context)
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

    tresult PLUGIN_API Instrument::canProcessSampleSize(int32 symbolicSampleSize)
    {
        switch (symbolicSampleSize)
        {
        case kSample32: return kResultTrue;
        case kSample64: return kResultTrue;
        default: return kResultFalse;
        };
    }

    tresult PLUGIN_API Instrument::process(ProcessData& data)
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

    tresult PLUGIN_API Instrument::setState(IBStream* state)
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

    tresult PLUGIN_API Instrument::getState(IBStream* state)
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
}