#pragma once
#include "pch.hpp"
#include "Processing/Filter.hpp"

namespace Kaixo
{
    // Settings for a lookup table axis.
    struct TableAxis 
    { 
        int size; // Size of the axis
        double begin; // Start of the range of values for this axis
        double end;   // End of the range of values for this axis
        bool constrained = false; // Constrain value when looking up
        bool interpolate = false; // Interpolate result from lookup for smoother curve
    };

    // Simple lookup table, using a 1d array of doubles.
    template<TableAxis...>
    struct LookupTable;

    // 1 Dimensional lookup
    template<TableAxis Size>
    struct LookupTable<Size>
    {
        constexpr LookupTable(auto&& powerCurve)
        {   // Fill the table using the provided function.
            for (int i = 0; i < Size.size + 1; i++)
                table[i] = powerCurve((i / (double)Size.size) * (Size.end - Size.begin) + Size.begin);
        }

        constexpr double get(double val) const
        {   // Constrain value if necessary
            if constexpr (Size.constrained) val = constrain(val, Size.begin, Size.end);
            if constexpr (Size.interpolate) // If we need to interpolate
            {   // Get prefered value
                double v = Size.size * (val - Size.begin) / (Size.end - Size.begin);
                int a = (int)(v); // Round value using integer casting
                double r = v - a; // Get ratio towards next integer index
                return (1 - r) * table[a] + r * table[a + 1]; // Interpolate both indices using ratio
            }
            else // No interpolating, we cast to integer.
                return table[(int)(Size.size * (val - Size.begin) / (Size.end - Size.begin))];
        }

        float table[Size.size + 1];
    };

    template<TableAxis Width, TableAxis Height>
    struct LookupTable<Width, Height>
    {
        constexpr LookupTable(auto&& powerCurve)
        {   // Fill table
            for (int i = 0; i < Width.size + 1; i++)
                for (int j = 0; j < Height.size + 1; j++)
                    table[i + j * (Width.size + 1)] = // Convert 2d index to 1d index, and calculate correct argument
                    powerCurve((i / (double)Width.size) * (Width.end - Width.begin) + Width.begin, // values using the range
                      (j / (double)Height.size) * (Height.end - Height.begin) + Height.begin); // provided in the TableAxis
        }

        constexpr double get(double x, double y) const
        {   // Constrain axis if necessary
            if constexpr (Width.constrained) x = constrain(x, Width.begin, Width.end);
            if constexpr (Height.constrained) y = constrain(y, Height.begin, Height.end);
            if constexpr (Width.interpolate && Height.interpolate) // If both interpolate
            {   // Calculate index and ratios for both indices
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr; // We average the ratios of both indices.
                return ((2 - _xrem - _yrem) * 0.5) * table[_xr + (Width.size + 1) * _yr] +
                    ((_xrem + _yrem) * 0.5) * table[_xr + 1 + (Width.size + 1) * (_yr + 1)];
            }
            else if constexpr (Width.interpolate)
            {   // Single axis interpolation, works the same as in 1d lookup table
                const double _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _xr = (int)(_x);
                const double _xrem = _x - _xr;
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                return (1 - _xrem) * table[_xr + (Width.size + 1) * _y] + 
                    _xrem * table[_xr + 1 + (Width.size + 1) * _y];
            }
            else if constexpr (Height.interpolate)
            {   // Single axis interpolation again.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const double _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                const int _yr = (int)(_y);
                const double _yrem = _y - _yr;
                return (1 - _yrem) * table[_x + (Width.size + 1) * _yr] + 
                    _yrem * table[_x + (Width.size + 1) * (_yr + 1)];
            }
            else
            {   // No interpolation, calculate 2d indices first, then convert to 1d index.
                const int _x = Width.size * (x - Width.begin) / (Width.end - Width.begin);
                const int _y = Height.size * (y - Height.begin) / (Height.end - Height.begin);
                return table[_x + (Width.size + 1) * _y];
            }
        }

        float table[(Width.size + 1) * (Height.size + 1)];
    };

    // Convert note number to frequency, uses lookup table 
    // to be more efficient and fast.
    constexpr TableAxis note2freqd{ .size = 100000, .begin = -100, .end = 135, .constrained = true };
    const static inline LookupTable<note2freqd> note2freqt = [](double note) { return (440. / 32.) * pow(2, ((note - 9) / 12.0)); };
    inline double noteToFreq(double note) { return note2freqt.get(note); }

    // Fast random number generator, uses bitwise operators
    // to generate a random number between -1 and 1.
    static inline unsigned long x = 123456789, y = 362436069, z = 521288629;
    inline double random() {
        unsigned long t;
        x ^= x << 16, x ^= x >> 5, x ^= x << 1, t = x, x = y, y = z, z = t ^ x ^ y;
        return 2 * (z % 32767) / 32767. - 0.5;
    }

    // Wave shapers
    namespace Shapers
    {
        // No shaper, for convenience in interpolating shapers
        inline double noShaper(double x, double amt) { return x * amt; }

        // All these shapers are arbitrary wave shapers pulled out of thin air
        // for the sake of creating weird harmonic content.
        inline double shaper1(double x, double amt)
        {
            const double cos1 = std::cos(159 * x);
            const double sin1 = std::sin(59 + 132 * amt * x - 7.8);
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper2(double x, double amt)
        {
            const double cos1 = std::cos((amt * 518 + 22) * x * x);
            const double sin1 = std::sin(10 * x * x);
            const double m1 = 0.64 * x;
            const double fp = 9 * m1 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper3(double x, double amt)
        {
            const double x1 = x * 5;
            const double cos1 = std::cos((42 + amt * 2) * x);
            const double cos2 = std::cos((23 + amt * 2) * x);
            const double sin1 = std::sin((74 + amt * 2) * x1 * x1 * x1);
            const double sin2 = std::sin((80 + 40 * amt) * x);
            const double res = (0.1 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper8(double x, double amt)
        {
            const double x1 = x * 8;
            const double cos1 = std::sin((3 + amt * 2) * x);
            const double cos2 = std::cos((6 + amt * 9) * x);
            const double sin1 = std::sin((2 + amt * 2) * x1 * x1 * x1 * x1);
            const double sin2 = std::cos((4 + 3 * amt) * x);
            const double res = (0.9 * cos2 + 0.1 * sin1 * sin1 - 0.4
                * cos1 * cos1 * cos1 + 0.4 * sin2);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper9(double x, double amt)
        {
            const double cos1 = std::cos((amt * 3 + 22) * x * 129);
            const double sin1 = std::sin(3 * x * x * x);
            const double m1 = 0.64 * x;
            const double fp = 3 * m1 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper7(double x, double amt)
        {
            const double cos1 = std::cos(19 * x * x * x);
            const double sin1 = std::sin(1 + 3 * amt * x * x - 7.8);
            const double sin2 = std::sin(x * x);
            const double res = 1.2 * (cos1 * cos1 + sin1 * sin1 * sin1 + cos1 * sin1 * sin2);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper6(double x, double amt)
        {
            const double cos1 = std::sin((amt * 3 + 22) * x * x);
            const double sin1 = std::cos(54 * x * x * amt);
            const double m1 = 0.64 * x;
            const double m3 = 0.58 * x * cos1;
            const double fp = 3 * m1 * m1 * m3;
            const double sp = x * x * x * fp * m1;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper5(double x, double amt)
        {
            const double cos1 = std::cos((amt * 5 + 22) * x * x);
            const double sin1 = std::sin(312 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
        }

        inline double shaper0(double x, double amt)
        {
            const double cos1 = std::cos((amt * 3 + 4) * x * x * x);
            const double sin1 = std::sin(5 * x * x);
            const double m1 = 0.1 * x;
            const double fp = 2 * m1 * m1 * m1;
            const double sp = x * x * x * x;
            const double res = (fp * cos1 + sin1 + sp);
            return constrain(res, -1., 1.) * amt;
        }

        // Interpolating shapers lookup table axis.
        constexpr TableAxis shaperx{ .size = 5000, .begin = -1, .end = 1, .constrained = true };
        constexpr TableAxis shapery{ .size = 1000, .begin = 0, .end = 1 };

        // Wave shaper 0% morph lookup table
        const static LookupTable<shaperx, shapery> ws0m = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static double(*funcs[(int)steps + 2])(double, double){
                shaper7, shaper8, noShaper, shaper0, shaper6, shaper6,
            };

            const int i = amt * 4 + 1;
            const double r = myfmod1(amt * 4);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x, 1 - r) * (1 - r);
            const double s2 = func2(x, r) * r;
            return s1 + s2;
        };

        // Wave shaper 100% morph lookup table
        const static LookupTable<shaperx, shapery> ws100m = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static double(*funcs[(int)steps + 2])(double, double){
                shaper3, shaper2, shaper9, shaper5, shaper1, shaper1,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1(x, 1 - r) * (1 - r);
            const double s4 = func2(x, r) * r;
            return s3 + s4;
        };

        // Phase shaper 0% morph lookup table
        const static LookupTable<shaperx, shapery> ps0m = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static double(*funcs[(int)steps + 2])(double, double){
                shaper7, shaper8, noShaper, shaper0, shaper6, shaper6,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s1 = func1(x, 1 - r) * (1 - r);
            const double s2 = func2(x, r) * r;
            return constrain(s1 + s2, 0., 1.);
        };

        // Phase shaper 100% morph lookup table
        const static LookupTable<shaperx, shapery> ps100m = [&](double x, double amt) {
            constexpr static double steps = 4;

            constexpr static double(*funcs[(int)steps + 2])(double, double){
                shaper3, shaper2, shaper9, shaper5, shaper1, shaper1,
            };

            const int i = amt * steps + 1;
            const double r = myfmod1(amt * steps);
            const auto& func1 = funcs[i - 1];
            const auto& func2 = funcs[i];
            const double s3 = func1(x, 1 - r) * (1 - r);
            const double s4 = func2(x, r) * r;
            return constrain(s3 + s4, 0., 1.);
        };

        // Main wave shaper, is an interpolating shaper, going over 5 shapers
        // and the ability to morph between an aditional 5 shapers.
        inline double mainWaveShaper(double x, double amt, double morph)
        {   // Interpolate between 0% and 100% morph tables, since a 3d lookup would use too much memory
            return (1 - morph) * ws0m.get(x, amt) + morph * ws100m.get(x, amt);
        }

        // Main phase shaper, the same as the wave shaper, except the output
        // is always constrained between 0 and 1.
        inline double mainPhaseShaper(double x, double amt, double morph)
        {   // Interpolate between 0% and 100% morph tables, since a 3d lookup would use too much memory
            return (1 - morph) * ps0m.get(x, amt) + morph * ps100m.get(x, amt);
        }

        // Simple waveshaper that interpolates between x^(1/5), x, and x^5.
        inline double simpleshaper(double x, double amt)
        {   // If amt < 0.5 we have x^(1/5)
            if (amt < 0.5)
            {
                if (x < 0) // to prevent imaginary numbers, separate case for x < 0
                {
                    const double p2 = std::pow(-x, 1. / 5.);
                    double r = amt * 2;
                    return x * r + -p2 * (1 - r); // Interpolate between x^(1/5) and x
                }
                const double p2 = std::pow(x, 1. / 5.);
                double r = amt * 2;
                return x * r + p2 * (1 - r);
            }
            else
            {
                const double p1 = x * x * x * x * x;
                double r = (amt - 0.5) * 2;
                return p1 * r + x * (1 - r); // Interpolate between x^5 and x
            }
        }

        const static inline LookupTable<
            TableAxis{.size = 100000, .begin = -4, .end = 4 }
        > foldt = [](double x) {
            constexpr static double b = 4;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        };

        // Wavefolder, basically bounces back signal above 1 and below -1 so it folds
        // in on itself, creating aditional harmonics
        inline double fold(double x, double bias)
        {
            //return foldt.get(myfmod1((x + bias) * 0.25) * 4);
            constexpr static double b = 4;
            x += bias;
            return 4 / b * (4.0 * (std::abs(1 / b * x + 1 / b - std::round(1 / b * x + 1 / b)) - 1 / b) - b / 4 + 1);
        }

        // Drive lookup table, 1d.
        const static inline LookupTable<
            TableAxis{.size = 100000, .begin = -10, .end = 10, .constrained = true }
        > drivet = [](double x) {
            const double _abs = std::max(std::abs(x), 0.000001);
            const double _pow = (x / _abs) * (1 - std::exp((-x * x) / _abs));
            return _pow;
        };

        inline double drive(double x, double gain, double amt)
        {   // Drive table is 1d, interpolate between constrained value.
            const double _gain = gain * x;
            return drivet.get(_gain) * amt + (constrain(_gain, -1., 1.)) * (1 - amt);
        }

        // Power curve lookup table, interpolate x-axis once again to prevent aliasing.
        const static inline LookupTable <
            TableAxis{ .size = 1000, .begin = 0, .end = 1, .interpolate = true },
            TableAxis{ .size = 1000, .begin = -1, .end = 1 }
        > powerCurvet = [](double x, double curve) {
            constexpr double MULT = 16;
            const double a = curve < 0 ? (curve * MULT - 1) : curve * MULT + 1;
            constexpr static auto b = 0.5;
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

        // power curve interpolates between x^(1/16), x, and x^16, and makes
        // sure the output curve perfectly outputs between 0 and 1.
        inline double powerCurve(double x, double curve) { return powerCurvet.get(x, curve); }
    }

    using Wavetable = double(*)(double, double);
    namespace Wavetables
    {
        // Simple wavetables
        inline double sine(double phase, double wtpos) { return std::sin(phase * std::numbers::pi_v<double> *2); };
        inline double saw(double phase, double wtpos) { return phase * 2 - 1; };
        inline double square(double phase, double wtpos) { return std::floor(phase + 0.5) * 2. - 1.; };
        inline double triangle(double phase, double wtpos) { return 4 * std::abs(0.5 - phase) - 1; }

        // Lookup for basic wavetable, interpolate x-axis, since that's the phase
        // and that needs to be smooth to avoid aliasing.
        const static inline LookupTable<
            TableAxis{.size = 1000, .begin = 0, .end = 1, .interpolate = true },
            TableAxis{.size = 1000, .begin = 0, .end = 1 }
        > basict = [](double phase, double wtpos) {
            double p = phase;
            if (wtpos < 0.33)
            {
                const double r = wtpos * 3;
                return triangle(p, wtpos) * r + sine(p, wtpos) * (1 - r);
            }
            else if (wtpos < 0.66)
            {
                const double r = (wtpos - 0.33) * 3;
                return saw(p, wtpos) * r + triangle(p, wtpos) * (1 - r);
            }
            else
            {
                const double r = (wtpos - 0.66) * 2.9;
                return square(p, wtpos) * r + saw(p, wtpos) * (1 - r);
            }
        };

        // Basic wavetable combines sine, saw, square, and triangle into single wavetable.
        inline double basic(double phase, double wtpos) { return basict.get(phase, wtpos); }

        inline double sub(double phase, double wtpos)
        {   // Sub wavetable uses the simple shaper to add additional harmonics as its wtpos
            return Shapers::simpleshaper(sine(phase, wtpos), -wtpos * 0.5 + 0.5);
        }
    }

    // Simple ADSR envelope, with curve and level settings.
    class ADSR
    {
    public:
        double SAMPLE_RATE = 44100.;

        struct Settings
        {
            double attack = 0.02; // seconds
            double decay = 0.1;   // seconds
            double sustain = 1.0; // between 0 and 1
            double release = 0.1; // seconds

            double attackCurve = 0.5;  
            double decayCurve = 0.5;  
            double releaseCurve = 0.5; 

            double attackLevel = 0;
            double decayLevel = 1;

            double timeMult = 1;
            
            bool legato = false;
        } settings;

        ADSR(const Settings& s = {}) : settings(s) {};

        double Apply(double s, size_t) { return sample * s; }
        void Generate(size_t);
        double Offset(double p);
        void Trigger();
        void Gate(bool g);
        bool Gate() { return m_Gate; }
        bool Done() { return m_Phase == -1; }

        double sample = 0;

    private:
        double m_Down = 0;
        double m_Phase = -1;
        bool m_Gate = false;
    };

    // Specialized oscillator with several 
    // wave and phase shapers.
    class Oscillator
    {
    public:
        double SAMPLE_RATE = 44100.;

        struct Settings
        {
            double frequency = 440;
            double wtpos = 0;
            double sync = 0;
            double pw = 0.5;
            double bend = 0.5;
            double shaper = 0;
            double shaperMix = 0;
            double shaper2 = 0;
            double shaper2Mix = 0;
            double shaper3 = 0.5;
            double shaperMorph = 0;
        } settings;

        Oscillator(const Settings& s = {}) : settings(s) {}

        void Generate(size_t);
        double Offset(double phaseoffset); // Generate at phase offset, basic wavetable.
        double OffsetSimple(double phaseoffset); // Only simple shaper, at phase offset, basic wavetable.
        double OffsetClean(double phaseoffset); // No shapers, at phase offset, sub wavetable.

        double sample = 0;
        float phase = 0;
    };
}