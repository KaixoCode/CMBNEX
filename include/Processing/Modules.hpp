#pragma once
#include "pch.hpp"
#include "Processing/Filter.hpp"
#include "Utils/LookupTable.hpp"

namespace Kaixo
{
    // Convert note number to frequency, uses lookup table to be more efficient and fast.
    double noteToFreq(double note);

    // Wave shapers
    namespace Shapers
    {
        // No shaper, for convenience in interpolating shapers
        inline double noShaper(double x, double amt) { return x; }

        // Sine shaper, goes from (-1, -1) to (1, 1) like a normal waveshaper
        // x: input signal
        // r: percentage of frequency
        // m: frequency
        inline double SineShaper(double x, double r, double m)
        {
            constexpr static auto pi = std::numbers::pi_v<double>;
            return x * std::cos((r + 1 / m) * x * x * pi * m - r * pi * m - pi);
        }

        // Powered Sine shaper, goes from (-1, -1) to (1, 1) like a normal waveshaper
        inline double PoweredSineShaper(double x, double r, double m, int p)
        {
            return std::pow(SineShaper(x, r, m), p * 2. + 1);
        }

        inline double CenterExpander(double x)
        {
            return 2 - std::abs(x);
        }

        inline double SaturatorShaper(double x, double pow)
        {
            return std::tanh(x * (3 + pow));
        }

        inline double MinimizerShaper(double x, double pow, double pow2)
        {
            return SaturatorShaper(std::pow(std::abs(x), pow), pow2) * x;
        }

        // All these shapers contain 3 sine shapers with unique
        // tuneable frequencies.
        inline double Shaper01(double x, double freq)
        {
            const double coefs[3]{ 2, 9, 19 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = SaturatorShaper(x, 3);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.4 + p4 * 0.2;
        }

        inline double Shaper02(double x, double freq)
        {
            const double coefs[3]{ 39, 4, 17 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = SaturatorShaper(x, 5);
            return p1 * 0.6 + p2 * 0.1 + p3 * 0.1 + p4 * 0.2;
        }

        inline double Shaper03(double x, double freq)
        {
            const double coefs[3]{ 5, 13, 47 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 2) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 5);
            const double p4 = SaturatorShaper(x, 2);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.3 + p4 * 0.3;
        }

        inline double Shaper04(double x, double freq)
        {
            const double coefs[3]{ 6, 12, 63 };
            const double p1 = PoweredSineShaper(x, freq, coefs[0], 5) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 2) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 1) * CenterExpander(x);
            const double p4 = SaturatorShaper(x, 5);
            return p1 * 0.3 + p2 * 0.2 + p3 * 0.3 + p4 * 0.2;
        }

        inline double Shaper11(double x, double freq)
        {
            const double coefs[3]{ 24, 7, 21 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]);
            const double p4 = MinimizerShaper(x, 3, 2);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.4 + p4 * 0.2;
        }

        inline double Shaper12(double x, double freq)
        {
            const double coefs[3]{ 17, 16, 15 };
            const double p1 = SineShaper(x, freq, coefs[0]);
            const double p2 = SineShaper(x, freq, coefs[1]) * CenterExpander(x);
            const double p3 = SineShaper(x, freq, coefs[2]) * CenterExpander(x);
            const double p4 = MinimizerShaper(x, 5, 1);
            return p1 * 0.6 + p2 * 0.1 + p3 * 0.1 + p4 * 0.2;
        }

        inline double Shaper13(double x, double freq)
        {
            const double coefs[3]{ 8, 16, 32 };
            const double p1 = SineShaper(x, freq, coefs[0]) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 1) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 3);
            const double p4 = MinimizerShaper(x, 2, 5);
            return p1 * 0.2 + p2 * 0.2 + p3 * 0.3 + p4 * 0.3;
        }

        inline double Shaper14(double x, double freq)
        {
            const double coefs[3]{ 10, 31, 97 };
            const double p1 = PoweredSineShaper(x, freq, coefs[0], 7) * CenterExpander(x);
            const double p2 = PoweredSineShaper(x, freq, coefs[1], 1) * CenterExpander(x);
            const double p3 = PoweredSineShaper(x, freq, coefs[2], 4) * CenterExpander(x);
            const double p4 = MinimizerShaper(x, 5, 3);
            return p1 * 0.3 + p2 * 0.2 + p3 * 0.3 + p4 * 0.2;
        }

        // Main wave shaper, is an interpolating shaper, going over 5 shapers
        // and the ability to morph between an aditional 5 shapers.
        double mainWaveShaper(double x, double amt, double morph);
        const std::array<float, 4097 * 9 * 1025 + 1>& getMainWaveShaper();

        // Main phase shaper, the same as the wave shaper, except the output
        // is always constrained between 0 and 1.
        double mainPhaseShaper(double x, double amt, double morph);
        const std::array<float, 4097 * 9 * 1025 + 1>& getMainPhaseShaper();

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

        // Wavefolder, basically bounces back signal above 1 and below -1 so it folds
        // in on itself, creating aditional harmonics
        double fold(double x, double bias);
        const std::array<float, 100001 + 1>& getFold();

        // Simple drive algorithm with lookup table.
        double drive(double x, double gain, double amt);
        const std::array<float, 100001 * 2 + 1>& getDrive();

        // power curve interpolates between x^(1/16), x, and x^16, and makes
        // sure the output curve perfectly outputs between 0 and 1.
        double powerCurve(double x, double curve);
        const std::array<float, 1001 * 1001 + 1>& getPowerCurve();
    }

    using Wavetable = double(*)(double, double);
    namespace Wavetables
    {
        // Simple wavetables
        double sine(double phase);
        double saw(double phase, double f);
        double square(double phase, double f);
        double triangle(double phase, double f);

        // Basic wavetable combines sine, saw, square, and triangle into single wavetable.
        double basic(double phase, double wtpos, double f);
        const std::array<float, 2049 * 4 * 33 + 1>& getBasicTable();
        bool basicLoaded();

        inline double sub(double phase, double wtpos)
        {   // Sub wavetable uses the simple shaper to add additional harmonics as its wtpos
            return Shapers::simpleshaper(sine(phase), -wtpos * 0.5 + 0.5);
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

        double m_Down = 0;
        double m_Phase = -1;
        bool m_Gate = false;
        bool m_SustainPhase = false;
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
            double panning = 0;
        } settings;

        Oscillator(const Settings& s = {}) : settings(s) {}

        void Generate(size_t);
        double Offset(double phaseoffset, bool shaper, bool freeze); // Generate at phase offset, basic wavetable.
        double OffsetSimple(double phaseoffset); // Only simple shaper, at phase offset, basic wavetable.
        double OffsetClean(double phaseoffset); // No shapers, at phase offset, sub wavetable.

        double sample = 0;
        float phase = 0;
    };
}