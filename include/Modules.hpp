#pragma once
#include "pch.hpp"
#include "Filter.hpp"

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

#define db2lin(db) std::powf(10.0f, 0.05 * (db))
#define lin2db(lin) (20.0f * std::log10f(static_cast<float>(lin)))

    enum Polarity { Positive = 1, Negative = -1 };

    using Wavetable = double(*)(double, double);

    namespace Shapers
    {
        double noShaper(double x, double amt);
        double shaper1(double x, double amt);
        double shaper2(double x, double amt);
        double shaper3(double x, double amt);
        double shaper4(double x, double amt);
        double shaper5(double x, double amt);
        double shaper24(double x, double amt);
        double simpleshaper(double x, double amt);
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

    class Module
    {
    public:
        double SAMPLE_RATE = 44100.;

        virtual double Apply(double sample = 0, size_t channel = 0) { return sample; };
        virtual void Generate(size_t channel = 0) {};
    };

    class Generator : public Module
    {
    public:
        operator double& () { return sample; }

        double sample = 0;
    };

    class Envelope : public Generator
    {
    public:
        virtual void Trigger() = 0;
        virtual void Gate(bool) = 0;
        virtual bool Done() { return true; }
    };

    class ADSR : public Envelope
    {
    public:
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

        double Apply(double s, size_t) override { return sample * s; }
        void Generate(size_t) override;
        double Offset(double p);
        void Trigger() override;
        void Gate(bool g) override;
        bool Gate() { return m_Gate; }
        bool Done() override { return m_Phase == -1; }

    private:
        double m_Down = 0;
        double m_Phase = -1;
        bool m_Gate = false;
    };

    class LPF : public Module
    {
    public:
        struct Settings
        {
            double frequency = 2000;
            double resonance = 1;
            double mix = 1;
        } settings;

        LPF(const Settings& s = {}) : settings(s) {}

        void Generate(size_t) override;
        double Apply(double s, size_t) override;

    private:
        BiquadParameters m_Params;
        StereoEqualizer<2, BiquadFilter<>> m_Filter{ m_Params };
    };

    class Oscillator : public Generator
    {
    public:
        struct Settings
        {
            double frequency = 440;
            double wtpos = 0;
            //int oversample = 8;
            double sync = 1;
            double pw = 0.5;
            double shaper = 0;
            double shaper2 = 0;
            double shaper3 = 0.5;
            Wavetable wavetable = Wavetables::basic;
        } settings;

        Oscillator(const Settings& s = {}) : settings(s) {}

        void Generate(size_t) override;
        double Apply(double s = 0, size_t = 0) override;
        double Offset(double phaseoffset);

        float phase = 0;
    private:
        //BiquadParameters m_Params;
        //BiquadFilter<> m_Filter[1];

        double OffsetOnce(double phaseoffset);
    };

    class Chorus : public Module
    {
    public:
        struct Settings
        {
            Oscillator oscillator;
            double mix = 0.5; // Percent
            double amount = 1;
            double feedback = 0; // Percent
            double delay1 = 5; // milliseconds
            double delay2 = 5; // milliseconds
            bool stereo = true;
            bool enableDelay2 = true;
            Polarity polarity = Negative;
        } settings;

        Chorus(const Settings& s = {}) : settings(s) {}

        void Channels(int c);
        double Apply(double sin, size_t c) override;

    private:
        constexpr static int BUFFER_SIZE = 2048;
        std::vector<std::vector<float>> m_Buffers;
        int m_Position = 0;
        int m_Delay1t = 5;
        int m_Delay2t = 5;
    };

    class Delay : public Module
    {
    public:
        struct Settings
        {
            double mix = 0.5; // Percent
            double delay = 500; // Milliseconds
            double feedback = 0.4; // Percent
            double gain = 0; // Input gain in decibel
            bool stereo = false;
            bool filter = true;
            struct {
                double amount = 0;
                double rate = 0.4;
            } mod;

        } settings;

        Delay(const Settings& s = {}) : settings(s) {}

        void Channels(int c);
        void Generate(size_t c) override;
        double Apply(double sin, size_t c) override;

    private:
        std::vector<std::vector<float>> m_Buffers;
        int BUFFER_SIZE = SAMPLE_RATE * 10;
        int m_Position = 0;

        Oscillator m_Oscillator{ {.wavetable = Wavetables::sine } };

        SimpleFilterParameters m_Parameters;
        std::vector<ChannelEqualizer<2, BiquadFilter<>>> m_Equalizers;

        bool m_Dragging = false;
    };
}