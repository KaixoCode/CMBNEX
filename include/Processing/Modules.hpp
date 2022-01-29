#pragma once
#include "pch.hpp"
#include "Processing/Filter.hpp"

namespace Kaixo
{
    struct Table1
    {
        constexpr static int precision = 100000;
        constexpr static int start = -100;
        constexpr static int end = 136;
        constexpr Table1(auto&& l)
        {
            for (int i = 0; i < precision; i++)
                table[i] = l((i / (double)precision) * (end - start) + start);
        }

        inline double get(double val) const
        {
            if (val < start) return 0.08;
            if (val > end) return 22000;
            return table[(int)(precision * (val - start) / (end - start))];
        }

        double table[precision];
    };
    
    const static inline Table1 note2freq = [](double note) { return (440. / 32.) * pow(2, ((note - 9) / 12.0)); };

    inline double noteToFreq(double note) { return note2freq.get(note); }

    static inline unsigned long x = 123456789, y = 362436069, z = 521288629;
    inline double random() {
        
        unsigned long t;
        x ^= x << 16;
        x ^= x >> 5;
        x ^= x << 1;

        t = x;
        x = y;
        y = z;
        z = t ^ x ^ y;

        return 2 * (z % 32767) / 32767. - 0.5;
    }

    enum Polarity { Positive = 1, Negative = -1 };

    using Wavetable = double(*)(double, double);

    namespace Shapers
    {
        double noShaper(double x, double amt);
        double shaper0(double x, double amt);
        double shaper1(double x, double amt);
        double shaper2(double x, double amt);
        double shaper3(double x, double amt);
        double shaper6(double x, double amt);
        double shaper7(double x, double amt);
        double shaper8(double x, double amt);
        double shaper9(double x, double amt);
        double shaper4(double x, double amt, double morph);
        double shaper5(double x, double amt);
        double shaper24(double x, double amt, double morph);
        double simpleshaper(double x, double amt);
        double fold(double x, double bias);
        double drive(double x, double gain, double amt);
    }

    namespace Wavetables
    {
        double sine(double phase, double wtpos);
        double saw(double phase, double wtpos);
        double square(double phase, double wtpos);
        double triangle(double phase, double wtpos);
        double basic(double phase, double wtpos);
        double sub(double phase, double wtpos);
    }

    struct Range
    {
        double middle = 0;
        double range = 1;
    };

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

    class Oscillator
    {
    public:
        double SAMPLE_RATE = 44100.;

        struct Settings
        {
            double frequency = 440;
            double wtpos = 0;
            //int oversample = 8;
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
        double Apply(double s = 0, size_t = 0);
        double Offset(double phaseoffset);
        double OffsetOnce(double phaseoffset);
        double OffsetOnceLFO(double phaseoffset);
        double OffsetOnceClean(double phaseoffset);

        double sample = 0;
        float phase = 0;
    };
}