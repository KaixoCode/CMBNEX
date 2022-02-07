#include "Processing/Modules.hpp"

namespace Kaixo
{
    constexpr TableAxis note2freqd{ .size = 100000, .begin = -100, .end = 135, .constrained = true };
    const LookupTable<note2freqd> note2freqt = [](double note) { return (440. / 32.) * std::pow(2, ((note - 9) / 12.0)); };
    double noteToFreq(double note) { return note2freqt.get(note); }

    namespace Shapers
    {
        // Interpolating shapers lookup table axis.
        constexpr TableAxis shaperx{ .size = 2048, .begin = -1, .end = 1, .interpolate = true };
        constexpr TableAxis shapery{ .size = 1000, .begin = 0, .end = 1 };

        // Wave shaper 0% morph lookup table
        const LookupTable<shaperx, shapery> ws0m = [&](double x, double amt) {
            constexpr double steps = 4;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                shaper7, shaper8, noShaper, shaper0, shaper6, shaper6,
            };

            const int i = amt * 4 + 1;
            const double r = myfmod1(amt * 4);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x, 1 - r);
            const double s2 = func2(x, r);
            return s1 + s2;
        };

        // Wave shaper 100% morph lookup table
        const LookupTable<shaperx, shapery> ws100m = [&](double x, double amt) {
            constexpr double steps = 4;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                shaper3, shaper2, shaper9, shaper5, shaper1, shaper1,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1(x, 1 - r);
            const double s4 = func2(x, r);
            return s3 + s4;
        };

        // Phase shaper 0% morph lookup table
        const LookupTable<shaperx, shapery> ps0m = [&](double x, double amt) {
            constexpr double steps = 4;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                shaper7, shaper8, noShaper, shaper0, shaper6, shaper6,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x, 1 - r);
            const double s2 = func2(x, r);
            return constrain(s1 + s2, 0., 1.);
        };

        // Phase shaper 100% morph lookup table
        const LookupTable<shaperx, shapery> ps100m = [&](double x, double amt) {
            constexpr double steps = 4;

            constexpr double(*funcs[(int)steps + 2])(double, double){
                shaper3, shaper2, shaper9, shaper5, shaper1, shaper1,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1(x, 1 - r);
            const double s4 = func2(x, r);
            return constrain(s3 + s4, 0., 1.);
        };

        double mainWaveShaper(double x, double amt, double morph)
        {   // Interpolate between 0% and 100% morph tables, since a 3d lookup would use too much memory
            return (1 - morph) * ws0m.get(x, amt) + morph * ws100m.get(x, amt);
        }

        double mainPhaseShaper(double x, double amt, double morph)
        {   // Interpolate between 0% and 100% morph tables, since a 3d lookup would use too much memory
            return (1 - morph) * ps0m.get(x, amt) + morph * ps100m.get(x, amt);
        }

        // Wavefolder lookup table
        //const LookupTable <
        //    TableAxis{ .size = 100000, .begin = -4, .end = 4 }
        //> foldt = [](double x) {
        //    constexpr static double b = 4;
        //    return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        //};

        double fold(double x, double bias)
        {
            //return foldt.get(myfmod1((x + bias) * 0.25) * 4);
            constexpr static double b = 4;
            x += bias;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        }

        // Drive lookup table, 1d.
        const LookupTable <
            TableAxis{ .size = 100000, .begin = -10, .end = 10, .constrained = true }
        > drivet = [](double x) {
            const double _abs = std::max(std::abs(x), 0.000001);
            const double _pow = (x / _abs) * (1 - std::exp((-x * x) / _abs));
            return _pow;
        };

        double drive(double x, double gain, double amt)
        {   // Drive table is 1d, interpolate between constrained value.
            const double _gain = gain * x;
            return drivet.get(_gain) * amt + (constrain(_gain, -1., 1.)) * (1 - amt);
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
    }

    namespace Wavetables
    {
        const LookupTable <
            TableAxis{ .size = 1000, .begin = 0, . end = 1, .interpolate = true }
        > sinet = [](double phase) {            
            return std::sin(phase * std::numbers::pi_v<double> *2);
        };

        double sine(double phase, double f) { return sinet.get(phase); };

        const LookupTable <
            TableAxis{ .size = 2048, .begin = 0, . end = 1, .interpolate = true },
            TableAxis{ .size = 32, .begin = 0, .end = 32, .constrained = true }
        > sawt = [](double phase, double f) {
            constexpr static int harmonics[]{ 
                1024, 1024, 
                1024, 1024, 
                1024, 1024, 
                1024, 1024, 
                1024, 1024,
                768,  512, 
                384,  256, 
                192,  128, 
                96,   64, 
                48,   32,
                24,   16,
                12,   8, 
                6,    4,
                3,    2, 
                1,    1,
                1,    1,
            };
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics; k++)
                    out += (std::pow(-1, k) / k) * std::sin(2 * std::numbers::pi_v<double> * k * phase);
                return out * (-2 / std::numbers::pi_v<double>);
            };

            return constrain(generator(phase, h) * 0.90, -1, 1);
        };

        double saw(double phase, double f) { return sawt.get(phase, (std::log(f) / std::numbers::ln2_v<double>) * 2); };

        const LookupTable <
            TableAxis{ .size = 2048, .begin = 0, . end = 1, .interpolate = true },
            TableAxis{ .size = 32, .begin = 0, .end = 32, .constrained = true }
        > squaret = [](double phase, double f) {
            constexpr static int harmonics[]{
                4096, 4096,
                4096, 4096,
                4096, 4096,
                2048, 2048,
                1024, 1024,
                768,  512,
                384,  256,
                192,  128,
                96,   64,
                48,   32,
                24,   16,
                12,   8,
                6,    4,
                3,    2,
                1,    1,
                1,    1,
            };
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics / 2.; k++)
                    out += std::sin(2 * std::numbers::pi_v<double> * (2 * k - 1) * phase) / (2 * k - 1);
                return out * (4 / std::numbers::pi_v<double>);
            };

            return constrain(generator(phase, h) * 0.82, -1, 1);
        };

        double square(double phase, double f) { return squaret.get(phase, (std::log(f) / std::numbers::ln2_v<double>) * 2); };

        const LookupTable <
            TableAxis{ .size = 2048, .begin = 0, . end = 1, .interpolate = true },
            TableAxis{ .size = 32, .begin = 0, .end = 32, .constrained = true }
        > trianglet = [](double phase, double f) {
            constexpr static int harmonics[]{
                4096, 4096,
                4096, 4096,
                4096, 4096,
                2048, 2048,
                1024, 1024,
                768,  512,
                384,  256,
                192,  128,
                96,   64,
                48,   32,
                24,   16,
                12,   8,
                6,    4,
                3,    2,
                1,    1,
                1,    1,
            };
            const double h = harmonics[(int)f];

            constexpr static auto generator = [](double phase, int harmonics) {
                double out = 0;
                for (int k = 1; k <= harmonics / 2.; k++)
                    out += std::pow(-1, k) * std::sin(2 * std::numbers::pi_v<double> * (2 * k - 1) * phase) / std::pow(2 * k - 1, 2);
                return out * (8 / (std::numbers::pi_v<double> * std::numbers::pi_v<double>));
            };

            return constrain(generator(myfmod1(phase + 0.5), h) * 0.95, -1, 1);
        };

        double triangle(double phase, double f) { return trianglet.get(phase, (std::log(f) / std::numbers::ln2_v<double>) * 2); }

        // Lookup for basic wavetable, interpolate x-axis, since that's the phase
        // and that needs to be smooth to avoid aliasing.
        //const LookupTable <
        //    TableAxis{ .size = 1000, .begin = 0, .end = 1, .interpolate = true },
        //    TableAxis{ .size = 1000, .begin = 0, .end = 1 }
        //> basict = [](double phase, double wtpos) {
        //    double p = phase;
        //    if (wtpos < 0.33)
        //    {
        //        const double r = wtpos * 3;
        //        return triangle(p, wtpos) * r + sine(p, wtpos) * (1 - r);
        //    }
        //    else if (wtpos < 0.66)
        //    {
        //        const double r = (wtpos - 0.33) * 3;
        //        return saw(p, wtpos) * r + triangle(p, wtpos) * (1 - r);
        //    }
        //    else
        //    {
        //        const double r = (wtpos - 0.66) * 2.9;
        //        return square(p, wtpos) * r + saw(p, wtpos) * (1 - r);
        //    }
        //};

        // Basic wavetable combines sine, saw, square, and triangle into single wavetable.
        double basic(double phase, double wtpos, double f) 
        {
            double p = phase;
            if (wtpos < 0.33)
            {
                const double r = wtpos * 3;
                return triangle(p, f) * r + sine(p, f) * (1 - r);
            }
            else if (wtpos < 0.66)
            {
                const double r = (wtpos - 0.33) * 3;
                return saw(p, f) * r + triangle(p, f) * (1 - r);
            }
            else
            {
                const double r = (wtpos - 0.66) * 3;
                return square(p, f) * r + saw(p, f) * (1 - r);
            }
            //return basict.get(phase, wtpos);
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
        return Shapers::simpleshaper(Wavetables::basic(myfmod1(phase + phaseoffset + 100000), settings.wtpos, settings.frequency), settings.shaper3);
    }

    double Oscillator::Offset(double phaseoffset)
    {
        // Increment phase according to the frequency
        double phase = this->phase;
        this->phase = myfmod1(this->phase + settings.frequency / SAMPLE_RATE);

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
            if (_ph / _d > 1) return 0; // When this is > 1, we're past 1 oscillation, remainder returns 0.

            // Here we apply the phase divisor to the phase, and also use the phase offset. Always fmod 1.
            const double _phase = myfmod1((_ph) / _d + phaseoffset + 1000000);

            // Next we apply the wave bender to the phase using the powerCurve, and next we apply the hard sync, fmod1.
            const double _dphase = myfmod1(Shapers::powerCurve(_phase, settings.bend * 2 - 1) * (settings.sync * 7 + 1) + 1000000);

            // Finally we can take the value of the wavetable at the calculated phase.
            const double _wt = Wavetables::basic(_dphase, settings.wtpos, settings.frequency);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = 
                Shapers::mainWaveShaper(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix 

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = Shapers::simpleshaper(_s1, settings.shaper3);
        }
        else
        {   // Apply phase shaper, taking into account mix
            const double _ph = 
                Shapers::mainPhaseShaper(phase, settings.shaper, settings.shaperMorph) * settings.shaperMix + // Shaper mix
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
            const double _wt = Wavetables::basic(_dphase, settings.wtpos, settings.frequency);

            // Apply the main waveshaper, taking into account mix
            const double _s1 = 
                Shapers::mainWaveShaper(_wt, settings.shaper2, settings.shaperMorph) * settings.shaper2Mix + // Shaper mix
                _wt * (1 - settings.shaper2Mix); // Clean mix

            // Taking into account the pulse width remainer, return either 0 or result from simple waveshaper.
            _s = Shapers::simpleshaper(_s1, settings.shaper3);
        }

        return _s; // Our resulting value is returned!
    }
}