#include "Processing/BaseProcessor.hpp"
#include "Processing/Processor.hpp"

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
            //params.resize(data.numSamples);

            int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
            for (int32 index = 0; index < numParamsChanged; index++)
            {
                if (auto* paramQueue = data.inputParameterChanges->getParameterData(index))
                {
                    auto param = paramQueue->getParameterId();

                    // Normal parameter, just assign it to the goal so we can interpolate to this value.
                    if (param < Params::Size)
                    {
                        //ParamValue value;
                        //int32 sampleOffset;
                        //int32 numPoints = paramQueue->getPointCount();
                        //int prevOff = 0;
                        //double prevValue = params.goals[param];
                        //for (int i = 0; i < numPoints; i++)
                        //{
                        //    if (paramQueue->getPoint(i, sampleOffset, value) != kResultTrue) continue;
                        //    //if (numPoints == 1 && sampleOffset == 0)
                        //    //    sampleOffset = data.numSamples;
                        //    
                        //    for (int j = prevOff; j < sampleOffset; j++)
                        //    {
                        //        double ratio = (j - prevOff) / (double)(sampleOffset - prevOff);
                        //        double curVal = prevValue * (1 - ratio) + value * ratio;
                        //        params.persample[param][j] = curVal;
                        //    }
                        //    prevOff = sampleOffset;
                        //    prevValue = value;
                        //}
                        //
                        //for (int j = prevOff; j < data.numSamples; j++)
                        //{
                        //    params.persample[param][j] = value;
                        //}
                        //
                        //params.goals[param] = value;

                        ParamValue end;
                        int32 sampleOffset;
                        int32 numPoints = paramQueue->getPointCount();
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, end) != kResultTrue) continue;
                        params.goals[param] = end;

                    }

                    // Otherwise it's part of a modulation.
                    else
                    {
                        ParamValue end;
                        int32 sampleOffset;
                        int32 numPoints = paramQueue->getPointCount();
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, end) != kResultTrue) continue;

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
        swapBuffer.prepare(data.numSamples);

        constexpr static auto _handler = [](auto code, auto ep) {

            // Handle exceptions
            switch (code)
            {
            case STATUS_ILLEGAL_INSTRUCTION:
            {
                switch (Processor::SIMDPATH)
                {
                case SIMDPATH::s512: Processor::SIMDPATH = SIMDPATH::s256; break;
                case SIMDPATH::s256: Processor::SIMDPATH = SIMDPATH::s128; break;
                default: Processor::SIMDPATH = SIMDPATH::s0; break;
                }

                break;
            }
            }

            return EXCEPTION_EXECUTE_HANDLER;
        };

        __try
        {
            Generate(data, data.numSamples); // Call generate to fill buffer
        }
        __except (_handler(GetExceptionCode(), GetExceptionInformation()))
        {
            std::cout << "Caught exception \n";
        }

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
        
        // While we can read parameter name
        while (char8* name = streamer.readStr8())
        {
            for (size_t i = 0; i < Params::Size; i++)
            {   // Find parameter
                if (std::strcmp(ParamNames[i].name, name) == 0)
                {   // Apply value
                    streamer.readDouble(params.goals[i]);
                    if (i < Params::ModCount) // If modable
                    {   // Set mod goals + amount
                        hasmod[i] = false;
                        for (int j = 0; j < ModAmt; j++)
                        {   // Keep track of if mod, for efficiency
                            hasmod[i] |= modgoals[(i * ModAmt + j)] > 0;
                            streamer.readDouble(modgoals[(i * ModAmt + j)]);
                            streamer.readDouble(modamount[(i * ModAmt + j)]);
                        }
                    }
                    break;
                }
            }
        }

        return kResultOk;
    }

    tresult PLUGIN_API Instrument::getState(IBStream* state)
    {
        IBStreamer streamer(state, kLittleEndian);

        for (size_t i = 0; i < Params::Size; i++)
        {
            // First write parameter name, to identify.
            streamer.writeStr8(ParamNames[i].name);
            streamer.writeDouble(params.goals[i]); // Set value
            if (i < Params::ModCount) // If modable
            {   // Add mod amount + goals
                for (int j = 0; j < ModAmt; j++)
                {
                    streamer.writeDouble(modgoals[(i * ModAmt + j)]);
                    streamer.writeDouble(modamount[(i * ModAmt + j)]);
                }
            }
        }

        return kResultOk;
    }
}