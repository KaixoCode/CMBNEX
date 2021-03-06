#include "Processing/Modules.hpp"

namespace Kaixo
{
    constexpr TableAxis note2freqd{ .size = 100000, .begin = -100, .end = 135, .constrained = true };
    const LookupTable<note2freqd> note2freqt = [](double note) { return (440. / 32.) * std::pow(2, ((note - 9) / 12.0)); };
    double noteToFreq(double note) { return note2freqt.get(note); }

    namespace Shapers
    {
        // Interpolating shapers lookup table axis.
        constexpr TableAxis shaperx{ .size = 4096, .begin = -1, .end = 1, .interpolate = true };
        constexpr TableAxis shapery{ .size = 8, .begin = 0, .end = 1, .interpolate = true };
        constexpr TableAxis shaperz{ .size = 1024, .begin = 0, .end = 3, .constrained = true, .interpolate = true };

        // Wave shaper lookup table
        const LookupTable<shaperx, shapery, shaperz> ws0m = [&](double x, double amt, double freq) {
            constexpr double steps = 8;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                Shaper04, Shaper03, Shaper02, Shaper01, noShaper, Shaper11, Shaper12, Shaper13, Shaper14, Shaper14,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x, freq) * (1 - r);
            const double s2 = func2(x, freq) * r;
            return s1 + s2;
        };

        // Phase shaper lookup table
        const LookupTable<shaperx, shapery, shaperz> ps0m = [&](double x, double amt, double freq) {
            constexpr double steps = 8;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                Shaper04, Shaper03, Shaper02, Shaper01, noShaper, Shaper11, Shaper12, Shaper13, Shaper14, Shaper14,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x * 2 - 1, freq) * (1 - r);
            const double s2 = func2(x * 2 - 1, freq) * r;
            return constrain(s1 + s2, -1., 1.) * 0.5 + 0.5;
        };

        const std::array<float, 4097 * 9 * 1025 + 1>& getMainWaveShaper() { return ws0m.table; };
        double mainWaveShaper(double x, double amt, double morph)
        {   
            return ws0m.get(x, amt, morph);
        }

        const std::array<float, 4097 * 9 * 1025 + 1>& getMainPhaseShaper() { return ps0m.table; };
        double mainPhaseShaper(double x, double amt, double morph)
        { 
            return ps0m.get(x, amt, morph);
        }

        // Wavefolder lookup table
        const LookupTable <
            TableAxis{ .size = 100000, .begin = -5, .end = 5 }
        > foldt = [](double x) {
            constexpr static double b = 4;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        };

        const std::array<float, 100001 + 1>& getFold() { return foldt.table; }
        double fold(double x, double bias)
        {
            return foldt.get(myfmod1((x + bias) * 0.25) * 4);
            //constexpr static double b = 4;
            //x += bias;
            //return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        }

        // Drive lookup table, 1d.
        const LookupTable <
            TableAxis{ .size = 100000, .begin = -10, .end = 10, .constrained = true },
            TableAxis{ .size = 1, .begin = 0, .end = 1, .interpolate = true }
        > drivet = [](double x, double amt) {
            const double _pow = std::tanh(x);
            const double _cns = constrain(x, -1., 1.);
            return amt * _pow + (1 - amt) * _cns;
        };

        const std::array<float, 100001 * 2 + 1>& getDrive() { return drivet.table; };
        double drive(double x, double gain, double amt)
        {
            const double _gain = gain * x;
            return drivet.get(_gain, amt);
        }

        // Power curve lookup table, interpolate x-axis once again to prevent aliasing.
        const LookupTable <
            TableAxis{ .size = 1000, .begin = 0, .end = 1, .interpolate = true },
            TableAxis{ .size = 1000, .begin = -1, .end = 1 }
        > powerCurvet = [](double x, double curve) {
            constexpr double MULT = 16;
            const double a = curve < 0 ? (curve * MULT - 1) : curve * MULT + 1;
            constexpr auto b = 0.5;
            if (a >= 0)
            {
                const auto pba = std::pow(b, a);
                return (std::pow(b * x + b, a) - pba) / (1 - pba);
            }
            else
            {
                const auto pba = std::pow(b, -a);
                return 1 - (std::pow(b * (1 - x) + b, -a) - pba) / (1 - pba);
            }
        };

        double powerCurve(double x, double curve) { return powerCurvet.get(x, curve); }
        const std::array<float, 1001 * 1001 + 1>& getPowerCurve() { return powerCurvet.table; };
    }

    namespace Wavetables
    {
        // Amount of harmonics for a saw wave, 2 numbers per octave
        constexpr static int harmonics[]{
             1024,  1024,
             1024,  1024,
             1024,  1024,
             1024,  1024,
             1024,  1024,
              768,   512,
              384,   256,
              192,   128,
               96,    64,
               48,    32,
               24,    16,
               12,     8,
                6,     4,
                3,     2,
                1,     1,
                1,     1,
        };
        double sine(double phase) { return std::sin(phase * std::numbers::pi_v<double> *2); };

        double saw(double phase, double f) 
        {
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics; k++)
                    out += (std::pow(-1, k) / k) * std::sin(2 * std::numbers::pi_v<double> * k * phase);
                return out * (-2 / std::numbers::pi_v<double>);
            };

            return constrain(generator(phase, h) * 0.90, -1, 1);
        };

        double square(double phase, double f) 
        {
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics / 2.; k++)
                    out += std::sin(2 * std::numbers::pi_v<double> * (2 * k - 1) * phase) / (2 * k - 1);
                return out * (4 / std::numbers::pi_v<double>);
            };

            return constrain(generator(phase, h) * 0.82, -1, 1);
        };

        double triangle(double phase, double f) 
        {
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics / 2.; k++)
                    out += std::pow(-1, k) * std::sin(2 * std::numbers::pi_v<double> * (2 * k - 1) * phase) / std::pow(2 * k - 1, 2);
                return out * (8 / (std::numbers::pi_v<double> * std::numbers::pi_v<double>));
            };

            return constrain(generator(myfmod1(phase + 0.5), h) * 0.95, -1, 1);
        };

        // Lookup for basic wavetable, 3rd axis is frequency, prevent aliasing by
        // choosing the table with the correct number of harmonics
        LookupTable <
            TableAxis{ .size = 2048, .begin = 0, .end = 1, .interpolate = true },
            TableAxis{ .size =    3, .begin = 0, .end = 1,. interpolate = true },
            TableAxis{ .size =   32, .begin = 0, .end = 32, .constrained = true }
        > basict = [](double phase, double wtpos, double f) {
            double p = phase;
            if (wtpos < 0.33)
            {
                const double r = wtpos * 3;
                return constrain(triangle(p, f) * r + sine(p) * (1 - r), -1., 1.);
            }
            else if (wtpos < 0.66)
            {
                const double r = (wtpos - 0.33) * 3;
                return constrain(saw(p, f) * r + triangle(p, f) * (1 - r), -1., 1.);
            }
            else
            {
                const double r = (wtpos - 0.66) * 3;
                return constrain(square(p, f) * r + saw(p, f) * (1 - r), -1., 1.);
            }
        };

        const std::array<float, 2049 * 4 * 33 + 1>& getBasicTable() { return basict.table; }

        // Basic wavetable combines sine, saw, square, and triangle into single wavetable.
        bool basicLoaded() { return basict.done; };
        double basic(double phase, double wtpos, double f) 
        {   // Log2(f) to get octave, *2 because 2 tables per octave
            return basict.get(phase, wtpos , fastlog2(f) * 2);
        }
    }

    // ADSR
    void ADSR::Generate(size_t c)
    {   // Only generate on channel == 0
        if (c != 0) return;

        // If phase not -1 (inactive) and before sustain, increment like normal.
        // Unless gate not held.
        if (m_Phase >= 0 && (m_Phase < settings.attack + settings.decay && !m_SustainPhase || !m_Gate))
            m_Phase += settings.timeMult / (double)SAMPLE_RATE;

        else if (m_Gate) // Otherwise if gate keep it at sustain.
            m_Phase = settings.attack + settings.decay, m_SustainPhase = true;

        // If past release, reset back to inactive.
        if (m_Phase > settings.attack + settings.decay + settings.release) 
            m_Phase = -1, m_SustainPhase = false;

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

        if (m_Phase < settings.attack + settings.decay)
            m_SustainPhase = false;
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
            m_Phase = 0, m_SustainPhase = false; // Just reset phase to 0.

        m_Gate = g; // Finally
    }

    // Oscillator
    void Oscillator::Generate(size_t c)
    {   // Only generate at channel == 0
        if (c != 0) return;
        sample = Offset(0, true, false);
    }

    double Oscillator::OffsetClean(double phaseoffset)
    {   // Clean just generates from sub wavetable, no shapers.
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Wavetables::sub(myfmod1(phase + phaseoffset + 100000), settings.wtpos);
    }

    double Oscillator::OffsetSimple(double phaseoffset)
    {   // Simple generates from basic wavetable, only simple shaper
        phase = myfmod1(phase + settings.frequency / SAMPLE_RATE);
        return Shapers::simpleshaper(Wavetables::basic(myfmod1(phase + phaseoffset + 100000), settings.wtpos, settings.frequency), settings.shaper3);
    }

    double Oscillator::Offset(double phaseoffset, bool shaper, bool freeze)
    {
        // Increment phase according to the frequency
        double phase = this->phase;
        this->phase = myfmod1(this->phase + settings.frequency / SAMPLE_RATE);

        double _s = 0;
        const double _pw = settings.pw * 2 - 1;
        const double _sfm = freeze ? 3 * settings.shaperMorph * (30 / settings.frequency) : settings.shaperMorph;

        const double _foffp = (settings.sync * 7 + 1) * (std::abs(settings.bend * 2 - 1) * 4 + 1)
            * (std::abs(settings.pw * 2 - 1) * 4 + 1);

        // Separate cases for pulse width < 0 and > 0
        if (_pw > 0)
        {   // Apply phase shaper, taking into account mix
            const double _ph = !shaper ? phase : 
                Shapers::mainPhaseShaper(phase, settings.shaper, _sfm) * settings.shaperMix + // Shaper mix
                phase * (1 - settings.shaperMix); // Clean mix

            // Pulse width calculations
            const double _d = std::max(0.000001, 1 - _pw); // Calculate phase divisor for pulse width.
            if (_ph / _d > 1) return 0; // When this is > 1, we're past 1 oscillation, remainder returns 0.

            // Here we apply the phase divisor to the phase, and also use the phase offset. Always fmod 1.
            const double _phase = myfmod1((_ph) / _d + phaseoffset + 1000000);

            // Next we apply the wave bender to the phase using the powerCurve, and next we apply the hard sync, fmod1.
            const double _dphase = myfmod1(Shapers::powerCurve(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);

            // Finally we can take the value of the wavetable at the calculated phase.
            const double _wt = Wavetables::basic(_dphase, settings.wtpos, settings.frequency * _foffp);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = !shaper ? _wt :
                Shapers::mainWaveShaper(_wt, settings.shaper2, _sfm) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix 

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = _s1;
        }
        else
        {   // Apply phase shaper, taking into account mix
            const double _ph = !shaper ? phase :
                Shapers::mainPhaseShaper(phase, settings.shaper, _sfm) * settings.shaperMix + // Shaper mix
                phase * (1 - settings.shaperMix); // Clean mix

            // Since pulse width < 0 here, we do 1 + _pw here.
            const double _d = std::max(0.000001, 1 + _pw); // Calculate phase divisor for pulse width.
            if ((1 - _ph) / _d > 1) return 0;  // When this is > 1, we're past 1 oscillation, remainder returns 0.

            // Here we apply the phase divisor to the phase, we need to add the pulse width because start of the waveshape
            // will be 0, not the end, so we need to offset by that amount. Also use the phase offset. Always fmod 1.
            const double _phase = myfmod1((_ph + _pw) / _d + phaseoffset + 1000000);

            // Next we apply the wave bender to the phase using the powerCurve, and next we apply the hard sync, fmod1.
            const double _dphase = myfmod1(Shapers::powerCurve(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);

            // Finally we can take the value of the wavetable at the calculated phase.
            const double _wt = Wavetables::basic(_dphase, settings.wtpos, settings.frequency * _foffp);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = !shaper ? _wt :
                Shapers::mainWaveShaper(_wt, settings.shaper2, _sfm) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = _s1;
        }

        return _s; // Our resulting value is returned!
    }
}