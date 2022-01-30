#include "Processing/Modules.hpp"

namespace Kaixo
{
    // ADSR
    void ADSR::Generate(size_t c)
    {   // Only generate on channel == 0
        if (c != 0) return;

        // If phase not -1 (inactive) and before sustain, increment like normal.
        // Unless gate not held.
        if (m_Phase >= 0 && (m_Phase < settings.attack + settings.decay || !m_Gate))
            m_Phase += settings.timeMult / (double)SAMPLE_RATE;

        else if (m_Gate) // Otherwise if gate keep it at sustain.
            m_Phase = settings.attack + settings.decay;

        // If past release, reset back to inactive.
        if (m_Phase > settings.attack + settings.decay + settings.release) m_Phase = -1;

        sample = Offset(m_Phase); // Get value at current phase and set sample.
    }

    double ADSR::Offset(double p)
    {
        return p < 0 ? 0 // Before start, always 0.
            : p < settings.attack ? // Attack part
                Shapers::powerCurve(p / settings.attack, settings.attackCurve) * // Calculate attack curve multiplier
                (settings.decayLevel - settings.attackLevel) + // Start and stop level determine amount
                settings.attackLevel // Start at attack level
            : p < settings.attack + settings.decay ? // Decay part.
                Shapers::powerCurve((p - settings.attack) / settings.decay, settings.decayCurve) * // Calculate decay curve multiplier
                (settings.sustain - settings.decayLevel) + // Start and stop level determine amount
                settings.decayLevel // Start at decay level
            : p == settings.attack + settings.decay ? // Sustain part
                settings.sustain // Return sustain level
            : p <= settings.attack + settings.decay + settings.release ? // Release part
                m_Down - m_Down * // The release part takes into account the level when the note was released, and goes down from there.
                Shapers::powerCurve((p - settings.attack - settings.decay) / settings.release, settings.releaseCurve) // Release curve.
            : 0; // in any other case return 0;
    }

    void ADSR::Trigger()
    {   // When triggering, reset down level to sustain.
        m_Down = settings.sustain;
        if (m_Gate && settings.legato) // If the gate was already on, and legato
            m_Phase = std::min(m_Phase, settings.attack + settings.decay); // Set to sustain, if phase was already beyond that point.
        else // No legato, just reset phase.
            m_Phase = 0;
    }

    void ADSR::Gate(bool g)
    {   // If gate was true, and now set to false
        if (m_Gate && !g)
        {   // Set phase to release portion
            m_Phase = settings.attack + settings.decay;
            m_Down = sample; // And set down to current level.
        }
        else if (!m_Gate && g) // If no gate, and now true.
        {   // Trigger the envelope.
            Trigger();
        }
        else if (!settings.legato) // If gate was set to same value and not legato
            m_Phase = 0; // Just reset phase to 0.

        m_Gate = g; // Finally
    }

    // Oscillator
    void Oscillator::Generate(size_t c)
    {   // Only generate at channel == 0
        if (c != 0) return;
        sample = Offset(0);
    }

    double Oscillator::OffsetClean(double phaseoffset)
    {   // Clean just generates from sub wavetable, no shapers.
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Wavetables::sub(myfmod1(phase + phaseoffset + 100000), settings.wtpos);
    }

    double Oscillator::OffsetSimple(double phaseoffset)
    {   // Simple generates from basic wavetable, only simple shaper
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Shapers::simpleshaper(Wavetables::basic(myfmod1(phase + phaseoffset + 100000), settings.wtpos), settings.shaper3);
    }

    double Oscillator::Offset(double phaseoffset)
    {
        double _s = 0;
        const double _pw = settings.pw * 2 - 1;

        // Separate cases for pulse width < 0 and > 0
        if (_pw > 0)
        {   // Apply phase shaper, taking into account mix
            const double _ph = 
                Shapers::mainPhaseShaper(phase, settings.shaper, settings.shaperMorph) * settings.shaperMix + // Shaper mix
                phase * (1 - settings.shaperMix); // Clean mix

            // Pulse width calculations
            const double _d = std::max(0.000001, 1 - _pw); // Calculate phase divisor for pulse width.
            const double _p1 = _ph / _d; // When this is > 1, we're past 1 oscillation, remainder returns 0.

            // Here we apply the phase divisor to the phase, and also use the phase offset. Always fmod 1.
            const double _phase = myfmod1((_ph) / _d + phaseoffset + 1000000);

            // Next we apply the wave bender to the phase using the powerCurve, and next we apply the hard sync, fmod1.
            const double _dphase = myfmod1(Shapers::powerCurve(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);

            // Finally we can take the value of the wavetable at the calculated phase.
            const double _wt = Wavetables::basic(_dphase, settings.wtpos);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = 
                Shapers::mainWaveShaper(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(_s1, settings.shaper3);
        }
        else
        {   // Apply phase shaper, taking into account mix
            const double _ph = 
                Shapers::mainPhaseShaper(phase, settings.shaper, settings.shaperMorph) * settings.shaperMix + // Shaper mix
                phase * (1 - settings.shaperMix); // Clean mix

            // Since pulse width < 0 here, we do 1 + _pw here.
            const double _d = std::max(0.000001, 1 + _pw); // Calculate phase divisor for pulse width.
            const double _p1 = (1 - _ph) / _d;  // When this is > 1, we're past 1 oscillation, remainder returns 0.

            // Here we apply the phase divisor to the phase, we need to add the pulse width because start of the waveshape
            // will be 0, not the end, so we need to offset by that amount. Also use the phase offset. Always fmod 1.
            const double _phase = myfmod1((_ph + _pw) / _d + phaseoffset + 1000000);

            // Next we apply the wave bender to the phase using the powerCurve, and next we apply the hard sync, fmod1.
            const double _dphase = myfmod1(Shapers::powerCurve(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);

            // Finally we can take the value of the wavetable at the calculated phase.
            const double _wt = Wavetables::basic(_dphase, settings.wtpos);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = 
                Shapers::mainWaveShaper(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = _p1 > 1 ? 0 : Shapers::simpleshaper(_s1, settings.shaper3);
        }

        // Increment phase according to the frequency
        phase = phase + settings.frequency / SAMPLE_RATE;
        if (phase > 1) phase -= 1;

        return _s; // Our resulting value is returned!
    }
}